#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <cassert>

namespace freeze
{
	constexpr char blockStart = '(';
	constexpr char blockEnd = ')';
	constexpr char arrayStart = '[';
	constexpr char arrayEnd = ']';
	constexpr char stringIndicator = '"';
	constexpr char intIndicator = 'i';
	constexpr char doubleIndicator = 'd';

	void escapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);
	void unescapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);

	class IceBlock;

	class Puddle
	{
	public:
		// Remove data from IceBlock and pass on to additional steps (if there are any)
		virtual void melt(IceBlock i) = 0;

		// Add data to IceBlock
		virtual void freeze(IceBlock& i) const = 0;
	};

	class IceBlock
	{
	private:
		std::string frozenData;

		std::string thawNewData();

		template <typename T> void meltVector(std::vector<T>& into)
		{
			assert(frozenData[0] == arrayStart);
			IceBlock arrayBlock = IceBlock(thawNewData());

			unsigned int index = 0;
			while (!arrayBlock.empty())
			{
				T& item = into[index];
				arrayBlock.melt(item);
				index++;
			}
		}

		template <typename T> void freezeVector(const std::vector<T>& from)
		{
			frozenData += arrayStart;
			for (T i : from)
				freeze(i);
			frozenData += arrayEnd;
		}

	public:
		static IceBlock fromFile(const std::string& path);

		IceBlock(std::string frozenData);
		IceBlock();

		bool empty();


		// Not currently supported because boolean out-parameters aren't supported (bool& test = vector[0];) fails
		//void melt(bool& into);
		//void freeze(const bool data);

		void melt(int& into);
		void freeze(const int data);

		void melt(double& into);
		void freeze(const double data);

		void melt(std::string& into);
		void freeze(const std::string& into);

		void melt(Puddle* into);
		void freeze(const Puddle* from);


		// Could simply provide templated argument but this is easier for the end user
		void melt(std::vector<bool>& into);
		void freeze(const std::vector<bool>& data);

		void melt(std::vector<int>& into);
		void freeze(const std::vector<int> data);

		void melt(std::vector<double>& into);
		void freeze(const std::vector<double>& data);

		void melt(std::vector<std::string>& into);
		void freeze(const std::vector<std::string>& from);

		template<typename T> void meltPuddles(std::vector<T>& into)
		{
			assert(frozenData[0] == arrayStart);
			IceBlock arrayBlock = IceBlock(thawNewData());

			unsigned int index = 0;
			while (!arrayBlock.empty())
			{
				Puddle* item = &into[index];
				arrayBlock.melt(item);
				index++;
			}
		}

		template<typename T> void freezePuddles(const std::vector<T>& from)
		{
			frozenData += arrayStart;
			for (const T& item : from)
			{
				const Puddle* p = &item; // have to use implicit conversion
				freeze(p);
			}
			frozenData += arrayEnd;
		}


		void save(const std::string& path);
	};
}
