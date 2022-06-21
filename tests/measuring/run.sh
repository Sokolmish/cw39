#!/bin/bash

run_test() {
    name="$1"

    cd $name

    echo REFERENCE
    ./${name}_ref >ref_out
    echo

    echo TESTED
    ./${name}_tst >tst_out
    echo

    echo MAXIMAl
    ./${name}_max >max_out
    echo

    md5sum ref_out tst_out max_out
    rm ref_out tst_out max_out
}

if [ -z "$1" ]; then
    echo 'Specify test name'
    exit 1
fi

run_test $1
