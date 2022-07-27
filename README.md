# DSUWithData

Data structure with possibility to set your own data into set components.
Current version is `0.0.4`. Library is written in `C++20`.

Requirements:

- `g++-12` or `clang++-14`
- `libbost-1.79`
- `CMake 22`

Library files are placed in `\lib`.

Main library classes are documented using Doxygen. To generate documentation
run:

    doxygen Doxyfile

You can build library and tests using the following commands:

    mkdir build
    cd build
    cmake -S ../ -B .
    cmake --build . --config Debug

To include library into your project you can add it as submodule.
After that you should write the following in your CMakeLists.txt:

    ...
    add_subdirectory(path/to/submodule/DSUWithData/lib)
    ...
    target_link_libraries(project_name DSUWithData)
    