#!/bin/bash

defines="-DENGINE"
warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Ithird_party -Ithird_party/Include"

timestamp=$(date +%s)


echo "Running on Windows"
libs="-luser32 -lopengl32 -lgdi32 -lole32" #if adding a space before the value, making it invalid when passed to the compiler.
outputFile="AlanAdventure.exe"


rm -f game_* # Remove old game_* files
clang++ -g "src/game.cpp" -shared -o game_$timestamp.dll $warnings $defines
mv game_$timestamp.dll game.dll

clang++ $includes -g src/main.cpp -o$outputFile $libs $warnings $defines