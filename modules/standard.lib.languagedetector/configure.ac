#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.languagedetector, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.languagedetector, 1.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
pkg_modules="cld >= 0.1"
PKG_CHECK_MODULES(CLD, [$pkg_modules])

AC_SUBST(CLD_CFLAGS)
AC_SUBST(CLD_LIBS)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
