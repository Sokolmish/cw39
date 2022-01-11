# Diploma

C99 compiler with optimizations.

[Test program](https://github.com/Sokolmish/coursework_3/blob/master/tests/tst_prog1.c)

## Requirements

- **gcc11** and above (because not all features of C++20 are suppoted by early versions)
- **LLVM-13** and above (because early versions cannot be compiled with C++20)
- **[fmtlib](https://fmt.dev/latest/index.html)**
- \[optionally\] **Flex** and **Bison** to generate lexer and parser (invoking manually with [Makefile](https://github.com/Sokolmish/coursework_3/blob/master/src/parser/Makefile))

## Building with docker

Docker container can be built using the [`setup.sh`](https://github.com/Sokolmish/coursework_3/blob/master/setup.sh) script. After the first usage, this command should be executed if [Dockerfile](https://github.com/Sokolmish/coursework_3/blob/master/Dockerfile) was changed.

Project can be built using the [`build.sh`](https://github.com/Sokolmish/coursework_3/blob/master/build.sh) script. This script will store final binary and building files in the `docker_build` directory created by previous script.

Both scripts don't expect arguments and should be executed from project root (where Dockerfile is located). Optionally, one can specify number of threads used by make in build script (default is 3).

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
