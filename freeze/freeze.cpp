// freeze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "IceBlock.h"

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
		i.melt<std::string>(someData);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) override
	{
		i.freeze<std::string>(someData);
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
		i.melt<std::vector<DoublyDerivedClass>>(elements);
	}

	// Add data to IceBlock
	void freeze(IceBlock& i) override
	{
		i.freeze<std::vector<DoublyDerivedClass>>(elements);
	}
};

// I actually HAVE to make a single line save unless I want to somehow overload vectors, because treating vectors as primitives fucks everything up completely since you can have a vector of subclasses

int main()
{
	std::vector<unsigned int> someVector = { 1, 2, 3, 4, 5 };
	std::string someString = "Hi, I'm an IceBlock";
	bool someBoolean = false;
	DerivedClass someClass = DerivedClass();

	// Example freezing
	IceBlock block = IceBlock("frozen.txt");
	block.freeze<std::vector<unsigned int>>(someVector);
	block.freeze<std::string>(someString);
	block.freeze<bool>(someBoolean);
	block.freeze<DerivedClass>(someClass);

	// Presumably some time passes
	
	// Example melting (must occur in same order)
	IceBlock block2 = IceBlock("frozen.txt");
	block2.melt<std::vector<unsigned int>>(someVector);
	block2.melt<std::string>(someString);
	block2.melt<bool>(someBoolean);
	block2.melt<DerivedClass>(someClass);
}
