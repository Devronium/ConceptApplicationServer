#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(win32.base.messages, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(win32.base.messages, 1.0)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([memset select strstr])

# Checks for libraries.
pkg_modules="libssl >= 0.9.8"
PKG_CHECK_MODULES(SSL, [$pkg_modules])

AC_SUBST(SSL_CFLAGS)
AC_SUBST(SSL_LIBS)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
