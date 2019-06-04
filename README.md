# freeze
☃️ Quick method of concisely loading and saving the properties of arbitrarily complex class structures in a text file, provided loading occurs in a known order.  

## Usage 

### Basic

```
// data
int someNum = 2;
std::string someString = "string";
bool someBool = false;
std::vector<std::vector<ClassA>> twod = {{ClassA(1), ClassA(2)}, {ClassA(3)}};

freeze::IceBlock i;
i.freeze(someNum);
i.freeze(someString);
i.freeze(someBool);
i.freeze(twod);
i.save("resources/ice.txt");

// ... process exits, new run (order dependent loading)

freeze::IceBlock i = freeze::IceBlock::fromFile("resources/ice.txt");

int someNum = i.melt<int>();
std::string someString = i.melt<std::string>();
bool someBool = i.melt<bool>();
std::vector<std::vector<ClassA>> twod = i.melt<std::vector<std::vector<ClassA>>();

// proceed with data from last run
```

### Advanced

See `main.cpp`.

## Building

Copy `freeze.h` and `freeze.cpp` somewhere into your project and `#include "freeze.h"` wherever.  
