#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.commercial.pdf, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.commercial.pdf, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_PDFLIB=false

AC_ARG_WITH(pdflib,
[  --with-pdflib=/path/to/pdflib build pdflib library against pdflib])
if test "$with_pdflib"; then
	HAVE_PDFLIB=true
	PDFLIB_CFLAGS="-I$with_pdflib/include"
	PDFLIB_LIBS="-L$with_pdflib/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $PDFLIB_LIBS"

	AC_CHECK_LIB([pdf], [PDF_load_font],
	 [PDFLIB_LIBS="$PDFLIB_LIBS -lpdf"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the PDF_load_font function.])], HAVE_PDFLIB=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_PDFLIB" = "xfalse"; then
	HAVE_PDFLIB=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	PDFLIB_CFLAGS="-I$my_prefix/include"
	PDFLIB_LIBS="-L$my_prefix/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $PDFLIB_LIBS"

	AC_CHECK_LIB([pdf], [PDF_load_font],
	 [PDFLIB_LIBS="$PDFLIB_LIBS -lpdf"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the PDF_load_font function.])], HAVE_PDFLIB=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_PDFLIB" = "xfalse"; then
	echo
	echo "PDFLib was not found on your system. Please use the --with-pdflib=/path/to/pdflib."
	echo "PDFLib library can be downloaded from http://www.pdflib.org/."
	exit 1
fi

AC_SUBST(PDFLIB_CFLAGS)
AC_SUBST(PDFLIB_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
