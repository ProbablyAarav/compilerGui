# compilerGui
Made a GUI to show how a compiler works, along with a very basic compiler of my own design. The compiler was written in C++

## Build instructions

#### Requirements (these can all be installed from most package managers, or their respective websites linked below): 
C++ >= 17 (most computers come with this, for Windows, follow [this](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170))
Cmake >= 3.16 (Get it [here](https://cmake.org/download/))
qt6 (Get it [here](https://doc.qt.io/qt-6/get-and-install-qt.html))
git (Get it [here](https://git-scm.com/downloads))

#### Building
In terminal 
``git clone https://github.com/ProbablyAarav/compilerGUI.git``
``mkdir build``
``cd build``
``cmake ..``
``make``
``./npavc_gui``

## Known bugs & issues
When typing in the console emulator, text doesn't show up until you click enter.
Windows support may be choppy
