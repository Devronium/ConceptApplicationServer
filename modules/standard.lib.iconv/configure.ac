#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.iconv, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.iconv, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_ICONV=false

AC_ARG_WITH(iconv,
[  --with-iconv=/path/to/iconv build library against iconv])
if test "$with_iconv"; then
	HAVE_ICONV=true
	ICONV_CFLAGS="-I$with_iconv/include"
	ICONV_LIBS="-L$with_iconv/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ICONV_LIBS"

	AC_CHECK_LIB([iconv], [iconv_open],
	 [ICONV_LIBS="$ICONV_LIBS -liconv"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the iconv_open function.])], HAVE_ICONV=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_ICONV" = "xfalse"; then
	HAVE_ICONV=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	ICONV_CFLAGS="-I$my_prefix/include"
	ICONV_LIBS="-L$my_prefix/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ICONV_LIBS"

	AC_CHECK_LIB([iconv], [iconv_open],
	 [ICONV_LIBS="$ICONV_LIBS -liconv"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the iconv_open function.])], HAVE_ICONV=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_ICONV" = "xfalse"; then
	echo
	echo "Iconv was not found on your system. Please use the --with-iconv=/path/to/iconv."
	echo "libiconv can be downloaded from http://www.gnu.org/software/libiconv. Trying to build anyway."
fi

AC_SUBST(ICONV_CFLAGS)
AC_SUBST(ICONV_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
