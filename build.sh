#!/bin/bash
libs="-luser32 -lopengl32" #if adding a space before the value, making it invalid when passed to the compiler.
warnings="-Wno-writable-strings -Wno-format-security"
includes="-Ithird_party -Ithird_party/Include"
clang++ $includes -g src/CPPs/main.cpp -oAlanAdventure.exe $libs $warnings