#noinst_PROGRAMS = encode listcap names posix simple sql syntax crnl

lib_onig = ../src/libonig.la
LDADD  = $(lib_onig)

AM_LDFLAGS  = -L$(prefix)/lib
AM_CPPFLAGS = -I$(top_srcdir)/src

TESTS = encode listcap names posix simple sql syntax user_property callout echo count bug_fix regset

check_PROGRAMS = $(TESTS)

encode_SOURCES  = encode.c
listcap_SOURCES = listcap.c
names_SOURCES   = names.c
posix_SOURCES   = posix.c
simple_SOURCES  = simple.c
sql_SOURCES     = sql.c
syntax_SOURCES  = syntax.c
user_property_SOURCES = user_property.c
callout_SOURCES = callout.c
echo_SOURCES    = echo.c
count_SOURCES   = count.c
bug_fix         = bug_fix.c
regset_SOURCES  = regset.c

sampledir = .

test: $(TESTS)
	$(sampledir)/encode
	$(sampledir)/listcap
	$(sampledir)/names
	$(sampledir)/posix
	$(sampledir)/simple
	$(sampledir)/sql
	$(sampledir)/syntax
	$(sampledir)/user_property
	$(sampledir)/callout
	$(sampledir)/echo
	$(sampledir)/count
	$(sampledir)/bug_fix
	$(sampledir)/regset
