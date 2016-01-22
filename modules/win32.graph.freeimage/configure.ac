#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(win32.graph.freeimage, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(win32.graph.freeimage, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_FREEIMAGE=false

AC_ARG_WITH(freeimage,
[  --with-freeimage=/path/to/freeimage build image library against freeimage])
if test "$with_freeimage"; then
	HAVE_FREEIMAGE=true
	FREEIMAGE_CFLAGS="-I$with_freeimage/include"
	FREEIMAGE_LIBS="-L$with_freeimage/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $FREEIMAGE_LIBS"
	AC_LANG_PUSH([C++])
	AC_CHECK_LIB(freeimage, FreeImage_SetPixelColor,
	 [FREEIMAGE_LIBS="$FREEIMAGE_LIBS -lfreeimage"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the FreeImage_SetPixelColor function.])], HAVE_FREEIMAGE=false)
	AC_LANG_POP([C++])
	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_FREEIMAGE" = "xfalse"; then
	HAVE_FREEIMAGE=true
	img_prefix=$prefix

	if test "x$img_prefix" = "xNONE"; then
    		img_prefix="/usr/local"
	fi

	FREEIMAGE_CFLAGS="-I$img_prefix/include"
	FREEIMAGE_LIBS="-L$img_prefix/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $FREEIMAGE_LIBS"
	AC_LANG_PUSH([C++])
	AC_CHECK_LIB(freeimage, FreeImage_SetPixelColor,
	 [FREEIMAGE_LIBS="$FREEIMAGE_LIBS -lfreeimage"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the FreeImage_SetPixelColor function.])], HAVE_FREEIMAGE=false)
	AC_LANG_POP([C++])
	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_FREEIMAGE" = "xfalse"; then
	echo
	echo The FreeImage library was not found on your system. Please use the --with-freeimage=/path/to/freeimage.
	echo Freeimage library can be downloaded from http://freeimage.sourceforge.net/.
	exit 1
fi

AC_SUBST(FREEIMAGE_CFLAGS)
AC_SUBST(FREEIMAGE_LIBS)



# Checks for header files.
AC_CHECK_HEADERS([wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
