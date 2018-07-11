#!/bin/bash

export THREADS=16

do_release() {
    export CPU_ARCH=$1
    export LD_ARCH=$2
    export CC_ARCH=$3
    export LD_PATH=$4
    export CC_FLAGS=-DLSP_NO_EXPERIMENTAL
    
    echo "******************************************************"
    echo "  Releasing arch=$CPU_ARCH ld=$LD_ARCH cc=$CC_ARCH";
    echo "******************************************************"

    make clean && make -j$THREADS all VERSION=dbg && make VERSION=dbg dbg_release;
    make clean && make -j$THREADS profile VERSION=dbg && make VERSION=dbg release_profile;
}

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

make unrelease
do_release 'i586' '-m elf_i386' '-m32' '/usr/lib:/lib';
do_release 'x86_64' '-m elf_x86_64' '-m64' '/usr/lib64:/lib64';

