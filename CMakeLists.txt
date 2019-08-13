cmake_minimum_required(VERSION 3.1)
project(oniguruma
  VERSION 6.9.3
  LANGUAGES C)

set(PACKAGE onig)
set(PACKAGE_VERSION ${PROJECT_VERSION})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_POSIX_API  "Include POSIX API" ON)
if(MSVC)
  option(MSVC_STATIC_RUNTIME "Build with static runtime" OFF)
endif()

set(USE_CRNL_AS_LINE_TERMINATOR 0)
set(VERSION ${PACKAGE_VERSION})

include(CheckCSourceCompiles)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(TestBigEndian)

check_function_exists(alloca HAVE_ALLOCA)
check_include_files(alloca.h HAVE_ALLOCA_H)
check_include_files(stdint.h    HAVE_STDINT_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/time.h  HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h    HAVE_UNISTD_H)
check_include_files(inttypes.h  HAVE_INTTYPES_H)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/oniguruma.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc @ONLY)

set(_SRCS src/regint.h src/regparse.h src/regenc.h src/st.h
 src/regerror.c src/regparse.c src/regext.c src/regcomp.c src/regexec.c
 src/reggnu.c src/regenc.c src/regsyntax.c src/regtrav.c src/regversion.c
 src/st.c src/onig_init.c
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

set(_INST_HEADERS src/oniguruma.h src/oniggnu.h)

if(ENABLE_POSIX_API)
  set(_SRCS ${_SRCS} src/regposix.c src/regposerr.c)
  set(_INST_HEADERS ${_INST_HEADERS} src/onigposix.h)
endif()

add_library(onig ${_SRCS})
target_include_directories(onig PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_compile_definitions(onig PUBLIC
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:ONIG_STATIC>)

if(MSVC)
  target_compile_options(onig PRIVATE
	#/W4
	)
  if(MSVC_STATIC_RUNTIME)
	target_compile_options(onig PRIVATE
	  $<$<CONFIG:Release>:/MT>
	  $<$<CONFIG:Debug>:/MTd>
	  $<$<CONFIG:MinSizeRel>:/MT>
	  $<$<CONFIG:RelWithDebgInfo>:/MTd>
	  )
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC)
  target_compile_options(onig PRIVATE
	-Wall
	)
endif()


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
    FILES ${_INST_HEADERS}
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
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PACKAGE}")

# Other files (uses onig not oniguruma for directory)
install(FILES AUTHORS COPYING HISTORY README.md
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PACKAGE}")

# pkg-config
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/oniguruma.pc
        DESTINATION lib/pkgconfig)
