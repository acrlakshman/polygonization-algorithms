Overview {#mainpage}
========

[TOC]

## About

Implemention of scalar polygonization algorithms as part of learning experience targeted to extract surface of fluid interface in multiphase flow simulations.

## Source code

[Surface Polygonization][src]

## Installation

#### Dependencies

* [CMake]
* Compiler that supports C++11

```sh
   git clone https://github.com/acrlakshman/scalar-polygonization
   cd scalar-polygonization
   mkdir -p build && cd build
   cmake .. && make -j 4
```

### Additional build options

#### Dependencies

* [Doxygen]
* [lcov]

```sh
cmake .. -DBUILD_DOCUMENTATION=ON -DBUILD_COVERAGE=ON
make -j 4
./tests/sp_unit_tests
lcov --directory . --base-directory ../src --capture --no-external --output-file coverage.info
genhtml coverage.info --output-directory ./docs/html/coverage
```

* Documentation can be found at `<build-dir>/docs/html/index.html`.

## Coverage

[Coverage]

[src]:https://github.com/acrlakshman/scalar-polygonization
[CMake]:https://github.com/Kitware/CMake
[Doxygen]:https://github.com/doxygen/doxygen
[lcov]:https://github.com/linux-test-project/lcov
[Surface Polygonization]:https://acrlakshman.github.io/scalar-polygonization
[Coverage]:https://acrlakshman.github.io/scalar-polygonization/coverage
