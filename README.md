# About Game Sorting

Game Sorting is a little C++ application I've wrote to help classify my list of wished games. It's main purpose is for games, but it can be used for other uses. It's just a simple app that display a Table with three columns : **Game name**, **Game type** and the **Game rate**. It also have multiple tabs, you can close them (except the last one) and open new one.

# Installation

The app work on both Windows and Linux. You can download the build of Game Sorting from the [release page](https://github.com/Erwan28250/GameSorting/releases/). 
You can also compile GameSorting from the sources, I tried to explain the compilation process step by step below, the only dependency you need is Qt 6. The app have been tested with Qt 6.0.1.

# Building from sources

### Windows

To Compile on Windows, you need a C++11 compiler like [Visual Studio](https://visualstudio.microsoft.com/fr/), the framework [Qt 6](https://www.qt.io/) and [CMake](https://cmake.org/) to configure the project.

To install Visual Studio, go to the [Visual Studio](https://visualstudio.microsoft.com/fr/) website and download Visual Studio Community. On the installation, when the install tools ask you what modules you want to install, tick the Desktop C++ module.

To install Qt, go to the [Qt](https://www.qt.io/) website. Then go to the **Download. Try.**, **Download for Open Source** and then **Online installer** at bottom of the page. On the installation, when the installer ask you what module you want to download, choose **MSVC 2019 64-bit** (2019 or newer) under `Qt/Qt 6.0.1/` (6.0.1 or newer).

You can download the ZIP of the project under the **Code** button on top right, or you can clone it with git, but this step won't be explain here.

To configure CMake, you need to set the `CMAKE_INSTALL_DIR` cmake variable to the directory where the **Qt6** is stored, something like this `Qt6Dir\compiler\`, in my case, it's : `C:\Qt\6.0.0\msvc2019_64\`.

Go to the project folder, create a new directory (you can name it like you want :slightly_smiling_face:), open CMake, choose the **Where is the source code** to the folder of the project and the **Where to build the binaries** to your newly created folder. Add an **Entry** of the name CMAKE_INSTALL_PREFIX of type **PATH** and choose the path to `Qt6Dir\compiler\` has we saw above, then click on **configure**, if everythings is okay, click on generate.

On your folder, you should have a Visual Studio project name `GameSorting.vcxproj`, open it with visual studio. On the toolbar change **Debug** to **Release** and then right click on **ALL_BUILD** on right and click on build.

If everything is Okay, the executable should be in `YourFolder\bin\Release`.

Before executing the program, you need to copy some Qt DLLs, go to `Qt6Dir\compiler\bin` and copy and paste where the program executable the following DLLs :
- **Qt6Core.dll**
- **Qt6Gui.dll**
- **Qt6Svg.dll**
- **Qt6Widgets.dll**

Then create a folder **plugins**, inside it create three folders **imageformats**, **platforms** and **styles**.
Now, from the folder `Qt6Dir\compiler\plugins` copy the following DLLs to the specified folder :
- From the folder **imageformats**, copy the DLLs **qico.dll** and **qsvg.dll** to the same named folder in plugins of the ouput directory of the compiled program.
- From the folder **platforms**, copy the DLL **qwindows.dll** to the same named folder.
- From the folder **styles**, copy the DLL **qwindowsvistastyle.dll** to the same named folder.

Now you can enjoy :slightly_smiling_face:.

### Linux

On Linux, you need to install a compiler like `gcc`, the framework `Qt 6`, `CMake` to configure the project for compilation and `git` to download the project.

#### Dependencies

##### Ubuntu

On Ubuntu, install the packages `build-essential`, `libopengl-dev` `cmake` and `git`
```
sudo apt install build-essential cmake git libopengl-dev
```

Then, you need to install Qt6 manually. Go to the [Qt Website](https://www.qt.io/). Then go to the **Download. Try.**, **Download for Open Source** and then **Online installer** at bottom of the page.
To run the installer, you first need to make the downloaded file executable. Open a terminal in where the file is downloaded and enter the command ``chmod +x *filename*` where *filename* is the name of the downloaded file.
Execute the installer and follow it. When you are at the **components selection** section tick the `Qt/Qt 6.0.1/Desktop gcc 64-bit` (6.0.1 or higher) and the click next to install.

The next steps are below, there is only one more variable to set to cmake. Set the variable **CMAKE_INSTALL_PREFIX** to `Qt6Dir/QtVersion/gcc_64` where **Qt6Dir** is where Qt6 is installed and QtVersion the version of Qt (ex: 6.0.1).
In my case, I got something like this `-DCMAKE_INSTALL_PREFIX=/opt/Qt/6.0.1/gcc_64/`.

##### Arch Linux

On Arch Linux, install the packages `base-devel`, `glibc`, `qt6-base`, `qt6-svg`, `cmake` and `git`.

```
sudo pacman -S base-devel glibc qt6-base qt6-svg cmake git
```

#### Build process

Open a terminal, and use the git command to download the project :

```
git clone https://github.com/Erwan28250/GameSorting.git
```

Now, you can build the project :
```
cd gamesorting
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make -j $(nproc)
make install
```

# Licence
Please see the LICENSE file.

