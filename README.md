# Fake

Fake build system

## Usage

Execute `fake` in directory with `fake.yaml` and target task.

`fake.yaml` contains description of tasks in following format:
 * task-name
   * "`run`": command that is executed when target is out of date
   * "`dependencies`" (optional): contains **one** dependency or **list** of dependencies\
     If dependencies not specified, task is executed unconditionally
   * "`target`" (optional): contains name of target file that will be produced
    after successful execution of this task\
    If target isn't specified, task is executed unconditionally

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

~/my_project$ frm ./main.o
~/my_project$ fake exec  # Will execute all tasks
```


## Build

To build this project we need [`jbeder/yaml-cpp`](https://github.com/jbeder/yaml-cpp) - library for work with YAML

**Requirements**:
 * CMake >= 3.16
 * git
 * `g++`/`clang` on Linux, MSVC on Windows

There are 3 options:
1. You already have `yaml-cpp` in your system, findable with CMake's `find_package`
2. You have C++ package manager that has cmake toolchain
3. You have none of that

### Option #1 


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

### Option #2
[ Tested only with vcpkg ]\
Install `yaml-cpp` with your package manager. For example `vcpkg install yaml-cpp:x64-linux`.

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

### Option #3

In this case `yaml-cpp` is build as subdir project

**Sub-option 1 (Linux only)**
```shell
./build.sh build-yaml
```
For vcpkg it's `[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake"`

**Sub-option 2**\
I'm sorry
```shell
git clone "https://github.com/jbeder/yaml-cpp"
mkdir build
cd build
cmake -DYAML_IS_SUBDIR ..
cmake --build .
```



