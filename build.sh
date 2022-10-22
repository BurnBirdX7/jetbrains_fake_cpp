
if [ "$1" = "toolchain" ] && [ ! -z ${2+x} ]; then
  OPTIONS="-DCMAKE_TOOLCHAIN_FILE=$2"
fi

[ -d "build" ] && rm -rf build
cmake "$OPTIONS" -S . -B build &&
  cmake --build build --target fake &&
  echo "Done!" || echo "Cannot build fake :("
