
if [ "$1" = "toolchain" ] && [ ! -z "$2" ]; then
  echo "Toolchain is set to '$2'"
  OPTIONS="-DCMAKE_TOOLCHAIN_FILE=$2"
fi

[ -d "build" ] && [ ! -f "build/CMakeCache.txt" ] && rm -rf build
cmake -DCMAKE_BUILD_TYPE=Release "$OPTIONS" -S . -B build &&
  cmake --build build --target fake --config Release &&
  echo "Done!" || echo "Cannot build fake :("
