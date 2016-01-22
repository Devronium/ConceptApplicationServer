#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.net.opal, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.net.opal, 1.0)
AC_CONFIG_SRCDIR([src/AnsiString.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lxml2':
# AC_CHECK_LIB([xml2], [xmlReaderForFile])

pkg_modules="opal >= 3.12.0"
PKG_CHECK_MODULES(OPAL, [$pkg_modules])

AC_SUBST(OPAL_CFLAGS)
AC_SUBST(OPAL_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h memory.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
