cmake_minimum_required(VERSION 3.1)
project(oniguruma VERSION 6.8.2)

set(PACKAGE onig)
set(PACKAGE_VERSION ${PROJECT_VERSION})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(USE_CRNL_AS_LINE_TERMINATOR 0)
set(VERSION ${PACKAGE_VERSION})

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()


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
check_include_files(limits.h    HAVE_LIMITS_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/time.h  HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h    HAVE_UNISTD_H)
check_include_files(inttypes.h  HAVE_INTTYPES_H)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size(short SIZEOF_SHORT)
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/oniguruma.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc @ONLY)

set(_SRCS src/regint.h src/regparse.h src/regenc.h src/st.h
 src/regerror.c src/regparse.c src/regext.c src/regcomp.c src/regexec.c
 src/reggnu.c src/regenc.c src/regsyntax.c src/regtrav.c src/regversion.c
 src/st.c src/regposix.c src/regposerr.c src/onig_init.c
 src/unicode.c src/ascii.c src/utf8.c src/utf16_be.c src/utf16_le.c
 src/utf32_be.c src/utf32_le.c src/euc_jp.c src/sjis.c src/iso8859_1.c
 src/iso8859_2.c src/iso8859_3.c src/iso8859_4.c src/iso8859_5.c
 src/iso8859_6.c src/iso8859_7.c src/iso8859_8.c src/iso8859_9.c
 src/iso8859_10.c src/iso8859_11.c src/iso8859_13.c src/iso8859_14.c
 src/iso8859_15.c src/iso8859_16.c src/euc_tw.c src/euc_kr.c src/big5.c
 src/gb18030.c src/koi8_r.c src/cp1251.c
 src/euc_jp_prop.c src/sjis_prop.c
 src/unicode_unfold_key.c
 src/unicode_fold1_key.c src/unicode_fold2_key.c src/unicode_fold3_key.c)

add_library(onig ${_SRCS})
target_include_directories(onig PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

# Installation (https://github.com/forexample/package-example)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib/libonig.a
#   * header location after install: <prefix>/include/
#   * headers can be included by C code `#include <oniguruma.h>`
install(
    TARGETS onig
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Headers:
#   * src/oniguruma.h -> <prefix>/include/oniguruma
install(
    FILES src/oniguruma.h src/onigposix.h src/oniggnu.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Config
#   * <prefix>/lib/cmake/oniguruma/onigurumaConfig.cmake
#   * <prefix>/lib/cmake/oniguruma/onigurumaConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/oniguruma/onigurumaTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

# Documentation (uses onig not oniguruma for directory)
install(FILES doc/API doc/API.ja doc/RE doc/RE.ja doc/FAQ doc/FAQ.ja
              doc/CALLOUTS.BUILTIN doc/CALLOUTS.BUILTIN.ja
              doc/CALLOUTS.API doc/CALLOUTS.API.ja
              doc/UNICODE_PROPERTIES
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PACKAGE_NAME}")

# Other files (uses onig not oniguruma for directory)
install(FILES AUTHORS COPYING HISTORY README.md
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PACKAGE_NAME}")

# pkg-config
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc
        DESTINATION lib/pkgconfig)
