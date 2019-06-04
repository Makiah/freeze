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

freeze::IceBlock::IceBlock()
{}

bool freeze::IceBlock::empty()
{
	return frozenData.empty();
}

std::string freeze::IceBlock::thawNewData()
{
	bool inPrimitive = false;
	bool inString = false; 
	int arrayDepth = 0; 
	int blockDepth = 0;

	// Set entry parameters based on first char
	size_t i = 0;
	while (i == 0 || (inPrimitive || inString || arrayDepth > 0 || blockDepth > 0))
	{
		assert(i < frozenData.size());
		assert(blockDepth >= 0 && arrayDepth >= 0);

		char c = frozenData[i];

		if (inString)
		{
			if (c == stringIndicator)
				inString = false;
		}
		else if (inPrimitive)
		{
			if (c == doubleIndicator || c == intIndicator || c == boolIndicator)
			{
				inPrimitive = false;
			}
		}
		else 
		{
			if (c == blockStart)
			{
				blockDepth++;
			}
			else if (c == arrayStart)
			{
				arrayDepth++;
			}
			else if (c == blockEnd)
			{
				blockDepth--;
			}
			else if (c == arrayEnd)
			{
				arrayDepth--;
			}
			else if (c == stringIndicator && (i == 0 || frozenData[i - 1] == '\\'))
			{
				inString = true;
			}
			else if (c == doubleIndicator || c == intIndicator || c == boolIndicator)
			{
				inPrimitive = true;
			}
		}

		i++;
	}

	std::string chunked = frozenData.substr(1, i - 1); // remove indicators

	frozenData.erase(0, i + 1);
	
	return chunked;
}


void freeze::IceBlock::melt(int& into)
{
	assert(frozenData[0] == intIndicator);
	into = atoi(thawNewData().c_str());
}

void freeze::IceBlock::freeze(int data)
{
	frozenData += intIndicator;
	frozenData += std::to_string(data);
	frozenData += intIndicator;
}


void freeze::IceBlock::melt(double& into)
{
	assert(frozenData[0] == intIndicator);
	into = atof(thawNewData().c_str());
}

void freeze::IceBlock::freeze(double data)
{
	frozenData += doubleIndicator;
	frozenData += std::to_string(data);
	frozenData += doubleIndicator;
}


void freeze::IceBlock::melt(std::string& into)
{
	assert(frozenData[0] == stringIndicator);
	into.assign(thawNewData());
	unescapeReservedChars(into, { stringIndicator });
}

void freeze::IceBlock::freeze(const std::string& into)
{
	frozenData += stringIndicator;
	std::string dup = into;
	escapeReservedChars(dup, { stringIndicator });
	frozenData += dup;
	frozenData += stringIndicator;
}


void freeze::IceBlock::melt(Puddle* into)
{
	assert(frozenData[0] == blockStart);
	into->melt(IceBlock(thawNewData()));
}

void freeze::IceBlock::freeze(const Puddle* from)
{
	frozenData += blockStart;
	IceBlock dummy = IceBlock();
	from->freeze(dummy);
	frozenData += dummy.frozenData;
	frozenData += blockEnd;
}


void freeze::IceBlock::melt(std::vector<int>& into)
{
	meltVector<int>(into);
}

void freeze::IceBlock::freeze(const std::vector<int> data)
{
	freezeVector<int>(data);
}


void freeze::IceBlock::melt(std::vector<double>& into)
{
	meltVector<double>(into);
}

void freeze::IceBlock::freeze(const std::vector<double>& data)
{
	freezeVector<double>(data);
}


void freeze::IceBlock::melt(std::vector<std::string>& into)
{
	meltVector<std::string>(into);
}

void freeze::IceBlock::freeze(const std::vector<std::string>& into)
{
	freezeVector<std::string>(into);
}


void freeze::IceBlock::save(const std::string& path)
{
	std::ofstream outStream(path, std::ofstream::trunc | std::ofstream::out);
	assert(outStream);
	outStream << frozenData;
	outStream.close();
}
