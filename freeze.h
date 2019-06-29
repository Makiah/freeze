#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <map>

namespace freeze
{
	constexpr char blockStart = '(';
	constexpr char blockEnd = ')';
	constexpr char arrayStart = '[';
	constexpr char arrayEnd = ']';
	constexpr char stringIndicator = '"';
	constexpr char intIndicator = 'i';
	constexpr char doubleIndicator = 'd';
	constexpr char boolIndicator = 'b';

	void escapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);
	void unescapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);

	class IceBlock
	{
	private:
		std::string frozenData;

		std::string thawNewData();

		template<typename T> T meltItem()
		{
			assert(frozenData[0] == blockStart);
			return T(IceBlock(thawNewData()));
		}

		template<> bool meltItem<bool>()
		{
			assert(frozenData[0] == boolIndicator);
			return thawNewData()[0] == 't';
		}

		template<> int meltItem<int>()
		{
			assert(frozenData[0] == intIndicator);
			return atoi(thawNewData().c_str());
		}

		// TODO see if above can be merged since literally same code
		template<> unsigned int meltItem<unsigned int>()
		{
			assert(frozenData[0] == intIndicator);
			return atoi(thawNewData().c_str());
		}

		template<> double meltItem<double>()
		{
			assert(frozenData[0] == doubleIndicator);
			return atof(thawNewData().c_str());
		}

		template<> std::string meltItem<std::string>()
		{
			assert(frozenData[0] == stringIndicator);
			std::string data = thawNewData();
			unescapeReservedChars(data, { stringIndicator });
			return data;
		}

		template<typename T> std::vector<T> meltVector()
		{
			assert(frozenData[0] == arrayStart);
			IceBlock arrayBlock = IceBlock(thawNewData());

			std::vector<T> toReturn;
			while (!arrayBlock.empty())
				toReturn.push_back(arrayBlock.melt<T>());
			return toReturn;
		}

        template <typename T, size_t size> std::array<T, size> meltArray()
        {
            assert(frozenData[0] == arrayStart);
            IceBlock arrayBlock = IceBlock(thawNewData());

            std::array<T, size> toReturn;
            for (unsigned int i = 0; i < size; i++)
                toReturn.at(i) = arrayBlock.melt<T>();
            return toReturn;
        }

        template <typename T, typename U> std::pair<T, U> meltPair()
        {
            assert(frozenData[0] == arrayStart);
            IceBlock arrayBlock = IceBlock(thawNewData());
            return std::pair<T, U>{arrayBlock.melt<T>(), arrayBlock.melt<U>()};
        }

        template<typename T, typename U> std::map<T, U> meltMap()
        {
            assert(frozenData[0] = arrayStart);
            IceBlock mapBlock = IceBlock(thawNewData());
            std::map<T, U> toReturn;
            while (!mapBlock.empty())
                toReturn.insert(meltPair<T, U>());
            return toReturn;
        }

	public:
		static IceBlock fromFile(const std::string & path);

		IceBlock(std::string frozenData);
		IceBlock();

		bool empty();

        std::string getFrozenData();

		// Yeah yeah this feels hella jank but C++ doesn't support partial template specialization so this is the next best thing
		template<typename T> T melt()
		{
			return melt_impl(static_cast<T*>(0));
		}
		template<typename T> T melt_impl(T*)
		{
			return meltItem<T>();
        }
		template<typename T> std::vector<T> melt_impl(std::vector<T>*)
        {
			return meltVector<T>();
        }
        template<typename T, size_t size> std::array<T, size> melt_impl(std::array<T, size>*)
        {
            return meltArray<T, size>();
        }
        template<typename T, typename U> std::pair<T, U> melt_impl(std::pair<T, U>*)
        {
            return meltPair<T, U>();
        }
        template<typename T, typename U> std::map<T, U> melt_impl(std::map<T, U>*)
        {
            return meltMap<T, U>();
        }

		// See above
		template<typename T> void freeze(const T & item)
		{
			frozenData += blockStart;
			frozenData += item.freeze().frozenData;
			frozenData += blockEnd;
		}

		template<> void freeze<std::string>(const std::string & item)
		{
			frozenData += stringIndicator;
			std::string dup = item;
			escapeReservedChars(dup, { stringIndicator });
			frozenData += dup;
			frozenData += stringIndicator;
		}

		template<> void freeze<double>(const double& item)
		{
			frozenData += doubleIndicator;
			frozenData += std::to_string(item);
			frozenData += doubleIndicator;
		}

		template<> void freeze<unsigned int>(const unsigned int& item)
		{
			frozenData += intIndicator;
			frozenData += std::to_string(item);
			frozenData += intIndicator;
		}

		template<> void freeze<int>(const int& item)
		{
			frozenData += intIndicator;
			frozenData += std::to_string(item);
			frozenData += intIndicator;
		}

		template<> void freeze<bool>(const bool& item)
		{
			frozenData += boolIndicator;
			frozenData += (item ? 't' : 'f');
			frozenData += boolIndicator;
		}

		// Works bc overloading
		template<typename T> void freeze(const std::vector<T> & from)
		{
			frozenData += arrayStart;
			for (const T& item : from)
				freeze(item);
			frozenData += arrayEnd;
		}

        template <typename T, size_t size> void freeze(const std::array<T, size>& from)
        {
            frozenData += arrayStart;
            for (const T& item : from)
                freeze(item);
            frozenData += arrayEnd;
        }

        template <typename T, typename U> void freeze(const std::pair<T, U>& item)
        {
            frozenData += arrayStart;
            freeze(item.first);
            freeze(item.second);
            frozenData += arrayEnd;
        }

        template <typename T, typename U> void freeze(const std::map<T, U>& from)
        {
            frozenData += arrayStart;
            for (const std::pair<T, U>& pair : from)
                freeze(pair);
            frozenData += arrayEnd;
        }


		void save(const std::string & path);
	};
}
