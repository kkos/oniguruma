
ONIG_LIB=../src/.libs/libonig.a
LIBS=$(ONIG_LIB) /usr/local/lib/libLLVMFuzzerMain.a

TARGETS=libfuzzer-onig libfuzzer-onig-full

default: $(TARGETS)

libfuzzer-onig: libfuzzer-onig.cpp $(ONIG_LIB)
	clang++ $< $(LIBS) -o $@ -fsanitize-coverage=trace-pc-guard -fsanitize=fuzzer,address

libfuzzer-onig-full: libfuzzer-onig.cpp $(ONIG_LIB)
	clang++ -DFULL_TEST $< $(LIBS) -o $@ -fsanitize-coverage=trace-pc-guard -fsanitize=fuzzer,address


$(ONIG_LIB):
	cd ..; ./configure CC=clang LD=clang CFLAGS="-g -fsanitize=fuzzer,address" LDFLAGS="-fsanitize-coverage=trace-pc-guard -fsanitize=fuzzer,address"; make



clean: 
	rm -f $(TARGETS)
