
cmake_minimum_required(VERSION 2.8)
# required for exports? cmake_minimum_required (VERSION 2.8.6)
project(oniguruma C)

set(PACKAGE onig)
set(PACKAGE_VERSION "5.9.6")

set(USE_COMBINATION_EXPLOSION_CHECK 0)
set(USE_CRNL_AS_LINE_TERMINATOR 0)
set(VERSION ${PACKAGE_VERSION})

include(cmake/dist.cmake)

include(CheckCSourceCompiles)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(TestBigEndian)

check_function_exists(alloca HAVE_ALLOCA)
check_include_files(alloca.h HAVE_ALLOCA_H)
set(HAVE_PROTOTYPES 1)
check_include_files(stdarg.h    HAVE_STDARG_PROTOTYPES)
check_include_files(stdint.h    HAVE_STDINT_H)
check_include_files(stdlib.h    HAVE_STDLIB_H)
check_include_files(strings.h   HAVE_STRINGS_H)
check_include_files(string.h    HAVE_STRING_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/time.h  HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h    HAVE_UNISTD_H)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size(short SIZEOF_SHORT)
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/oniguruma.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc @ONLY)


include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(_SRCS regint.h regparse.h regenc.h st.h
 regerror.c regparse.c regext.c regcomp.c regexec.c reggnu.c
 regenc.c regsyntax.c regtrav.c regversion.c st.c
 regposix.c regposerr.c
 enc/unicode.c enc/ascii.c enc/utf8.c enc/utf16_be.c enc/utf16_le.c
 enc/utf32_be.c enc/utf32_le.c enc/euc_jp.c enc/sjis.c enc/iso8859_1.c
 enc/iso8859_2.c enc/iso8859_3.c enc/iso8859_4.c enc/iso8859_5.c
 enc/iso8859_6.c enc/iso8859_7.c enc/iso8859_8.c enc/iso8859_9.c
 enc/iso8859_10.c enc/iso8859_11.c enc/iso8859_13.c enc/iso8859_14.c
 enc/iso8859_15.c enc/iso8859_16.c enc/euc_tw.c enc/euc_kr.c enc/big5.c
 enc/gb18030.c enc/koi8_r.c enc/cp1251.c)


add_library(onig ${_SRCS})

install_library(onig)

install_header(oniguruma.h onigposix.h oniggnu.h)

install_doc(doc)
install_data(AUTHORS COPYING HISTORY README)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc
  DESTINATION lib/pkgconfig)
