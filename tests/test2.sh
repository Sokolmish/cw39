#!/bin/bash

set -euo pipefail

CC_ref=clang-10
CC_tst=../cmake-build-debug/cw39

ref_exe='ref_exe.out'
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

run_testcase() {
    local file=$1
    shift
    local args="$@"

    $CC_ref -o $ref_exe $file
    local ref_rc="$($ref_exe $args >$ref_out 2>&1; echo $?)"

    local tst_rc="$($CC_tst --llvm $file | lli-13 - $args >$tst_out 2>&1; echo $?)"

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

    rm $ref_exe
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

echo -e "[${green}PASSED${nc}] ${bold}All tests finished successfully${nb}"
