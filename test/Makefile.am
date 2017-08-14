## Makefile.am for Oniguruma
lib_onig = ../src/libonig.la

AM_LDFLAGS  = -L$(prefix)/lib
AM_CFLAGS =
AM_CPPFLAGS = -I../src -I$(includedir)

TESTS = test_utf8 testc testp testcu

check_PROGRAMS = test_utf8 testc testp testcu

test: test_utf8 testc testp testcu
	@echo "[Oniguruma API, UTF-8 check]"
	@./test_utf8  | grep RESULT
	@echo "[Oniguruma API, ASCII/EUC-JP check]"
	@./testc  | grep RESULT
	@echo "[POSIX API, ASCII/EUC-JP check]"
	@./testp  | grep RESULT
	@echo "[Oniguruma API, UTF-16 check]"
	@./testcu | grep RESULT

test_utf8_SOURCES = test_utf8.c
test_utf8_LDADD = $(lib_onig)

testc_SOURCES = testc.c
testc_LDADD = $(lib_onig)

testp_SOURCES = testc.c
testp_LDADD = $(lib_onig)
testp_CFLAGS = -DPOSIX_TEST

testcu_SOURCES = testu.c
testcu_LDADD = $(lib_onig)
