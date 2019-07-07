#!/bin/bash

make distclean
autoreconf -vfi
# build the library with ASAN
./configure CC=clang LD=clang CFLAGS="-g -fsanitize=address -fno-omit-frame-pointer" LDFLAGS="-g -fsanitize=address -fno-omit-frame-pointer"
make -j4

OUT=`pwd`/fuzzers
mkdir -p $OUT
LIBFUZZER_FLAGS="-fsanitize=fuzzer,address -fno-omit-frame-pointer"
LIBS="src/.libs/libonig.a"
#OPTLIBS="/usr/local/lib/libLLVMFuzzerMain.a"
#LIBS="src/.libs/libonig.a ${OPTLIBS}"

# Libfuzzer builds
clang++ contributed/libfuzzer-onig.cpp $LIBS -Isrc/ -g $LIBFUZZER_FLAGS -o $OUT/libfuzzer-onig
clang harnesses/syntax-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/syntax-libfuzzer
clang harnesses/encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/encode-libfuzzer
clang harnesses/deluxe-encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/deluxe-encode-libfuzzer
