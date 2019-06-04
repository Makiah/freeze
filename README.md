# freeze
☃️ Quick and dirty method of concisely loading and saving the properties of arbitrarily complex class structures in a text file, provided loading occurs in a known order.  

## Usage 

See `main.cpp`.  

`freeze::IceBlock` holds stored class data. 
* Load from file via `IceBlock::fromFile(path)`, and save via `block.save(path)`.  
* Unload from and load data to classes via `melt` and `freeze` respectively. 
* For iterables of classes, use `meltPuddles<T>` and `freezePuddles<T>`

`freeze::Puddle` handles any classes which have reloadable data should extend this
* To use, overload `melt(IceBlock)` and `freeze(IceBlock&)`.  

## Building

Copy `freeze.h` and `freeze.cpp` somewhere into your project and `#include "freeze.h"` wherever.  
