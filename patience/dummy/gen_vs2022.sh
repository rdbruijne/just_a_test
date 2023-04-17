#!/bin/bash

# clean old
[ -d "build" ] && rm -rf build
[ -d "bin" ] && rm -rf bin

# generate project
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ../

# msbuild options:
#   target: -t:dumpArgs
#   config -p:Configuration=Release
#   parallel build: -m
#cd ../
#MSBUILD=$(<build/msbuild)
#"$MSBUILD" build/PatienceDummy.sln -m -p:Configuration=Release
