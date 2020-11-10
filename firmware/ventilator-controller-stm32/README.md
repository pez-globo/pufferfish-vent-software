# ventilator-controller-stm32

This is a STM32CubeIDE project of the embedded software which will run on the
Pufferfish ventilator. We are testing this with the NUCLEO-H743ZI2 microcontroller
board. We are using STM32Cube IDE for all development.


## Reconfiguring Pins and Peripherals with STM32CubeMX

Before you make any configuration changes to the microcontroller's pins or built-in
peripherals using STM32CubeMX, you will need to rename the following files:

- Rename `Core/Src/main.cpp` to `Core/Src/main.c`
- Rename `Core/Src/stm32h7_it.cpp` to `Core/Src/stm32h7_it.c`

Then you can make configuration changes and regenerate code using STM32CubeMX.
Then you'll need to rename those files back in order to recompile the project:

- Rename `Core/Src/main.c` to `Core/Src/main.cpp`
- Rename `Core/Src/stm32h7_it.c` to `Core/Src/stm32h7_it.cpp`


## Auto-Formatting

To automatically format all code using clang-format, first install `clang-format`.

Then, from this directory, run the `clang-format-all.sh` script with the usual
options for clang-format (though note that this script will use the configuration
defined in the `.clang-format` file). For example, to do a dry run identifying all
suggested formatting changes, run:

```
./clang-format-all.sh --dry-run
```

Note that you can pass the same arguments to `clang-format-all.sh` as to
clang-format.  For example, to automatically apply all suggested formatting
changes directly to the files, run:

```
./clang-format-all.sh -i
```

If you want to run a command which sets the shell process return code to be an error
if at least one formatting change was suggested, run:

```
! ./clang-format-all.sh -dry-run 2>&1 | grep ''
```

## Cppcheck

To run static checking with cppcheck, first install `cppcheck`; if you'd like to
use the GUI, also install `cppcheck-gui`.

You can run the cppcheck GUI by starting `cppcheck-gui` and opening the
`application.cppcheck` project file (to check the code to be uploaded to the STM32)
or the `testcatch2.cppcheck` project file (to check the test code). Th GUI also
makes it easy to edit the project files.

You can run cppcheck checks for the main application from the command-line with:

```
./cppcheck.sh application
```

## CMake Cross-Compilation Builds

To use CMake to generate makefiles for building the main firmware application
without running STM32Cube IDE (e.g. to build on a headless server or to use some
Clang-based tools), first install `cmake`.

### Building the Firmware

Then find the path where the GCC arm-none-eabi toolchain is available (if you have
not already installed it into somewhere accessible from the shell), and add it to
your shell's path. For example, you might have this toolchain provided by the
STM32Cube IDE at
`/opt/st/stm32cubeide_1.3.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_1.0.0.201904181610/tools/bin/`
in which case you can save it into the `TOOLCHAIN_PATH` variable:
```
TOOLCHAIN_PATH="/opt/st/stm32cubeide_1.3.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_1.0.0.201904181610/tools"
```

To build the project in debug mode with four build threads (and to generate a
compile commands database for clang-tidy):
```
./cmake.sh Debug $TOOLCHAIN_PATH  # run from the firmware/ventilator-controller-stm32 directory
cd cmake-build-debug
make -j4
```

To build it in release mode with two build threads (and to generate a compile
commands database for clang-tidy):
```
./cmake.sh Release $TOOLCHAIN_PATH  # run from the firmware/ventilator-controller-stm32 directory
cd cmake-build-release
make -j2
```

If you are on a headless server without an STM32Cube IDE installation, you can
simply install this toolchain:
```
sudo apt-get install gcc-arm-none-eabi
```
Note that the installed version here will be different from what is provided by
the STM32Cube IDE. Then you may need to provide a different set of paths to CMake.
On Ubuntu 20.04, in order to make a Clang build (for clang-tidy) you'll need to
run cmake as follows:
```
./cmake.sh Clang /usr/lib /usr/bin
```

### Scan-build

To run scan-build on the project, first install `clang-tools` and use CMake to
generate the makefiles, then run your normal `make` command in a CMake build
directory but first prepend `scan-build`. For example:
```
./cmake.sh Debug $TOOLCHAIN_PATH  # run from the firmware/ventilator-controller-stm32 directory
cd cmake-build-debug
scan-build make -j4
```

Note that on Ubuntu 20.04, you may need to install `clang-tools-10` instead of
`clang-tools`.

### Clang-tidy

To run clang-tidy on the firmware, first install `clang-tidy`, then
use CMake to generate a compile commands database for the Clang build type
(with the appropriate toolchain path, as usual), and then run the
`clang-tidy-all.sh` script:
```
./cmake.sh Clang $TOOLCHAIN_PATH  # run from the firmware/ventilator-controller-stm32 directory
./clang-tidy-all.sh Clang
```
Note that this script will delete and rebuild the `cmake-build-debug` directory
if it already exists, and then it will run clang-tidy to report all warnings.
Note that on Ubuntu 20.04, you may need to install `clang-tidy-10` instead of
`clang-tidy`.

