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
	void melt(IceBlock& i) override
	{
		i.melt(someData);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) override
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
	void melt(IceBlock& i) override
	{
		i.melt(elements);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) override
	{
		i.freeze(elements);
	}
};

// I actually HAVE to make a single line save unless I want to somehow overload vectors, because treating vectors as primitives fucks everything up completely since you can have a vector of subclasses

int main()
{
	std::vector<unsigned int> someVector = { 1, 2, 3, 4, 5 };
	std::string someString = "Hi, I'm an IceBlock";
	bool someBoolean = false;
	DerivedClass someClass = DerivedClass();
	unsigned int someNumber = 5;

	// Example freezing
	IceBlock block = IceBlock::fromFile("frozen.txt");
	block.freeze(someVector);
	block.freeze(someString);
	block.freeze(someBoolean);
	block.freeze(&someClass);
	block.freeze(someNumber);

	// Presumably some time passes
	
	// Example melting (must occur in same order)
	IceBlock block2 = IceBlock::fromFile("frozen.txt");
	block2.melt(someVector);
	block2.melt(someString);
	block2.melt(someBoolean);
	block2.melt(&someClass);
	block2.melt(someNumber);
}
