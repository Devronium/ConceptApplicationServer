#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.net.tls, 1.0, eduard@devronium.com)
AM_INIT_AUTOMAKE(standard.net.tls, 1.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
pkg_modules="libssl >= 0.9.8"
PKG_CHECK_MODULES(SSL, [$pkg_modules])

AC_SUBST(SSL_CFLAGS)
AC_SUBST(SSL_LIBS)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
