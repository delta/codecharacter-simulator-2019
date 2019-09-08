# Codecharacter Simulator 2019

## Setup Instructions

1. Download [Boost 1.68](https://www.boost.org/users/history/version_1_68_0.html)

2. `cd` to the Boost directory and build the system library by running `./bootstrap.sh --prefix=. --with-libraries=system && ./b2 cxxflags=-std=c++14 install`

3. `git clone --recursive https://github.com/delta/codecharacter-simulator.git`

4. Install [CMake](https://cmake.org/download/) (>= v3.11.1)

5. Install [Protobuf](https://github.com/google/protobuf/releases)

6. Install Clang++ (>=4.0)

7. `mkdir build && cd build`

8. `export LD_LIBRARY_PATH=<repo_install_location>/lib`

9. `cmake .. -DCMAKE_INSTALL_PREFIX=<repo_install_location> -DBOOST_ROOT=<boost_download_location> -DCMAKE_CXX_COMPILER=clang++`

10. `make install`


To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests.

