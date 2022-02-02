cmake_minimum_required(VERSION 3.5)
project(cw39 VERSION 0.2.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
#set(CMAKE_C_EXTENSIONS OFF)
#set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)
add_compile_options("-Wno-unused-parameter") # Because of warnings in LLVM headers

set(PROJ_SRCS
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/print_tree.cpp
        ${PROJECT_SOURCE_DIR}/src/cli_args.cpp

        ${PROJECT_SOURCE_DIR}/src/parser/ast.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/y.tab.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/lex.yy.c

        ${PROJECT_SOURCE_DIR}/src/ir/types.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/value.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/nodes.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/cfg.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/generator.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/types_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/expr_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/constants_folder.cpp

        ${PROJECT_SOURCE_DIR}/src/transformations/vars_virtualizer.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/dominators.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/ssa_generator.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/cfg_cleaner.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/copy_propagator.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/algebraic_transformer.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/arcs_classifier.cpp

        ${PROJECT_SOURCE_DIR}/src/ir_2_llvm.cpp
        )

set(PROJ_INCLUDES
        ${PROJECT_SOURCE_DIR}/src
        )


add_executable(cw39 ${PROJ_SRCS})
target_include_directories(cw39 PRIVATE ${PROJ_INCLUDES})

# Fmt lib
find_package(fmt REQUIRED)
target_include_directories(cw39 PRIVATE ${FMT_INCLUDE_DIRS})
target_link_libraries(cw39 fmt)

# LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

target_include_directories(cw39 PRIVATE ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(cw39 ${llvm_libs})
