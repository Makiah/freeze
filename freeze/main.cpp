// freeze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "freeze.h"

using namespace freeze;

class DoublyDerivedClass : public Puddle
{
private:
	std::string someData;

public:
	DoublyDerivedClass(std::string someData) : someData(std::move(someData))
	{}

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

class DerivedClass : public Puddle
{
private: 
	std::vector<DoublyDerivedClass> elements;

public:
	DerivedClass() {}

	// Remove data from IceBlock and pass on to additional steps (if there are any)
	void melt(IceBlock i) override
	{
		i.meltPuddles<DoublyDerivedClass>(elements);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) const override
	{
		i.freezePuddles<DoublyDerivedClass>(elements);
	}
};

void createIceBlock()
{
	std::vector<int> someVector = { 1, 2, 3, 4, 5 };
	std::string someString = "Hi, I'm an IceBlock";
	DerivedClass someClass = DerivedClass();
	int someNumber = 5;

	IceBlock block;
	block.freeze(someVector);
	block.freeze(someString);
	block.freeze(&someClass);
	block.freeze(someNumber);
	block.save("frozen.txt");
}

void checkIceBlock()
{
	std::vector<int> someVector = { 0, 0, 0, 0, 0 };
	std::string someString = "";
	DerivedClass someClass = DerivedClass();
	int someNumber = 0;

	IceBlock block = IceBlock::fromFile("frozen.txt");
	block.melt(someVector);
	block.melt(someString);
	block.melt(&someClass);
	block.melt(someNumber);
}

int main()
{
	createIceBlock();
	checkIceBlock();
}
