#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(mod_concepthttpbridge, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(mod_concepthttpbridge, 1.0)
AC_CONFIG_SRCDIR([src/AnsiString.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB(dl, dlopen)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([strstr])

AC_DEFUN([APACHE_DIR],[

  AC_ARG_WITH(
    apache,
    [  --with-apache[=DIR]     Apache server directory],
    ,
    [with_apache="no"]
  )

  AC_MSG_CHECKING(for Apache directory)

  if test "$with_apache" = "no"; then
    AC_MSG_ERROR( Specify the apache using --with-apache)
  else
    # make sure that a well known include file exists
    if test -e $with_apache/include/httpd.h; then
      apache_dir=$with_apache
      AC_MSG_RESULT(APACHE found!)
    else
      AC_MSG_ERROR( $with_apache not found. )
    fi
  fi

])

APACHE_DIR

AC_SUBST(apache_dir) 

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
