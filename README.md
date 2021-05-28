# About Game Sorting
 
**IT IS NOT READY TO BE USED, ONLY FOR TESTING PURPOSE.**

Game Sorting is a little application to help classify wished games. It is in development and there is a lot of lacking features need to be added.

Some data added to the list needs to be inserted into a separated list called utility (the name isn't good, I didn't find anything better) and then added to the main list. The purpose is to help searching and filtering the list.

# Command Line Arguments

*Usage: ./GameSorting [options] itemList*

- *--reset-settings* &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Reset to default settings
- *--do-not-save-settings* &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Do not save the settings
- *-v, --version* &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Displays version information.
- *-h, --help* &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Displays help on commandline options.

The *itemList* is a positionnal argument, it's a path to a list file to open.

# Installation

The app is working on both Windows and Linux. For Windows you can find precompiled binaries in the [releases page](https://github.com/Erwan28250/GameSorting/releases/). You will need [Microsoft C++ 2019 Redistributable](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0).  
You can also compile it from the source code.

# Building from sources

## Windows

To compile on Windows you need a C++17 compatible compiler like [Visual Studio](https://visualstudio.microsoft.com/fr/) or [MinGW](http://mingw-w64.org/doku.php). You also need [Qt 6](https://www.qt.io/) and [CMake](https://cmake.org/).

To let cmake find Qt 6, use the **CMAKE_PREFIX_PATH** variable. The path should be something like this `C:\Qt\6.0.0\msvc2019_64\`. To build the program in release mode, set the CMake variable **CMAKE_BUILD_TYPE** to **Release**.

## Linux

To compile on Linux, you need to install a compiler like **gcc**, the framework **Qt 6**, **CMake** and **git**.

### Ubuntu

Unfortunaly, Qt 6 is not on the main repos of Ubuntu. You need to install Qt from the [official installer](https://www.qt.io/). To compile Game Sorting you need the package `build-essential`, `libopengl-dev`, `cmake` and `git`.

```
sudo apt install build-essential cmake git libopengl-dev
```

For CMake to detect Qt 6, you need to add the Qt 6 path to the **CMAKE_PREFIX_PATH** CMake variables. The path should be something like this `/opt/Qt/6.0.1/gcc_64/`.

```
cmake -DCMAKE_PREFIX_PATH=/opt/Qt/6.0.1/gcc_64/
```

### Arch Linux

On Arch Linux, install the packages `base-devel`, `glibc`, `qt6-base`, `qt6-svg`, `cmake` and `git`.

```
sudo pacman -S base-devel glibc qt6-base qt6-svg cmake git
```

### Build process

Clone the GameSorting repository with git:

```
git clone https://github.com/Erwan28250/GameSorting.git
```

Then, compile the program:

```
cd GameSorting
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make -j $(nproc)
sudo make install # If you want to install the program into /usr/local.
```

# Licence

Please see the [LICENCE](https://github.com/Erwan28250/GameSorting/blob/development/LICENCE) file.
