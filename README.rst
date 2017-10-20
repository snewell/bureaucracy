bureaucracy
===========
|codacy|

bureaucracy is a simple library to distribute work among threads; at its
heart, bureaucracy is a threadpool with a bit of extra functionality.


Building
--------
bureaucracy uses CMake_ as a build engine.  CMake provides numerous options
that won't be covered here but in most cases the basic usage is sufficient.

1. :code:`mkdir <build_dir>`
2. :code:`cd <build_dir>`
3. :code:`cmake /path/to/bureaucracy/src`
4. :code:`make`
5. (Optional, requires gtest_) :code:`make test`
6. :code:`make install`

You'll need a toolchain that supports the C++14 standard (any recent versions
of gcc or clang should be sufficient).  If you're using a toolchain CMake
doesn't have built in support for you'll need to provide the appropriate
compiler flags at configuration time (replace step 3 with something like
:code:`cmake -DCMAKE_CXX_FLAGS="-std=c++14" /path/to/bureaucracy/src`).


Installation Paths
------------------
bureaucracy uses the CMake_ module GNUInstallDirs.  Specific folders can be
overwritten at configuration time (step 3 in the previous section).

.. _CMake: https://cmake.org/
.. _gtest: https://github.com/google/googletest

.. |codacy| image::
    https://api.codacy.com/project/badge/Grade/734e74a8f0c542c584e72721f22c81ca
    :target: https://www.codacy.com/app/snewell/bureaucracy?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=snewell/bureaucracy&amp;utm_campaign=Badge_Grade
