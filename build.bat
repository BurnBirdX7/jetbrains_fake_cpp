@echo off

IF "%1" == "toolchain" IF NOT [%2] == [] set OPTIONS=--toolchain "%2"

cmake %OPTIONS% -S . -B build && cmake --build build --config Release --target fake
