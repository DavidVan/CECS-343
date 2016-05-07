CECS 343, Section 05/06

Project: Version Control System - Phase Three

Project By: Team DRM

Members:

Ryan Ea: helloxhi1@gmail.com

Mark Spencer Tan: msgtan@gmail.com

David Van: David@DavidVan.net

--------------------------------------------------------------------------------

Introduction:

This is the first part of our VCS (Version Control System) project. It only
implements an initial use case: Create Repository. It also makes a number of
simplifying assumptions in order to get to working software quickly.

--------------------------------------------------------------------------------

Usage:

Usage:

Initialize repository: drm init

Update repository: drm update

Check in: drm checkin <updated tree> <repo - where to store changes>
OR
drm checkin <repo - where to store changes>

Check out: drm checkout <project tree> <where to store the copy of the tree> <version>
OR
drm checkout <where to store the copy of the tree> <version>

Merge: drm merge <updated tree> <target tree - where it should merge to> <version>
OR
drm merge <updated tree> <version>

Example:
Check in: drm checkin "C:\\Original Tree" "C:\\Changed Tree\\repo343"

Check out: drm checkout "C:\\Original Tree\\repo343" "C:\\New Tree" "20160504201929"

Merge: drm merge "C:\\Changed Tree\\repo343" "C:\\New Tree" "20160504201950"

--------------------------------------------------------------------------------

Building Instructions and External Dependencies:

Have CMake 3.5.0 or higher installed.

--------------------------------------------------------------------------------

To build on Windows:

Have Visual Studio 2015 Installed.

Enter these commands:

```
cmake -G "Visual Studio 14 2015"
cmake --build . --target ALL_BUILD --config Release
<--OR-->
Open "drm.sln" and build inside Visual Studio.
```

--------------------------------------------------------------------------------

To build on Linux (This isn't working at the moment, I think.):

Have GCC 5.3 or above.

Enter these commands:

```
cmake -G "Unix Makefiles"
sudo make
```

--------------------------------------------------------------------------------

To run the program, go inside the "bin" folder, and move the executable named
"drm" to the root folder of where you want to create a repository. Then in a
terminal/cmd, type "drm init".

For other commands, please read above.

--------------------------------------------------------------------------------

BUGS:

No error handling.
Linux version does not work and was not tested; also we hardcoded Windows path
endings... (/ vs \\) Whoops.

Remaining bugs:

There is also something that looks like a bug, but isn't. When creating Artifact
IDs (using the CheckSum method), it seems like the Artifact ID is always zero.
This is just a coincidence. We take the file size and use the modulus operator
on it. It's filesize % 256.

Another bug involves compiling on Linux. If you get past the CMake instructions,
and actually compile the program, when you run it, the program doesn't work
properly. This is due to the experimental nature of the filesystem library.
Each compiler has to implement its own version of the filesystem library, and
then have it work across different operating systems. It's hard. Our code should
theoretically work across all major operating systems.

--------------------------------------------------------------------------------
