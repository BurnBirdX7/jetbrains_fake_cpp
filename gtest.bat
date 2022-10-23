@echo off

IF NOT exist "build" (echo "You need to build with ./build.sh first..." && exit 1)

cmake --build build --config Release --target test && cd build/gtest/Release && test.exe
