#!/bin/bash

../cmake-build-debug/cw39 ./tst_prog1.c > ./tst1.ll && lli-13 ./tst1.ll

echo "==================="

../cmake-build-debug/cw39 ./tst_prog2.c > ./tst2.ll && lli-13 ./tst2.ll 1 2 3
