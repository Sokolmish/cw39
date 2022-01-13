#!/bin/bash

inFile="$1"
ext="svg"

[ -z "$inFile" ] && inFile="tst_prog3.c"
# && echo "No file" >&2 && exit 1

../cmake-build-debug/cw39 --cfg "$inFile" | dot -T${ext} -o "graph.${ext}"
