# makefile
SRC = ../src
CFLAGS   = -I$(SRC) -Wall -g -fsanitize=fuzzer,address -fno-omit-frame-pointer
CFLAGS_M = -I$(SRC) -Wall -g -fsanitize=fuzzer-no-link,address -fno-omit-frame-pointer -DWITH_READ_MAIN
ONIG_LIB = $(SRC)/.libs/libonig.a
LIBS     = $(ONIG_LIB)

TARGETS = encode-libfuzzer syntax-libfuzzer deluxe-encode-libfuzzer \
          utf16-be-libfuzzer utf16-le-libfuzzer main-encode main-syntax \
          main-deluxe-encode main-utf16-be main-utf16-le main-regset \
          regset-libfuzzer \
          libfuzzer-onig libfuzzer-onig-full

default: $(TARGETS)

encode-libfuzzer: encode-harness.c $(ONIG_LIB)
	clang $(CFLAGS) $< $(LIBS) -o $@

syntax-libfuzzer: syntax-harness.c $(ONIG_LIB)
	clang $(CFLAGS) $< $(LIBS) -o $@

deluxe-encode-libfuzzer: deluxe-encode-harness.c $(ONIG_LIB)
	clang $(CFLAGS) $< $(LIBS) -o $@

utf16-be-libfuzzer: encode-harness.c $(ONIG_LIB)
	clang -DUTF16_BE $(CFLAGS) $< $(LIBS) -o $@

utf16-le-libfuzzer: encode-harness.c $(ONIG_LIB)
	clang -DUTF16_LE $(CFLAGS) $< $(LIBS) -o $@

regset-libfuzzer: regset-harness.c $(ONIG_LIB)
	clang $(CFLAGS) $< $(LIBS) -o $@

main-encode: encode-harness.c $(ONIG_LIB)
	clang $(CFLAGS_M) $< $(LIBS) -o $@

main-syntax: syntax-harness.c $(ONIG_LIB)
	clang $(CFLAGS_M) $< $(LIBS) -o $@

main-deluxe-encode: deluxe-encode-harness.c $(ONIG_LIB)
	clang $(CFLAGS_M) $< $(LIBS) -o $@

main-utf16-be: encode-harness.c $(ONIG_LIB)
	clang -DUTF16_BE $(CFLAGS_M) $< $(LIBS) -o $@

main-utf16-le: encode-harness.c $(ONIG_LIB)
	clang -DUTF16_LE $(CFLAGS_M) $< $(LIBS) -o $@

main-regset: regset-harness.c $(ONIG_LIB)
	clang $(CFLAGS_M) $< $(LIBS) -o $@

libfuzzer-onig: libfuzzer-onig.cpp $(ONIG_LIB)
	clang++ $(CFLAGS) $< $(LIBS) -o $@

libfuzzer-onig-full: libfuzzer-onig.cpp $(ONIG_LIB)
	clang++ -DFULL_TEST $(CFLAGS) $< $(LIBS) -o $@


$(ONIG_LIB):
	cd ..; make clean
	#cd ..; autoreconf -vfi
	cd ..; ./configure CC=clang LD=clang CFLAGS="-g -fsanitize=address -fno-omit-frame-pointer" LDFLAGS="-g -fsanitize=address -fno-omit-frame-pointer"
	cd ..; make -j4


clean:
	rm -f $(TARGETS)
