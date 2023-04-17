#!/bin/bash

# msbuild options:
#   target: -t:dumpArgs
#   config -p:Configuration=Release
#   parallel build: -m
MSBUILD=$(<build/msbuild)
"$MSBUILD" build/PatienceDummy.sln -m -p:Configuration=Release