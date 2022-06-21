#!/bin/bash
#set -x

CC_ref=clang-10
CC_tst=../cw39

$CC_ref -Wall -O2 -c -o obj_o_meter.o obj_o_meter.c

create_test() {
    local tname="$1"

    echo "[*] Making ${tname}"

    $CC_ref -O3 -c -o ${tname}_ref.o ${tname}.c

    export CW39_LLC=llc-13
    $CC_tst --asm ${tname}.c | $CC_ref -O0 -c -x assembler -o ${tname}_tst.o -

    $CC_tst --llvm ${tname}.c | $CC_ref -O3 -c -x ir -o ${tname}_max.o -

    $CC_ref -O0 -lm -o ${tname}_ref obj_o_meter.o ${tname}_ref.o
    $CC_ref -O0 -lm -o ${tname}_tst obj_o_meter.o ${tname}_tst.o
    $CC_ref -O0 -lm -o ${tname}_max obj_o_meter.o ${tname}_max.o

    rm ${tname}_ref.o ${tname}_tst.o ${tname}_max.o

    mkdir -p ${tname}/
    mv ${tname}_ref ${tname}_tst ${tname}_max ${tname}/
}

create_test s1
create_test d1
create_test d2
create_test d3
create_test d4
create_test d5

rm obj_o_meter.o
