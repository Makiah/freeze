#pragma once

#include <string>
#include <fstream>
#include <functional>

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
		*/

		template<typename T> void melt(T& into)
		{
			if (std::is_same<T, int>::value)
			{

			}
			else if (std::is_base_of<Puddle, T>::value)
			{
				into.melt(*this);
			}
		}

		template<typename T> void freeze(T& data)
		{

		}
	};
}
