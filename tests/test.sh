#!/bin/bash

../cmake-build-debug/cw39 ./tst_prog1.c > ./tst1.ll &&
  llvm-as-13 ./tst1.ll && lli-13 ./tst1.bc
