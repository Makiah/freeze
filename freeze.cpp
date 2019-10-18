#include "freeze.h"
#include <sstream>

void freeze::escapeReservedChars(std::string &s, const std::vector<char> &reservedCharacters) {
    for (size_t i = 0; i < s.size(); i++) {
        for (char c : reservedCharacters) {
            if (c == s[i] && (i == 0 || s[i - 1] != '\\')) {
                s.insert(i, "\\");
                break;
            }
        }
    }
}

void freeze::unescapeReservedChars(std::string &s, const std::vector<char> &reservedCharacters) {
    for (size_t i = 1; i < s.size(); i++) {
        for (char c : reservedCharacters) {
            if (c == s[i] && s[i - 1] == '\\') {
                s.erase(i - 1, 1);
                break;
            }
        }
    }
}

freeze::IceBlock freeze::IceBlock::fromFile(const std::string &path) {
    std::string frozenData = "";
    std::ifstream inFile(path);
    if (inFile.good()) {
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        frozenData = buffer.str();
        inFile.close();
    }

    return IceBlock(frozenData);
}

freeze::IceBlock::IceBlock(std::string frozenData) : frozenData(frozenData) {}

freeze::IceBlock::IceBlock() {}

bool freeze::IceBlock::empty() {
    return frozenData.empty();
}

std::string freeze::IceBlock::getFrozenData() {
    return frozenData;
}

std::string freeze::IceBlock::thawNewData() {
    bool inPrimitive = false;
    bool inString = false;
    int arrayDepth = 0;
    int blockDepth = 0;

    // Set entry parameters based on first char
    size_t i = 0;
    while (i == 0 || (inPrimitive || inString || arrayDepth > 0 || blockDepth > 0)) {
        assert(i < frozenData.size());
        assert(blockDepth >= 0 && arrayDepth >= 0);

        char c = frozenData[i];

        if (inString) {
            if (c == stringIndicator)
                inString = false;
        } else if (inPrimitive) {
            if (c == doubleIndicator || c == intIndicator || c == boolIndicator) {
                inPrimitive = false;
            }
        } else {
            if (c == blockStart) {
                blockDepth++;
            } else if (c == arrayStart) {
                arrayDepth++;
            } else if (c == blockEnd) {
                blockDepth--;
            } else if (c == arrayEnd) {
                arrayDepth--;
            } else if (c == stringIndicator && (i == 0 || frozenData[i - 1] == '\\')) {
                inString = true;
            } else if (c == doubleIndicator || c == intIndicator || c == boolIndicator) {
                inPrimitive = true;
            }
        }

        i++;
    }

    std::string chunked = frozenData.substr(1, i - 2); // remove indicators

    frozenData.erase(0, i);

    return chunked;
}

void freeze::IceBlock::save(const std::string &path) {
    std::ofstream outStream(path, std::ofstream::trunc | std::ofstream::out);
    assert(outStream);
    outStream << frozenData;
    outStream.close();
}
