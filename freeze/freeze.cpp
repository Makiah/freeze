#include "freeze.h"

void freeze::escapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		for (char c : reservedCharacters)
		{
			if (c == s[i] && (i == 0 || s[i - 1] != '\\'))
			{
				s.insert(i, "\\");
				break;
			}
		}
	}
}

void freeze::unescapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters)
{
	for (size_t i = 1; i < s.size(); i++)
	{
		for (char c : reservedCharacters)
		{
			if (c == s[i] && s[i - 1] == '\\')
			{
				s.erase(i - 1, 1);
				break;
			}
		}
	}
}

freeze::IceBlock freeze::IceBlock::fromFile(const std::string& path)
{
	std::string frozenData = "";
	std::ifstream inFile(path);
	if (inFile.good())
	{
		// Load from file
		inFile >> frozenData; // a single line so this is the extent of the data
		inFile.close();
	}

	return IceBlock(frozenData);
}

freeze::IceBlock::IceBlock(std::string frozenData) : frozenData(frozenData)
{}

freeze::IceBlock::empty()
{
	return frozenData.empty();
}

void freeze::IceBlock::melt(int& into)
{
	assert(frozenData[0] == intStart);

	size_t i = 2;
	while (true)
	{
		assert(i < frozenData.size());

		for (char c : {stringIndicator, intStart, doubleStart, arrayStart, blockStart})
			if (frozenData[i] == c)
				break;

		i++;
	}

	into = atoi(frozenData.substr(1, i - 1).c_str());

	frozenData.erase(0, i);
}

void freeze::IceBlock::freeze(int data)
{
	frozenData += intStart;
	frozenData += std::to_string(data);
}


void freeze::IceBlock::melt(double& into)
{
	assert(frozenData[0] == doubleStart);

	size_t i = 2;
	while (true)
	{
		assert(i < frozenData.size());

		for (char c : {stringIndicator, intStart, doubleStart, arrayStart, blockStart})
			if (frozenData[i] == c)
				break;

		i++;
	}

	into = atof(frozenData.substr(1, i - 1).c_str());

	frozenData.erase(0, i);
}

void freeze::IceBlock::freeze(double data)
{
	frozenData += doubleStart;
	frozenData += std::to_string(data);
}


void freeze::IceBlock::melt(std::string& into)
{
	assert(frozenData[0] == stringIndicator);

	size_t i = 1;
	while (true)
	{
		assert(i < frozenData.size());
		if (frozenData[i] == stringIndicator && frozenData[i - 1] != '\\')
			break;
		i++;
	}

	into.assign(frozenData.substr(1, i - 1));
	unescapeReservedChars(into, { stringIndicator });

	frozenData.erase(0, i + 1);
}

void freeze::IceBlock::freeze(std::string& into)
{
	frozenData += stringIndicator;
	escapeReservedChars(into, { stringIndicator });
	frozenData += into;
	frozenData += stringIndicator;
}

size_t freeze::IceBlock::iterateUntilContextFree(size_t i, bool inString, int arrayDepth, int blockDepth)
{
	while (inString || arrayDepth > 0 || blockDepth > 0)
	{
		assert(i < frozenData.size());
		assert(blockDepth >= 0 && arrayDepth >= 0);

		if (!inString)
		{
			if (frozenData[i] == blockStart)
			{
				blockDepth++;
			}
			else if (frozenData[i] == arrayStart)
			{
				arrayDepth++;
			}
			else if (frozenData[i] == blockEnd)
			{
				blockDepth--;
			}
			else if (frozenData[i] == arrayEnd)
			{
				arrayDepth--;
			}
			else if (frozenData[i] == stringIndicator)
			{
				inString = true;
			}
		}
		else
		{
			if (frozenData[i] == stringIndicator)
				inString = false;
		}

		i++;
	}

	return i;
}


void freeze::IceBlock::melt(Puddle* into)
{
	assert(frozenData[0] == blockStart);

	size_t i = iterateUntilContextFree(1, false, 0, 1);

	IceBlock block = IceBlock(frozenData.substr(1, i - 1));
	into->melt(block);

	frozenData.erase(0, i + 1);
}

void freeze::IceBlock::freeze(Puddle* from)
{
	frozenData += blockStart;
	IceBlock dummy = IceBlock("");
	from->freeze(dummy);
	frozenData += dummy.frozenData;
	frozenData += blockEnd;
}


void freeze::IceBlock::melt(std::vector<int>& into)
{
	assert(frozenData[0] == blockStart);

	size_t i = iterateUntilContextFree(1, false, 1, 0);

	unsigned int index = 0;
	IceBlock block = IceBlock(frozenData.substr(1, i - 1));
	while (!block.empty())
	{
		melt(into[index]);
		index++;
	}

	frozenData.erase(0, i + 1);
}

void freeze::IceBlock::freeze(std::vector<int> from)
{
	frozenData += blockStart;
	IceBlock dummy = IceBlock("");
	from->freeze(dummy);
	frozenData += dummy.frozenData;
	frozenData += blockEnd;
}