You can also pass the `clang-tidy-all.sh` script the normal arguments for clang-tidy,
though you should not use `--` in the arguments (this causes clang-tidy to ignore
the compile commands database and become unable to find header files from our project).
For example, you can use the following to apply suggested fixes:
```
./cmake.sh Clang $TOOLCHAIN_PATH  # run from the firmware/ventilator-controller-stm32 directory
./clang-tidy-all.sh Clang --fix --fix-errors
```

Every time you create or delete a file, you will need to use CMake to re-generate
the compile commands database.

We have disabled the following checks:

- google-runtime-references: we use non-const references as output parameters and for dependency injection in constructors.
- modernize-use-trailing-return-type: we use the more traditional `int foo()` style of defining functions, rather than the `auto foo() -> int` style recommended by this check.

We have temporarily disabled the following checks until we decide on a better
way to pass around byte buffers (probably using a C++ spans library) than `uint8_t *` pointers:

- cppcoreguidelines-pro-bounds-pointer-arithmetic: this is needed for handling `uint8_t *` buffers
- cppcoreguidelines-pro-bounds-constant-array-index: this is needed for handling `uint8_t *` buffers

We have also disabled the following check aliases to checks which are enabled anyways
(to avoid repeated warnings on the same problems, and avoid double-fixing which makes
code fail compilation):

- bugprone-narrowing-conversions: this is a duplicate of cppcoreguidelines-narrowing-conversions.
- cert-con36-c: this is a duplicate of bugprone-spuriously-wake-up-functions.
- cert-con54-cpp: this is a duplicate of bugprone-spuriously-wake-up-functions.
- cert-dcl03-c: this is a duplicate of misc-static-assert.
- cert-dcl16-c: this is a duplicate of readability-uppercase-literal-suffix.
- cert-dcl37-c: this is a duplicate of bugprone-reserved-identifier.
- cert-dcl51-cpp: this is a duplicate of bugprone-reserved-identifier.
- cert-dcl54-cpp: this is a duplicate of misc-new-delete-overloads.
- cert-dcl59-cpp: this is a duplicate of google-build-namespaces.
- cert-err09-cpp: this is a duplicate of misc-throw-by-value-catch-by-reference.
- cert-err61-cpp: this is a duplicate of misc-throw-by-value-catch-by-reference.
- cert-fio38-c: this is a duplicate of misc-non-copyable-objects.
- cert-msc30-c: this is a duplicate of cert-msc50-cpp.
- cert-msc32-c: this is a duplicate of cert-msc51-cpp.
- cert-oop11-cpp: this is a duplicate of performance-move-constructor-init.
- cert-oop54-cpp: this is a duplicate of bugprone-unhandled-self-assignment.
- cert-pos44-c: this is a duplicate of bugprone-bad-signal-to-kill-thread.
- cert-str34-c: this is a duplicate of bugprone-signed-char-misuse.
- cppcoreguidelines-avoid-c-arrays: this is a duplicate of modernize-avoid-c-arrays.
- cppcoreguidelines-avoid-magic-numbers: this is a duplicate of readability-magic-numbers.
- cppcoreguidelines-c-copy-assignment-signature: this is a duplicate of misc-unconventional-assign-operator.
- cppcoreguidelines-explicit-virtual-functions: this is a duplicate of modernize-use-override.
- cppcoreguidelines-non-private-member-variables-in-classes: this is a duplicate of misc-non-private-member-variables-in-classes.
- google-readability-braces-around-statements: this is a duplicate of readability-braces-around-statements.
- google-readability-function-size: this is a duplicate of readability-function-size.
- google-readability-namespace-comments: this is a duplicate of llvm-namespace-comment.
- llvm-else-after-return: this is a duplicate of readability-else-after-return.
- llvm-qualified-auto: this is a duplicate of readability-qualified-auto.


## CMake Native Builds

To use CMake to generate makefiles for building the Catch2 tests, for execution
on the native computer (rather than an STM32) and without running STM32Cube IDE,
first ensure `cmake` is installed.

### Building the Catch2 Tests

Just run:
```
./cmake.sh TestCatch2  # run from the firmware/ventilator-controller-stm32 directory
cd cmake-build-testcatch2
make -j4
```

Then you can run the tests with `./TestCatch2`.

### Scan-build

To run scan-build on the Catch2 tests, first ensure `clang-tools` is installed and use
CMake to generate the makefiles, then run your normal `make` command in a CMake build
directory but first prepend `scan-build`. For example:
```
./cmake.sh TestCatch2   # run from the firmware/ventilator-controller-stm32 directory
cd cmake-build-testcatch2
scan-build make -j4
```

### Clang-tidy


To run clang-tidy on the Catch2 tests, use CMake to generate a compile commands
database for the TestCatch2 build type, and then run the `clang-tidy-all.sh` script:
```
./cmake.sh TestCatch2  # run from the firmware/ventilator-controller-stm32 directory
./clang-tidy-all.sh TestCatch2
```



## Running

### Embedded Software in Debug Mode

To run the embedded software on the STM32, select the "ventilator-controller-stm32 Debug"
run target in either the run configurations menu or the run configurations manager,
and then run the target.

### Running Automated Tests

STM32Cube IDE interacts in bad ways with our `CMakeLists.txt` file, so it is not
possible to build the Catch2 automated tests from within the IDE. Instead, you
should follow the above instructions in the "Building the Catch2 Tests" section.
