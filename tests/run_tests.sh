#!/bin/bash

set -euo pipefail

CC_ref=clang-10
CC_tst=../cmake-build-debug/cw39

ref_exe='ref_exe.out'
tst_exe='tst_exe.out'
ref_out='ref_out.txt'
tst_out='tst_out.txt'

export CW39_LLC=llc-13

green='\033[0;32m'
red='\033[0;31m'
nc='\033[0m'
bold='\033[1m'
nb='\033[0m'

fail_str="$(echo -en "[${red}FAILED${nc}]")"
ok_str="$(echo -en "[${green}OK${nc}]")"

test_vg=$([[ -n "$1" && "$1" == 'vg' ]] && echo 1 || echo 0)

run_testcase() {
    local file=$1
    shift
    local args="$@"

    $CC_ref -o $ref_exe $file
    local ref_rc="$($ref_exe $args >$ref_out 2>&1; echo $?)"

    $CC_tst --asm $file | $CC_ref -x assembler -o $tst_exe -
    local tst_rc="$($tst_exe $args >$tst_out 2>&1; echo $?)"

    if [[ "$ref_rc" != "$tst_rc" ]]; then
        echo "${fail_str} Test '$file': Exit codes differs ($ref_rc, $tst_rc)"
        return 1
    fi

    local outs_diff="$(diff --color=always $tst_out $ref_out)"

    if [[ -n "$outs_diff" ]]; then
        echo
        echo "$outs_diff"
        echo
        echo "${fail_str} Test '$file': Outputs differs"
        return 1
    fi

    if [[ "$test_vg" == '1' ]]; then
        local vg_rc=$(valgrind --leak-check=full --show-reachable=yes --error-exitcode=255 \
            $tst_exe >/dev/null 2>/dev/null; echo $?)
        if [[ "vg_rc" == '255' ]]; then
            echo "${fail_str} Test '$file': Valgrind shows errors"
            return 1
        fi
    fi

    rm $ref_exe $tst_exe
    rm $ref_out $tst_out

    echo "${ok_str} Test '$file' passed"
    return 0
}

run_testcase ./tst_prog1.c
run_testcase ./tst_prog2.c
run_testcase ./tst_prog2.c 1 2 3
run_testcase ./tst_prog3.c
run_testcase ./tst_prog4.c
run_testcase ./tst_prog5.c
run_testcase ./tst_prog6.c
run_testcase ./tst_prog7.c
run_testcase ./s1_test.c 800

echo -e "[${green}PASSED${nc}] ${bold}All tests finished successfully${nb}"
