#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([nuwriter], [1.0], [www.nuvoton.com.tw])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])


svn_enable_static=no

dnl check for --enable-static option
AC_ARG_ENABLE(static,
  AS_HELP_STRING([--enable-static],
                 [Build static libraries]),
   [svn_enable_static="yes"])
if test "$svn_enable_static" = "yes"; then
	CFLAGS="-static $CFLAGS"
	LDFLAGS="-static $LDFLAGS"
fi

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(LIBUSB, libusb-1.0)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strcasecmp strchr strtoul])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
