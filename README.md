AIS Radar style View plug-in for OpenCPN
==================================

Source Repository : https://github.com/Verezano/AISradar_pi

Maintance Repository: https://github.com/rgleason/AISradar_pi/

Please submit PR's to both repositories. Thank you.

History
-------
This plugin was created in 2011 to show AIS targets in a radar style view. While there were no radar plugins at the time it was inadvertently named radar_pi. Over time this has been corrected to aisradar_pi and so is the repostory name. Since the plugin is packaged in some distributions its name aisradar_pi cannot be changed, because that would break things.
All references to radar in the code have been changed to ais or aisview to reflect the real purpose of this plugin.
As suggested by r.gleason the repository is renamed to AISradar_pi.git

Plugin Manager
--------------
This plugin is easily available from within OpenCPN using Options > Plugin Manager for many supported OS.

Compiling
---------
```
git clone --recurse-submodules https://github.com/rgleason/AISradar_pi.git
```

###Build:
```
mkdir AISradar_pi/build
cd AISradar_pi/build
cmake ..
cmake --build .
```
Windows note: You must place opencpn.lib into your build directory to be able to link the plugin DLL. You can get this file from your local OpenCPN build, or alternatively download from http://sourceforge.net/projects/opencpnplugins/files/opencpn_lib/

Debugging:
If you check out the plugin source into the plugins subdirectory of your OpenCPN source tree, you can build it as part of it.

Windows Specific Libraries
--------------------------

Under windows, you must find the file "opencpn.lib" (Visual Studio) or "libopencpn.dll.a" (mingw) which is built in the build directory after compiling opencp

###Creating a package
Linux
```
make package
```

Windows
```
cmake --build . --config release --target package
```

###Build on Mac OS X:
Tools: Can be installed either manually or from Homebrew (http://brew.sh)
```
#brew install git #If I remember well, it is already available on the system
brew install cmake
brew install gettext
ln -s /usr/local/Cellar/gettext/0.19.2/bin/msgmerge /usr/local/bin/msgmerge
ln -s /usr/local/Cellar/gettext/0.19.2/bin/msgfmt /usr/local/bin/msgfmt
```

To target older OS X versions than the one you are running, you need the respective SDKs installed. The easiest way to achieve that is using https://github.co

####Building wxWidgets
(do not use wxmac from Homebrew, it is not compatible with OpenCPN)
Get wxWidgets 3.0.x source from http://wxwidgets.org
Configure, build and install
```
cd wxWidgets-3.0.2
./configure --enable-unicode --with-osx-cocoa --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7
make
sudo make install
```

####Building the plugin
Before running cmake, you must set the deployment target to OS X 10.7 to be compatible with the libraries used by core OpenCPN
```
export MACOSX_DEPLOYMENT_TARGET=10.7
```

####Packaging on OS X
Get and install the Packages application from http://s.sudre.free.fr/Software/Packages/about.html
```
make create-pkg
```

License
-------
The plugin code is licensed under the terms of the GPL v2 or, at your convenience, later version.
