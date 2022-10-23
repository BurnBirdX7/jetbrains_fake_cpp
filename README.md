# Fake

Fake build system

## Navigate
 * [Usage](#Usage)
 * [Build](#Build)
 * [Unit-testing](#Unit-testing)

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


**Also uses these:**
 * [`jbeder/yaml-cpp`](https://github.com/jbeder/yaml-cpp) - library for work with YAML
 * [`google/googletest`](https://github.com/google/googletest) - unit-test library for C++

You can build this project with your IDE, or from terminal:
* Linux
  ```shell
  ./build.sh
  ```
* Windows
  ```powershell
  .\build.bat
  ```

### CMake Toolchain
If you have vcpkg (or maybe other package manager that can be integrated with CMake this way)
you can specify location of cmake toolchain file with `toolchain` option for the script file:
* Linux
  ```shell
  ./build.sh toolchain /path/to/toolchain/file
  ```
* Windows
  ```powershell
  .\build.bat toolchain "C:\\path\\to\\toolchain\\file"`
  ```

For vcpkg it's `"[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake"`

### Manual build
If build scripts are not working for you, I recommend using **CLion** / **Visual Studio** IDEs to build the project.\
You can try building the project with CMake and parameters to your taste:
```shell
cmake -S . -B [Build dir] -G [Generator] -DCMAKE_BUILD_TYPE=[Build type] [Your other options]
cmake --build [Build dir] --target fake --config [Build type]
```

## Unit-testing

Unit tests are located in `gtest` directory.

If you used IDE to build the project it's likely that you can run tests there too.\
You can also run unit-tests from terminal:
 * Linux
   ```shell
   ./gtest.sh
   ```
 * Windows
   ```powershell
   .\gtest.bat
   ```

### With manual build
If you built it manually you can build and run these tests manually too:
```shell
cmake --build [Build dir] --target test
```
There's a problem: location of compiled binaries depends on Generator you used...\
If it was **ninja** or some **Makefiles**, tests most likely located in `[Build dir]/gtest`\
If it was **MSBuild** (default in Windows), they most likely located in `[Build dir]/gtest/[Build type]`
(Build type by default is `DEBUG`)

To launch tests:
```shell
# Ninja / Makefiles
cd [Build dir]/gtest
./test
# MSBuild
cd [Build dir]\gtest
.\[Build type]\test
```

