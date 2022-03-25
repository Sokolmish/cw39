# Diploma

C99 optimizing compiler producing LLVM assembler.

[One of test programs](https://github.com/Sokolmish/coursework_3/blob/master/tests/tst_prog1.c)

## Requirements

- **CMake**
- **gcc-10** and above
- **LLVM-13** and above (because early versions cannot be compiled with C++20)
- **[fmtlib](https://fmt.dev/latest/index.html)** (tested with versions 6 and 8)
- \[*optionally*\] [**Flex**](https://github.com/westes/flex) and [**Bison**](https://www.gnu.org/software/bison) to generate lexer and parser

## Building

To build this compiler one should execute following commands, which are common for most CMake projects. Executable binary (`cw39`) will be placed in the `build` directory.

```
mkdir build
cd build
cmake ..
make
```

By default, parsers code generation is enabled.
But if you don't have Flex and Bison installed, the project can be built using already generated source files.
To do this, one should call CMake with  option `-DCW39_NO_GENERATE_PARSERS=TRUE`, as shown below.
Other commands are identical to previous ones.

```
cmake -DCW39_NO_GENERATE_PARSERS=TRUE ..
```

Also, if you have troubles with CMake and Flex/Bison communication, you can generate source codes manually using this [Makefile](/src/parser/Makefile).

### Building using Docker

Docker container can be built using the [`setup.sh`](/setup.sh) script. After the first usage, this command should be executed if [Dockerfile](/Dockerfile) was changed.

Project can be built using the [`build.sh`](/build.sh) script. This script will store final binary and building files in the `docker_build` directory created by previous script.

Both scripts don't expect arguments and should be executed from project root (where Dockerfile is located). Optionally, one can specify number of threads used by make in the build script (default is 3).

## Usage

```
cw39 [options] <input_file>
```

### Options:

Each of following options can accept optional argument with path (e.g. `--llvm=./out.ll`). In this case output will be written into specified file. Otherwise, it will be written into stdout. Without any of these options, compiler will print nothing but errors.

- `--ast` - print abstract syntax tree with pseudo graphics
- `--ir` - print final IR
- `--cfg` - print final CFG representation in the [dot](https://graphviz.org/) language
- `--ir-raw` - print IR before optimizations (i.e. right after generation)
- `--cfg-raw` - print CFG representation before optimizations in the [dot](https://graphviz.org/) language
- `--llvm` - print final LLVM assembler code

## Examples

Print LLVM code into terminal:
```
cw39 --llvm ./test.c
```

Print LLVM code into the `out.ll` file:
```
cw39 --llvm=out.ll ./test.c
```

Execute generated code immediately (also `lli-13` and others can be used):
```
cw39 --llvm ./test.c | lli
```

Draw CFG into the `graph.svg` file:
```
cw39 --cfg ./test.c | dot -Tsvg -o graph.svg
```
