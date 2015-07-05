[![Build Status](https://secure.travis-ci.org/janbar/pvr.mythtv.png?branch=doityourself)](http://travis-ci.org/janbar/pvr.mythtv) [![Coverity Scan Build Status](https://scan.coverity.com/projects/3115/badge.svg)](https://scan.coverity.com/projects/3115)

## Building

### Linux, BSD

Start by creating a build folder
```
mkdir -p build
rm -rf build/*
cd build/
```

To build PVR addon execute the following:
```
cmake -DCMAKE_BUILD_TYPE=Release ../
make
```

To package it into individual ZIP archive (which can then be installed manually): 
```
cp -r ../pvr.mythtv ./
cp pvr.mythtv.so pvr.mythtv/
zip -r pvr.mythtv-linux.zip ./pvr.mythtv
```

### OSX

Start by creating a build folder
```
mkdir -p build
rm -rf build/*
cd build/
```

To build PVR addon execute the following:
```
cmake -DCORE_SYSTEM_NAME=darwin -DCMAKE_BUILD_TYPE=Release ../
make
```

To package it into individual ZIP archive (which can then be installed manually):
```
cp -r ../pvr.mythtv ./
cp pvr.mythtv.dylib pvr.mythtv/
zip -r pvr.mythtv-osx.zip ./pvr.mythtv
```

### Windows

Run cmake GUI and create the VC project in a dedicated folder.
Open the generated solution ```pvr.mythtv.sln``` in Visual C++, then Build it.
To package it into individual ZIP archive, copy the generated DLL ```pvr.mythtv.dll``` into sub folder pvr.mythtv from the sources. ZIP the sub folder with its content as pvr.mythtv-win32.zip.

