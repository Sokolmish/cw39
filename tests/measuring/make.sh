#!/bin/bash
set -x

CC_ref=clang-10
CC_tst=../cw39

$CC_ref -Wall -O2 -c -o obj_o_meter.o obj_o_meter.c

$CC_ref -O3 -c -o s1_ref.o s1.c

export CW39_LLC=llc-13
$CC_tst --asm s1.c | $CC_ref -O0 -c -x assembler -o s1_tst.o -

$CC_tst --llvm s1.c | $CC_ref -O3 -c -x ir -o s1_max.o -

$CC_ref -O0 -o s1_ref obj_o_meter.o s1_ref.o
$CC_ref -O0 -o s1_tst obj_o_meter.o s1_tst.o
$CC_ref -O0 -o s1_max obj_o_meter.o s1_max.o

rm obj_o_meter.o s1_ref.o s1_tst.o s1_max.o
