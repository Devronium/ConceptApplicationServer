#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.arch.codec2, 1.0, eduard@devronium...)
AM_INIT_AUTOMAKE(standard.arch.codec2, 1.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
pkg_modules="codec2 >= 0.8"
PKG_CHECK_MODULES(CODEC2, [$pkg_modules])

AC_SUBST(CODEC2_CFLAGS)
AC_SUBST(CODEC2_LIBS)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
