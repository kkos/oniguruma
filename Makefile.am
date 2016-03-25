## Makefile.am for Oniguruma

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS =
AM_CPPFLAGS = -I$(top_srcdir) -I$(includedir)

SUBDIRS = src test sample

EXTRA_DIST = oniguruma.pc.in HISTORY README.ja README.md \
	index.html index_ja.html \
	CMakeLists.txt dist.info oniguruma.pc.cmake.in cmake/dist.cmake \
	doc/API doc/API.ja doc/RE doc/RE.ja doc/FAQ doc/FAQ.ja \
	win32/Makefile win32/config.h win32/testc.c

bin_SCRIPTS = onig-config

onig-config: onig-config.in

do_subst = sed                                          \
      -e 's,[@]datadir[@],$(datadir),g'                 \
      -e 's,[@]datarootdir[@],$(datarootdir),g'         \
      -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
      -e 's,[@]prefix[@],$(prefix),g'                   \
      -e 's,[@]exec_prefix[@],$(exec_prefix),g'         \
      -e 's,[@]libdir[@],$(libdir),g'                   \
      -e 's,[@]includedir[@],$(includedir),g'

oniguruma.pc: $(srcdir)/oniguruma.pc.in Makefile
	$(do_subst) < $(<) > $(@)

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = oniguruma.pc
