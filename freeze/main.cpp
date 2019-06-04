// freeze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "freeze.h"

using namespace freeze;

class ClassA : public Puddle
{
public:
	int id;

	ClassA(int id) : id(id) {}

	ClassA() : id(0) {}

	// Remove data from IceBlock and pass on to additional steps (if there are any)
	void melt(IceBlock i) override
	{
		i.melt(id);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) const override
	{
		i.freeze(id);
	}
};

class ClassB : public Puddle
{
public:
	std::string someData;

	ClassB(std::string someData) : someData(std::move(someData)) {}
	ClassB() : someData(std::move("")) {}

	// Remove data from IceBlock and pass on to additional steps (if there are any)
	void melt(IceBlock i) override
	{
		i.melt(someData);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) const override
	{
		i.freeze(someData);
	}
};

void createIceBlock()
{
	std::cout << "Creating block..." << std::endl;

	std::vector<int> someVector = { 1, 2, 3, 4, 5 };
	std::string someString = "Hi, I'm an IceBlock";
	ClassA someClass = ClassA(2);
	int someNumber = 5;
	std::vector<ClassB> someClassVector = { ClassB("1"), ClassB("2"), ClassB("3") };

	IceBlock block;
	block.freeze(someVector);
	block.freeze(someString);
	block.freeze(&someClass);
	block.freeze(someNumber);
	block.freezePuddles<ClassB>(someClassVector);

	block.save("frozen.txt");
}

void checkIceBlock()
{
	// MUST be melted in the same order.

	std::cout << "Testing block..." << std::endl;

	std::vector<int> someVector = { 0, 0, 0, 0, 0 };
	std::string someString = "";
	ClassA someClass = ClassA();
	int someNumber = 0;
	std::vector<ClassB> someClassVector = { ClassB(), ClassB(), ClassB() };

	IceBlock block = IceBlock::fromFile("frozen.txt");
	
	block.melt(someVector);
	assert(someVector[0] == 1 && someVector[1] == 2 && someVector[2] == 3 && someVector[3] == 4 && someVector[4] == 5);
	
	block.melt(someString);
	assert(someString == "Hi, I'm an IceBlock");

	block.melt(&someClass);
	assert(someClass.id == 2);

	block.melt(someNumber);
	assert(someNumber == 5);

	block.meltPuddles<ClassB>(someClassVector);
	assert(someClassVector[0].someData == "1" && someClassVector[1].someData == "2" && someClassVector[2].someData == "3");

	std::cout << "All good!" << std::endl;
}

int main()
{
	createIceBlock();
	checkIceBlock();
}
