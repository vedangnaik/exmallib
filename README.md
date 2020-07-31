# exmallib: Exception's malloc library {#mainpage} 

# Introduction
The exmallib library provides an interface to UnhandledException's personal implementations of malloc (exmalloc), free (exfree), realloc (exrealloc), and calloc (excalloc), along with a custom (albeit simple) heap allocator. exmalloc, exrealloc, and excalloc provide chunk-aligned memory blocks closest in size to the user's request, while reusing as much space as possible and dealing with memory fragmentation in the background.

# Purpose
This library is the result of my quest to learn more about the internals of C and memory management on Linux systems. I suppose it could also be used as a starting point for the construction of memory allocators on other systems as well.

# Usage
Simply add `exmallib.h`, `exmallib.c`, and `helpers.c` to your project! You could also separately compile these and link the library after the fact: see the Build section to see how.

# Build
## The code
`cmake` is required to build the code. Otherwise you can build it manually using `gcc` or any other C99+ compiler. With `cmake`, run the following commands to set up the build system:
    
    git clone https://github.com/vedangnaik/exmallib.git
    cd exmallib
    mkdir build
    cd build
    cmake .

Run `cmake --build ..` directly after to build the code. The output file is placed in `/build`.

## The docs
`doxygen` is required to build the documentation. Run the following commands from `/exmallib` to build it. The config file `Doxyfile` can be modified as per your requirements.

    mkdir docs
    doxygen Doxyfile

    