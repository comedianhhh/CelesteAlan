#!/bin/bash
libs="-luser32" #if adding a space before the value, making it invalid when passed to the compiler.
warnings=-Wno-writable-strings

clang src/main.cpp -oAlanAdventure.exe $libs $warnings