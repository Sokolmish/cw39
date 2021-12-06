cmake_minimum_required(VERSION 3.5)
project(cw39)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -g)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(
        ${PROJECT_SOURCE_DIR}/src
#        ${PROJECT_SOURCE_DIR}/libs
)

set(PROJ_SRCS
        ${PROJECT_SOURCE_DIR}/src/parser/ast.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/y.tab.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/lex.yy.c

        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/print_tree.cpp
        ${PROJECT_SOURCE_DIR}/src/ir_nodes.cpp
        ${PROJECT_SOURCE_DIR}/src/ir_generator.cpp
        ${PROJECT_SOURCE_DIR}/src/ir_types_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/ir_expr_gen.cpp)

add_executable(cw39 ${PROJ_SRCS})

#target_link_libraries(cw39 ${PROJECT_SOURCE_DIR}/libs/libfmt.a)
find_package(fmt REQUIRED)
include_directories(${FMT_INCLUDE_DIRS})
target_link_libraries(cw39 fmt)

llvm_map_components_to_libnames(llvm_libs core native engine interpreter)
target_link_libraries(cw39 LLVM-10)
