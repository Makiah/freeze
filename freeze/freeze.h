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
	constexpr char boolIndicator = 'd';

	void escapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);
	void unescapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);

	class IceBlock
	{
	private:
		std::string frozenData;

		std::string thawNewData();

	public:
		static IceBlock fromFile(const std::string& path);

		IceBlock(std::string frozenData);
		IceBlock();

		bool empty();


		// Not currently supported because boolean out-parameters aren't supported (bool& test = vector[0];) fails
		//void melt(bool& into);
		//void freeze(const bool data);

		template<typename T> T melt()
		{
			assert(frozenData[0] == blockStart);
			return T(IceBlock(thawNewData()));
		}

		template<typename T> void freeze(const T& item)
		{
			frozenData += blockStart;
			frozenData += item.freeze().frozenData;
			frozenData += blockEnd;
		}

		
		template<> bool melt<bool>()
		{
			assert(frozenData[0] == boolIndicator);
			return thawNewData()[0] == 't';
		}

		template<> void freeze<bool>(const bool& item)
		{
			frozenData += boolIndicator;
			frozenData += (item ? 't' : 'f');
			frozenData += boolIndicator;
		}


		template<> int melt<int>()
		{
			assert(frozenData[0] == intIndicator);
			return atoi(thawNewData().c_str());
		}

		template<> void freeze<int>(const int& item)
		{
			frozenData += intIndicator;
			frozenData += std::to_string(item);
			frozenData += intIndicator;
		}


		template<> double melt<double>()
		{
			assert(frozenData[0] == doubleIndicator);
			return atof(thawNewData().c_str());
		}

		template<> void freeze<double>(const double& item)
		{
			frozenData += doubleIndicator;
			frozenData += std::to_string(item);
			frozenData += doubleIndicator;
		}


		template<> std::string melt<std::string>()
		{
			assert(frozenData[0] == stringIndicator);
			std::string data = thawNewData();
			unescapeReservedChars(data, { stringIndicator });
			return data;
		}

		template<> void freeze<std::string>(const std::string& item)
		{
			frozenData += stringIndicator;
			std::string dup = item;
			escapeReservedChars(dup, { stringIndicator });
			frozenData += dup;
			frozenData += stringIndicator;
		}


		// name change might be avoidable by using some sort of subtemplate but off wifi so idk
		template<typename T> std::vector<T> meltVector()
		{
			assert(frozenData[0] == arrayStart);
			IceBlock arrayBlock = IceBlock(thawNewData());

			std::vector<T> toReturn;
			while (!arrayBlock.empty())
				toReturn.push_back(arrayBlock.melt<T>());
			return toReturn;
		}

		template<typename T> void freezeVector(const std::vector<T>& from)
		{
			frozenData += arrayStart;
			for (const T& item : from)
				freeze(item);
			frozenData += arrayEnd;
		}


		void save(const std::string& path);
	};
}
