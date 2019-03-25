## Makefile.am for Oniguruma
libname = libonig.la

AM_CFLAGS = -Wall
AM_CPPFLAGS = -I$(top_srcdir)

include_HEADERS = oniguruma.h oniggnu.h

posix_headers = onigposix.h

if ENABLE_POSIX_API
posix_sources = regposix.c regposerr.c
include_HEADERS += $(posix_headers)
else
posix_sources =
endif


lib_LTLIBRARIES = $(libname)

libonig_la_SOURCES = regint.h regparse.h regenc.h st.h \
	regparse.c regcomp.c regexec.c \
	regenc.c regerror.c regext.c regsyntax.c regtrav.c regversion.c st.c \
	reggnu.c \
  $(posix_sources) \
	unicode.c \
	unicode_unfold_key.c \
	unicode_fold1_key.c \
	unicode_fold2_key.c \
	unicode_fold3_key.c \
	ascii.c utf8.c \
	utf16_be.c utf16_le.c \
	utf32_be.c utf32_le.c \
	euc_jp.c euc_jp_prop.c \
	sjis.c sjis_prop.c \
	iso8859_1.c \
	iso8859_2.c  iso8859_3.c \
	iso8859_4.c  iso8859_5.c \
	iso8859_6.c  iso8859_7.c \
	iso8859_8.c  iso8859_9.c \
	iso8859_10.c iso8859_11.c \
	iso8859_13.c iso8859_14.c \
	iso8859_15.c iso8859_16.c \
	euc_tw.c euc_kr.c big5.c \
	gb18030.c koi8_r.c cp1251.c \
	onig_init.c

libonig_la_LDFLAGS = -version-info $(LTVERSION)

EXTRA_DIST = koi8.c mktable.c \
	unicode_fold_data.c unicode_property_data.c \
	unicode_property_data_posix.c \
	unicode_egcb_data.c unicode_wb_data.c \
	make_unicode_egcb.sh make_unicode_egcb_data.py \
	make_unicode_wb.sh make_unicode_wb_data.py \
	make_unicode_fold.sh make_unicode_fold_data.py \
	make_unicode_property.sh make_unicode_property_data.py \
	make_property.sh \
	gperf_fold_key_conv.py gperf_unfold_key_conv.py

dll:
	$(CXX) -shared -Wl,--output-def,libonig.def -o libonig.dll *.o \
	$(LIBS)
	strip libonig.dll


# character-types-table source generator
mktable: mktable.c regenc.h
	$(CC) -I$(top_srcdir) -o mktable mktable.c
