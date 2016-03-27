CECS 343 Project - Version Control System - Phase One
Project By: Team DRM

Members:
Ryan Ea, Mark Spencer Tan, David Van

Usage:
Initialize repository: drm init

--------------------------------------------------------------------------------

Building Instructions:

Have CMake 3.5.0 or higher installed.

--------------------------------------------------------------------------------

To build on Windows:

Have Visual Studio 2015 Installed.

Enter these commands:

cmake -G "Visual Studio 14 2015"
cmake --build . --target ALL_BUILD --config Release
<--OR-->
Open "drm.sln" and build inside Visual Studio.

--------------------------------------------------------------------------------

To build on Linux (This isn't working at the moment, I think.):

Have GCC 5.3 or above.

Enter these commands:

cmake -G "Unix Makefiles"
sudo make

--------------------------------------------------------------------------------

To run the program, go inside the "bin" folder, and move the executable named
"drm" to the root folder of where you want to create a repository. Then in a
terminal/cmd, type "drm init".
