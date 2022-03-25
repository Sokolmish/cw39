#!/bin/bash

# NOTICE: this script should be executed from project root because of `pwd`
# Compiled program will be placed in `docker_build` directory

BUILD_COMMAND="cd build \
    && cmake ../project \
    && make -j 3"

mkdir -p docker_build

docker run -it --rm \
    -v "$(pwd):/root/project" \
    -v "$(pwd)/docker_build:/root/build" \
    cw39_builder /bin/bash -c "$BUILD_COMMAND"
