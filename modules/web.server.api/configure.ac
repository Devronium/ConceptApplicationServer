#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(web.server.api, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(web.server.api, 1.0)
AC_CONFIG_SRCDIR([src/AnsiString.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/file.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday select socket strdup strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
