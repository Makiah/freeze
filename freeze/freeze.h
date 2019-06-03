#pragma once

#include <string>
#include <fstream>
#include <functional>
#include <cassert>

namespace freeze
{
	class IceBlock;

	class Puddle
	{
	public:
		// Remove data from IceBlock and pass on to additional steps (if there are any)
		virtual void melt(IceBlock& i) = 0;

		// Add data to IceBlock
		virtual void freeze(IceBlock& i) = 0;
	};

	class IceBlock
	{
	private:
		std::ofstream outFile;

		std::string frozenData;

	public:
		IceBlock(std::string path)
		{
			std::ifstream inFile(path);
			if (inFile.good())
			{
				// Load from file
				inFile >> frozenData; // a single line so this is the extent of the data
				inFile.close();
			}

			// Create file
			outFile.open(path);
		}

		/*
		(...) = IceBlock
		[...] = iterable
		i... = int
		d... = double
		"..." = string
		, = general delimeter
		*/

		void handleDelim()
		{
			if (frozenData.size() > 0)
				frozenData.append(",");
		}

		void melt(unsigned int& into)
		{
		}

		void freeze(unsigned int& data)
		{
			handleDelim();
		}


		void melt(std::string& into)
		{

		}

		void freeze(std::string& into)
		{
			handleDelim();
		}


		void melt(Puddle* into)
		{
			assert(frozenData[0] == '(' && frozenData[frozenData.size() - 1] == ')');

			into->melt(*this);
		}

		void freeze(Puddle* from)
		{
			handleDelim();
			frozenData.append("(");
			from->freeze(*this);
			frozenData.append(")");
		}


	};
}
