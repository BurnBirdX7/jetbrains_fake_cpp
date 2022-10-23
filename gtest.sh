
[ ! -d "build" ] && echo "You need to build with ./build.sh first..." && exit 1

cmake --build build --target test &&
  ./build/gtest/test || echo "Failed to build and run tests"
