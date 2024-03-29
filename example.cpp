// freeze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "freeze.h"

using namespace freeze;

class ClassA {
public:
    int id;

    ClassA(int id) : id(id) {}

    ClassA(IceBlock i) {
        id = i.melt<int>();
    }

    // Add data to IceBlock
    IceBlock freeze() const {
        IceBlock i;
        i.freeze(id);
        return i;
    }
};

class ClassBDerived {
public:
    std::string additionalData;

    ClassBDerived(std::string additionalData) : additionalData(additionalData) {}

    ClassBDerived(IceBlock i) {
        additionalData = i.melt<std::string>();
    }

    // Add data to IceBlock
    IceBlock freeze() const {
        IceBlock i;
        i.freeze(additionalData);
        return i;
    }
};

class ClassB {
public:
    std::string someData;
    std::vector<ClassBDerived> derivedNodes;

    ClassB(std::string someData) : someData(std::move(someData)) {}

    ClassB(IceBlock i) {
        someData = i.melt<std::string>();
        derivedNodes = i.melt<std::vector<ClassBDerived>>();
    }

    // Add data to IceBlock
    IceBlock freeze() const {
        IceBlock i;
        i.freeze(someData);
        i.freeze(derivedNodes);
        return i;
    }
};

void createIceBlock() {
    std::cout << "Creating block..." << std::endl;

    std::vector<int> someVector = {1, 2, 3, 4, 5};
    std::string someString = "Hi, I'm an IceBlock";
    ClassA someClass = ClassA(2);
    int someNumber = 5;
    std::vector<ClassB> someClassVector = {ClassB("1"), ClassB("2"), ClassB("3")};
    someClassVector[0].derivedNodes.push_back(ClassBDerived("4"));
    bool someBool = false;
    std::vector<std::vector<int>> twoDimensional = {{1, 2, 3},
                                                    {4, 5, 6}};

    IceBlock block;
    block.freeze(someVector);
    block.freeze(someString);
    block.freeze(someClass);
    block.freeze(someNumber);
    block.freeze(someClassVector);
    block.freeze(someBool);
    block.freeze(twoDimensional);

    block.save("frozen.txt");
}

void checkIceBlock() {
    // MUST be melted in the same order.

    std::cout << "Testing block..." << std::endl;

    IceBlock block = IceBlock::fromFile("frozen.txt");

    std::vector<int> someVector = block.melt<std::vector<int>>();
    std::string someString = block.melt<std::string>();
    ClassA someClass = block.melt<ClassA>();
    int someNumber = block.melt<int>();
    std::vector<ClassB> someClassVector = block.melt<std::vector<ClassB>>();
    bool someBool = block.melt<bool>();
    std::vector<std::vector<int>> twoDimensional = block.melt<std::vector<std::vector<int>>>();

    assert(someVector[0] == 1 && someVector[1] == 2 && someVector[2] == 3 && someVector[3] == 4 && someVector[4] == 5);
    assert(someString == "Hi, I'm an IceBlock");
    assert(someClass.id == 2);
    assert(someNumber == 5);
    assert(someClassVector[0].someData == "1" && someClassVector[1].someData == "2" &&
           someClassVector[2].someData == "3");
    assert(someClassVector[0].derivedNodes[0].additionalData == "4");
    assert(someBool == false);
    assert(twoDimensional[0][0] == 1 && twoDimensional[1][0] == 4);


    std::cout << "All good!" << std::endl;
}

void runExample() {
    createIceBlock();
    checkIceBlock();
}
