# bureaucracy
bureaucracy is a simple library to distribute work among threads; at its heart
bureaucracy is a threadpool with a bit of extra functionality.

## Building
bureaucracy uses [CMake][1] as a build engine.  CMake provides numerous
options that won't be covered her but in most cases the basic usage is
sufficient.

1. `mkdir <build_dir>`
2. cd <build_dir>
3. `cmake /path/to/bureaucracy/src`
4. `make`
5. (Optional, requires [gtest][2]) `make test`
6. `make install`

You'll need a toolchain that supports the C++14 standard (any recent versions
of gcc or clang should be sufficient).  If you're using a toolchain CMake
doesn't have built in support for you'll need to provide the appropriate
compiler flags at configuration time (replced step 3 with something like
`cmake -DCMAKE_CXX_FLAGS="-std=c++14" /path/to/bureaucracy/src`).

[1]: https://cmake.org/ "CMake"
[2]: https://github.com/google/googletest "gtest"

