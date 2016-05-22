# Vector from scratch

Build a std::vector compatible vector from scratch.

for more info: http://www.sven-johannsen.de/slides/vector_from_scratch201605/index.html#/title-page

The project contain 2 targets:

### demo

Contail all single steps to create a more less complete vector container.

### vector_test

3 unit tests:

* `vector_test.cpp`
* `iterator_test.cpp`
* `allocator_test.cpp`

The file `vector_test.cpp` runs all tests agains the `std::vector` and `std2::vector` to compare the demo code with standard library of the compiler. 

## Dependencies

### Catch

Download the single file header from: https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp

Copy `catch.hpp` into the folder `extern/catch`.

### CMake

minimal version: 2.8 

## How to build

```
mkdir build
cd build
cmake ..
```

## TODO

* insert for multiple elements
* more tests



