#!/bin/bash

make clean
autoreconf -vfi
# build the library with ASAN
./configure CC=clang LD=clang CFLAGS="-g -fsanitize=address -fno-omit-frame-pointer -fsanitize=fuzzer-no-link" LDFLAGS="-g -fsanitize=address -fno-omit-frame-pointer -fsanitize=fuzzer-no-link"
make -j4

OUT=`pwd`/fuzzers
mkdir -p $OUT
LIBFUZZER_FLAGS="-fsanitize=fuzzer,address -fno-omit-frame-pointer"
LIBS="src/.libs/libonig.a"
#LIBS="src/.libs/libonig.a /usr/local/lib/libLLVMFuzzerMain.a"

# Libfuzzer builds
clang++ contributed/libfuzzer-onig.cpp $LIBS -Isrc/ -g $LIBFUZZER_FLAGS -o $OUT/libfuzzer-onig
clang harnesses/syntax-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/syntax-libfuzzer
clang harnesses/encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/encode-libfuzzer
clang harnesses/deluxe-encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/deluxe-encode-libfuzzer

clang -DUTF16_BE harnesses/encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/utf16-be-libfuzzer
clang -DUTF16_LE harnesses/encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/utf16-le-libfuzzer
clang -DWITH_READ_MAIN harnesses/encode-harness.c src/.libs/libonig.a -Isrc -g $LIBFUZZER_FLAGS -o $OUT/main-encode
clang -DWITH_READ_MAIN -DUTF16_LE harnesses/encode-harness.c src/.libs/libonig.a -Isrc -g $LIBFUZZER_FLAGS -o $OUT/main-utf16-le
clang -DWITH_READ_MAIN -DUTF16_BE harnesses/encode-harness.c src/.libs/libonig.a -Isrc -g $LIBFUZZER_FLAGS -o $OUT/main-utf16-be
clang -DWITH_READ_MAIN harnesses/deluxe-encode-harness.c $LIBS -Isrc -g $LIBFUZZER_FLAGS -o $OUT/main-deluxe-encode
