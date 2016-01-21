#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(onceptclient, 4.0, eduard@devronium.com)
AM_INIT_AUTOMAKE(conceptclient, 4.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LIBTOOL


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netinet/in.h stddef.h stdlib.h string.h sys/socket.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

pkg_modules="libssl >= 0.9.8"
PKG_CHECK_MODULES(SSL, [$pkg_modules])

AC_SUBST(SSL_CFLAGS)
AC_SUBST(SSL_LIBS)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_UTIME_NULL
AC_CHECK_FUNCS([memmove memset strstr utime])
AC_OUTPUT(Makefile src/Makefile)
