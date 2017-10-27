#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.json, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.json, 1.0)
AC_CONFIG_SRCDIR([src/AnsiString.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lfann':
# AC_CHECK_LIB([json], [json_object_new_object])

# pkg_modules="json >= 0.8"
# PKG_CHECK_MODULES(JSON, [$pkg_modules])

# AC_SUBST(JSON_CFLAGS)
# AC_SUBST(JSON_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
