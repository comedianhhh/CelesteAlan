#!/bin/bash

timestamp=$(date +%s)

libs="-luser32 -lopengl32 -lgdi32" #if adding a space before the value, making it invalid when passed to the compiler.
warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Ithird_party -Ithird_party/Include"
outputFile="AlanAdventure.exe"


rm -f game_* # Remove old game_* files
clang++ -g "src/game.cpp" -shared -o game_$timestamp.dll $warnings
mv game_$timestamp.dll game.dll

clang++ $includes -g src/main.cpp -o$outputFile $libs $warnings