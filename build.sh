#!/bin/bash
libs="-luser32" #if adding a space before the value, making it invalid when passed to the compiler.
warnings="-Wno-writable-strings -Wno-format-security"

clang++ -g src/CPPs/main.cpp -oAlanAdventure.exe $libs $warnings