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
	constexpr char intStart = 'i';
	constexpr char doubleStart = 'd';

	void escapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);
	void unescapeReservedChars(std::string& s, const std::vector<char>& reservedCharacters);

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
		std::string frozenData;

		size_t iterateUntilContextFree(size_t i, bool inString, int arrayDepth, int blockDepth);

	public:
		static IceBlock fromFile(const std::string& path);

		IceBlock(std::string frozenData);

		bool empty();

		void melt(int& into);
		void freeze(int data);

		void melt(double& into);
		void freeze(double data);

		void melt(std::string& into);
		void freeze(std::string& into);

		void melt(Puddle* into);
		void freeze(Puddle* from);

		void melt(std::vector<int>& into);
		void freeze(std::vector<int> from);
	};
}
