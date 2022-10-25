@echo off

IF NOT exist "build" (echo "You need to build with ./build.bat first..." && exit 1)

cmake --build build --config Release --target test && cd build\gtest && ctest
