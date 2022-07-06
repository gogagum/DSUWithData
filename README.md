# DSUWithData

Data structure with possibility to set your own data into set components.
Current version is `0.0.1`. Library is written in `C++20`.

Requirements:

- `g++-12` or `clang++-14`
- `CMake 23`

Library files are placed in `\lib`.

Main library classes are documented using Doxygen. To generate documentation
run:

    doxygen Doxyfile

You can build library and tests using the following commands:

    mkdir build
    cd build
    cmake -S ../ -B .
    cmake --build . --config Debug
