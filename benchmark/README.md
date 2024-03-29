# Description
This project contains benchmarks for testing data structures and algorithms taken from external libraries.

The goal is to determine the most effective and convenient of them, and then use them in the project to develop a pipeline for processing multidimensional data

## The project is build using CMake
```
# /* clone the repository, cd to directory */

mkdir build
cd build
cmake ..
sudo make -j4 install   # or use another generator
```

## Dependencies
The project requires the following libraries to build and run it:

-  [OpenCV](https://opencv.org/get-started/)
- [xtensor](https://github.com/xtensor-stack/xtensor)
- [CapnProto](https://capnproto.org/install.html)
- [cppzmq](https://github.com/zeromq/cppzmq)
- [Boost](https://www.boost.org/users/download/)

## License
The project is licensed under the MIT License

Copyright © 2024 MRX