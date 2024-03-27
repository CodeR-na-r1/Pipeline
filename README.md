# Description
A project to develop a framework for building a pipeline for processing multidimensional data, such as tensors or digital images.

The framework allows you to build a pipeline directly from code, or using a configuration file. The latter method allows you to rebuild the pipeline without recompiling the framework, “on the ball”

The project is currently under development

## Design goals
The following goals were set before development:

- Maximum performance. The project uses pre-tested libraries, and the project code is optimized to minimize unnecessary copying. Checks for memory leaks and uses smart pointers.
- Reducing the connectivity of the framework modules, so that they can be reused in the future. For this purpose, the technique of dependency inversion and the concept mechanism are used
- Maintaining cross-platform functionality. The project is being built for Windows and Linux

## Examples
🚧 Information will appear here later 🚧

## Dependencies
The project requires the following libraries to build and run it:

-  [OpenCV](https://opencv.org/get-started/)
- [xtensor](https://github.com/xtensor-stack/xtensor)
- [CapnProto](https://capnproto.org/install.html)
- [cppzmq](https://github.com/zeromq/cppzmq)
- [Boost](https://www.boost.org/users/download/)

## Build instructions
The project is build using CMake 🔧

To build this project, you need to install the dependency libraries listed above

After that, use the following commands:

```
# /* clone the repository, cd to directory */

mkdir build
cd build
cmake ..
sudo make -j4 install   # or use another generator
```
*You may need to additionally specify the CMake location of the installed libraries on your computer.

## License
The project is licensed under the MIT License

Copyright © 2024 MRX 🌎