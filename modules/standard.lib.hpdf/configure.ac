#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.hpdf, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.hpdf, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_HPDF=false

AC_ARG_WITH(hpdf,
[  --with-hpdf=/path/to/libharu build hpdf library against libharu])
if test "$with_hpdf"; then
	HAVE_HPDF=true
	HPDF_CFLAGS="-I$with_hpdf/include"
	HPDF_LIBS="-L$with_hpdf/lib -lz -lm -lpng"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $HPDF_LIBS"

	AC_CHECK_LIB([hpdf], [HPDF_New],
	 [HPDF_LIBS="$HPDF_LIBS -lhpdf"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the HPDF_New function.])], HAVE_HPDF=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_HPDF" = "xfalse"; then
	HAVE_HPDF=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	HPDF_CFLAGS="-I$my_prefix/include"
	HPDF_LIBS="-L$my_prefix/lib -lz -lm -lpng"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $HPDF_LIBS"

	AC_CHECK_LIB([hpdf], [HPDF_New],
	 [HPDF_LIBS="$HPDF_LIBS -lhpdf"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the HPDF_New function.])], HAVE_HPDF=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_HPDF" = "xfalse"; then
	echo
	echo "libharu (hpdf) was not found on your system. Please use the --with-hpdf=/path/to/libharu."
	echo "libharu library can be downloaded from http://libharu.sourceforge.net/."
	exit 1
fi

AC_SUBST(HPDF_CFLAGS)
AC_SUBST(HPDF_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
