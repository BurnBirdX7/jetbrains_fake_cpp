# Fake

Fake build system

Brother project of [fake-kotlin](https://github.com/BurnBirdX7/jetbrains_fake_kotlin).

## Navigate
 * [Use](#Use)
 * [Build](#Build)
   * [Standard](#Build)
   * [CMake Toolchain](#CMake-Toolchain)
 * [Test](#Test)
 * [All of this but with **fake**](#All-with-fake)
   * Build, test, install, etc.

## Use

Execute `fake <task>` in directory with `fake.yaml`.

`fake.yaml` contains description of tasks in following format:
 * task-name
   * "`run`": command that is executed when target is out of date
   * "`dependencies`" (optional): contains **one** dependency or **list** of dependencies\
     If **dependencies** not specified, task will be executed if `target` file doesn't exist
   * "`target`" (optional): contains name of target file that will be produced
    after successful execution of this task\
    If **target** isn't specified, task is executed unconditionally

Some details:
 * *fake* fails if *malformed* task is met while processing dependencies of the `<task>`
   * A task considered *malformed* when `run` property is undefined or any property is defined incorrectly.
   * **fake** fails immediately if the `<task>` is *malformed*.
   * If one of the dependencies is *malformed*, **fake** continues processing of other dependencies, but fails at the end.
   * *(Note: **fake-kotlin** behaves differently)*
 * If dependency is not defined as a task in `fake.yaml`, **fake** will try to find a file with the same name
     * If such file can't be found, **fake** remembers this as an error and continues processing.
       **fake** will fail in the end.
 * If there's a cyclic dependency (`task1 -> task2 -> task3 -> task1`), **fake** immediately fails.
 * When **fake** fails, list of occurred errors is printed
   * all detected *malformed* tasks and non-existent dependencies are listed.

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
~/my_project$ fake exec   # Executes all tasks
 > [compile]: g++ -c main.cpp -o main.o
 > [build]: g++ main.o -o main
 > [exec]: ./main
Finished!

~/my_project$ fake exec   # Executes only 'exec'
 > [exec]: ./main
Finished!

~/my_project$ rm ./main   # Delete target of 'build'
~/my_project$ fake exec   # Executes 'compile' and 'exec'
 > [build]: g++ main.o -o main
 > [exec]: ./main
Finished!

~/my_project$ rm ./main.o # Delete target of 'compile'
~/my_project$ fake exec   # Executes all tasks
 > [compile]: g++ -c main.cpp -o main.o
 > [build]: g++ main.o -o main
 > [exec]: ./main
Finished!
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

If `yaml-cpp` or `googletest` can't be found, they are fetched from GitHub.

Executable binary will be placed in `build` directory on Linux and in `build/Release` directory on Windows

### CMake Toolchain
If you have vcpkg (or maybe other package manager that can be integrated with CMake this way)
you can specify location of *cmake toolchain file* with `toolchain` option for the script file:
* Linux
  ```shell
  ./build.sh toolchain /path/to/toolchain/file
  ```
* Windows
  ```powershell
  .\build.bat toolchain "C:\\path\\to\\toolchain\\file"`
  ```

For vcpkg path is `"[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake"`

### Manual build
If build scripts are not working for you, I recommend using **CLion** / **Visual Studio** IDEs to build the project.\
You can try building the project with CMake and parameters to your taste:
```shell
cmake -S . -B [Build dir] -G [Generator] -DCMAKE_BUILD_TYPE=[Build type] [Your other options]
cmake --build [Build dir] --target fake --config [Build type]
```

## Test

Unit tests are located in `gtest` directory.

If you used IDE to build the project it's likely that you have tests available there.\
If you built tests with `build.sh`/`build.bat` you can build tests from terminal.
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
cd [Build dir]/gtest
ctest
```

### basic-test.sh (Linux only)

These aren't intended for Unit-testing, just to test if CI works properly...\
Executes only 2 tests:
 * Basic: checks if program fails when there's no `fake.yaml`, there's no `<task>` argument,
and not fails when executed with correct `fake.yaml`.
 * Cyclic dependency: checks if **fake** fails when called for different tasks.

## All with fake

If you already have **fake** installed on your system you can do all of this with help of **fake**.\
This repository has `fake.yaml` in its root and can be built with **fake**.

You still need all the other required software,
it's just funny replacement for build and test scripts :( 

You also can use [fake-kotlin](https://github.com/BurnBirdX7/jetbrains_fake_kotlin) to build this project

Main tasks:
 * `configure` - configures CMake
 * `build` - builds fake from sources
 * `build-tests` - builds test executable from sources
 * `run-tests` - runs tests
 * `clean` (*Linux only*) - cleans build directory

Bonus:
 * `install` (*Linux only*) - installs **fake** in `/usr/local/bin` (requires permissions to access the folder)
 * `remove` (*Linux only*) - removes **fake** from said directory (also requires permissions)
 * `update` - pulls the latest version of this repository from git

and other...

With **fake** you can shorten building and testing **fake** to:
```shell
fake run-tests
```
