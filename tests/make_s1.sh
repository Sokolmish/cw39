#!/bin/bash

set -euo pipefail
#set -x

export CW39_LLC=llc-13

clang-10 -O3 s1_test.c -o ref
echo "[+] Reference is made"

./cw39 --asm s1_test.c | clang-10 -x assembler -o subj -
echo "[+] Subject is made"

./cw39 --asm --llc-args="-O3 -mcpu=native" s1_test.c | clang-10 -x assembler -o fast -
echo "[+] Fast subject is made"
