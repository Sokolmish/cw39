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

set(BISON_PARSER_RES    yy_parser.cpp)
set(BISON_HEADER_RES    yy_parser.hpp)
set(BISON_LOCATIONS_HEADER  yy_location.hpp)
set(FLEX_LEXER_RES      yy_scanner.cpp)
set(BISON_PARSER_SRC    parser.y)
set(FLEX_LEXER_SRC      lexer.l)

set(GPERF_DIRECTIVES_RES    directives_hashtable.cpp)
set(GPERF_DIRECTIVES_SRC    directives.gperf)

set(PROJ_SRCS
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/print_tree.cpp
        ${PROJECT_SOURCE_DIR}/src/cli_args.cpp
        ${PROJECT_SOURCE_DIR}/src/utils.cpp

        ${PROJECT_SOURCE_DIR}/src/parser/parsing_context.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/preprocessor.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/lexer.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/ast.cpp
        ${PROJECT_SOURCE_DIR}/src/parser/core_driver.cpp

        ${PROJECT_SOURCE_DIR}/src/parser/${BISON_PARSER_RES}
        ${PROJECT_SOURCE_DIR}/src/parser/${FLEX_LEXER_RES}

        ${PROJECT_SOURCE_DIR}/src/ir/types.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/value.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/nodes.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/unit.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/generator.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/types_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/expr_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/ir/constants_folder.cpp

        ${PROJECT_SOURCE_DIR}/src/transformations/graph_info.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/loops_detector.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/vars_virtualizer.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/ssa_generator.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/cfg_cleaner.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/copy_propagator.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/algebraic_transformer.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/tailrec_eliminator.cpp
        ${PROJECT_SOURCE_DIR}/src/transformations/loop_inv_mover.cpp

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

# Flex and Bison

IF(NOT CW39_NO_BISON)
    # There are problems with changing version of flex and bison
    #   so there are no FIND_PACKAGE commands and direct names are used

    add_custom_command(
            OUTPUT  ${PROJECT_SOURCE_DIR}/src/parser/${BISON_PARSER_RES}
                    ${PROJECT_SOURCE_DIR}/src/parser/${BISON_HEADER_RES}
                    ${PROJECT_SOURCE_DIR}/src/parser/${BISON_LOCATIONS_HEADER}
            COMMAND bison -Wall -Wno-yacc
                    --defines=${BISON_HEADER_RES}
                    -Dapi.location.file='"${BISON_LOCATIONS_HEADER}"'
                    -o ${BISON_PARSER_RES}
                    ${BISON_PARSER_SRC}
            DEPENDS ${PROJECT_SOURCE_DIR}/src/parser/${BISON_PARSER_SRC}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/parser
            COMMENT "Generating core parser with bison"
    )

    add_custom_command(
            OUTPUT  ${PROJECT_SOURCE_DIR}/src/parser/${FLEX_LEXER_RES}
            COMMAND flex
                    -o${FLEX_LEXER_RES}
                    ${FLEX_LEXER_SRC}
            DEPENDS ${PROJECT_SOURCE_DIR}/src/parser/${FLEX_LEXER_SRC}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/parser
            COMMENT "Generating core lexer with flex"
    )
ENDIF() # CW39_NO_GENERATE_PARSERS

IF(NOT CW39_NO_GPERF)
    add_custom_command(
            OUTPUT  ${PROJECT_SOURCE_DIR}/src/parser/${GPERF_DIRECTIVES_RES}
            COMMAND gperf -m 20
                    --output-file=${GPERF_DIRECTIVES_RES}
                    ${GPERF_DIRECTIVES_SRC}
            DEPENDS ${PROJECT_SOURCE_DIR}/src/parser/${GPERF_DIRECTIVES_SRC}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/parser
            COMMENT "Generating preprocessor directives hastable"
    )
    add_custom_target(
            pc_directives_hashtable_target
            DEPENDS ${PROJECT_SOURCE_DIR}/src/parser/${GPERF_DIRECTIVES_RES}
    )
    add_dependencies(cw39 pc_directives_hashtable_target)

ENDIF()
