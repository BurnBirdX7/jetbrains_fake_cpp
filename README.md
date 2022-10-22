# Fake

Fake build system

## Usage

Execute `fake` in directory with `fake.yaml` and target task.

`fake.yaml` contains description of tasks in following format:
 * task-name
   * "`run`": command that is executed when target is out of date
   * "`dependencies`" (optional): contains **one** dependency or **list** of dependencies\
     If **dependencies** not specified, task will be executed if `target` file doesn't exist
   * "`target`" (optional): contains name of target file that will be produced
    after successful execution of this task\
    If **target** isn't specified, task is executed unconditionally

Example:
```yaml
# fake.yaml
compile:
  dependencies: main.cpp
  target: main.o
  run: g++ -c main.cpp -o main.o

build:
  dependencies:
    - compile
  target: main
  run: g++ main.o -o main

exec:
  dependencies:
    - build
  run: ./main
```

```shell
# Shell
~/my_project$ fake exec  # Will execute all tasks
~/my_project$ fake exec  # Will execute only `exec`

~/my_project$ rm ./main
~/my_project$ fake exec  # Will execute `compile` and `exec`

~/my_project$ rm ./main.o
~/my_project$ fake exec  # Will execute all tasks
```


## Build
**Requirements**:
* CMake 3.16+
    * Build tool with [CMake generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) (Make, Ninja, etc.)
* C/C++ Compiler
    * gcc 5.0+ / clang 5.0+ on Linux
    * MSVC 2015+ on Windows


**Also:**
 * [`jbeder/yaml-cpp`](https://github.com/jbeder/yaml-cpp) - library for work with YAML
 * [`google/googletest`](https://github.com/google/googletest) - unit-test library for C++

If you have C++ package manager that has these packages **and** cmake toolchain, you can use it (see **CMake Toolchain**) \
If `jbeder/yaml-cpp` and `google/googletest` cannot be found on your system, they are downloaded from GitHub

### Default Way

**Sub-option 1 (Linux only)**
```shell
./build.sh
```

**Sub-option 2**
```shell
mkdir build
cd build
cmake ..
cmake --build .
```

### CMake Toolchain
[ Tested only with vcpkg ]\
Install `yaml-cpp` with your package manager. For example `vcpkg install yaml-cpp:x64-linux`.\
install `googletest`. For example `vcpkg install gtest:x64-linux`.

Build `fake`:

**Sub-option 1 (Linux only)**
```shell
./build.sh toolchain /path/to/toolchain/file
```
For vcpkg it's `"[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake"`

**Sub-option 2**
```shell
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="/path/to/toolchain/file" ..
cmake --build .
```

