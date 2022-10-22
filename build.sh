
if [ "$1" = "toolchain" ] && [ ! -z ${2+x} ]; then
  OPTIONS="-DCMAKE_TOOLCHAIN_FILE=$2"

elif [ "$1" = "external-yaml" ]; then
  OPTIONS="-DYAML_IS_EXTERNAL=1"

elif [ "$1" = "build-yaml" ]; then
  YAML_CPP_SRC_DIR="$(pwd)/yaml-cpp"

  if [ ! -d "$YAML_CPP_SRC_DIR" ]
  then
    echo "yaml-cpp directory does not exist... cloning it..."
    git clone "https://github.com/jbeder/yaml-cpp.git"
  fi

  OPTIONS="-DYAML_IS_SUBDIR=1"
fi

[ -d "build" ] && rm -rf build
mkdir build && cd build || echo "Cannot create build directory"
cmake "$OPTIONS" .. && cmake --build . --target fake && echo "Done!" || echo "Cannot build fake :(" # and already trashed old build
