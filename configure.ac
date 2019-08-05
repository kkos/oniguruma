dnl Process this file with autoconf to produce a configure script.
AC_INIT(onig, 6.9.3)

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wno-portability])
AC_CONFIG_HEADERS([src/config.h])


dnl default value for STATISTICS
STATISTICS=""
AC_ARG_WITH([statistics],
  [AS_HELP_STRING([--with-statistics],
    [take matching time statistical data])],
  [STATISTICS=-DONIG_DEBUG_STATISTICS])
AC_SUBST(STATISTICS)


dnl check for POSIX API
AC_ARG_ENABLE([posix-api],
  [AS_HELP_STRING([--enable-posix-api],
    [turn on to include POSIX API [default=yes]])],
[\
case "${enableval}" in
 yes) enable_posix_api=yes ;;
 no)  enable_posix_api=no ;;
 *)   AC_MSG_ERROR(bad value for --enable-posix-api) ;;
esac],
enable_posix_api=yes)
AM_CONDITIONAL(ENABLE_POSIX_API, test x"${enable_posix_api}" = xyes)


dnl check for CRNL_AS_LINE_TERMINATOR
AC_ARG_ENABLE([crnl-as-line-terminator],
  [AS_HELP_STRING([--enable-crnl-as-line-terminator],
    [deprecated])],
  [crnl_as_line_terminator=$enableval])
if test "${crnl_as_line_terminator}" = yes; then
  AC_DEFINE(USE_CRNL_AS_LINE_TERMINATOR,1,[Define if enable CR+NL as line terminator])
fi


dnl Checks for programs.
AC_PROG_CC
LT_INIT
LTVERSION="5:0:0"
AC_SUBST(LTVERSION)

AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Checks for libraries.

dnl Checks for header files.
AC_CHECK_HEADERS(sys/time.h unistd.h sys/times.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

dnl Checks for library functions.
AC_FUNC_ALLOCA

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile sample/Makefile onig-config])
AC_CONFIG_COMMANDS([default],[chmod +x onig-config],[])
AC_OUTPUT
