##Implementation of polygonization algorithms

[![Build Status](https://travis-ci.org/acrlakshman/surface-polygonization.svg?branch=master)](https://travis-ci.org/acrlakshman/surface-polygonization)
[![Coverage Status](https://coveralls.io/repos/github/acrlakshman/surface-polygonization/badge.svg)](https://coveralls.io/github/acrlakshman/surface-polygonization)

Implemention of surface polygonization algorithms as part of learning experience targeted to extract surface of fluid interface in multiphase flow simulations.

* Marching cubes
  * Ref.: http://paulbourke.net/geometry/polygonise/

### Build instructions

#### Dependencies

* [CMake]
* Compiler that supports C++11

```sh
git clone https://github.com/acrlakshman/surface-polygonization --recursive
cd surface-polygonization
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

* Documentation can be found at `./docs/html/index.html`.

### Documentation

* [Documentation]
* [Coverage]

License
-------

BSD 3-Clause License. Please check the accompanying [LICENSE] file

[CMake]:https://github.com/Kitware/CMake
[Doxygen]:https://github.com/doxygen/doxygen
[lcov]:https://github.com/linux-test-project/lcov
[Documentation]:https://acrlakshman.github.io/surface-polygonization
[Coverage]:https://acrlakshman.github.io/surface-polygonization/coverage
[LICENSE]:https://github.com/acrlakshman/surface-polygonization/blob/master/LICENSE
