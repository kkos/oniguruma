## Makefile.am for Oniguruma
lib_onig = ../src/libonig.la

AM_LDFLAGS  = -L$(prefix)/lib
AM_CFLAGS = -Wall -Wno-invalid-source-encoding
AM_CPPFLAGS = -I$(top_srcdir)/src

if ENABLE_POSIX_API
TESTS = test_utf8 testc testp testcu
else
TESTS = test_utf8 testc testcu
endif

check_PROGRAMS = $(TESTS)

test: test_uchar $(TESTS)
	@echo "[Oniguruma API, UTF-8 check]"
	@./test_utf8  | grep RESULT
	@echo "[Oniguruma API, ASCII/EUC-JP check]"
	@./testc  | grep RESULT
if ENABLE_POSIX_API
	@echo "[POSIX API, ASCII/EUC-JP check]"
	@./testp  | grep RESULT
endif
	@echo "[Oniguruma API, UTF-16 check]"
	@./testcu | grep RESULT

test_uchar:
	@echo "[UChar in oniguruma.h check]"
	@grep "\(^\|[^g]\)UChar" $(top_srcdir)/src/oniguruma.h
	@echo ""

test_utf8_SOURCES = test_utf8.c
test_utf8_LDADD = $(lib_onig)

testc_SOURCES = testc.c
testc_LDADD = $(lib_onig)

testp_SOURCES = testc.c
testp_LDADD = $(lib_onig)
testp_CFLAGS = -DPOSIX_TEST -Wall -Wno-invalid-source-encoding


testcu_SOURCES = testu.c
testcu_LDADD = $(lib_onig)


gcov:
	make CFLAGS="--coverage" test_utf8
	make CFLAGS="--coverage" testc
	make CFLAGS="--coverage" testp
	make CFLAGS="--coverage" testcu
