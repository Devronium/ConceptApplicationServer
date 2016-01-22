#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.xslt, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.xslt, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_SABLOT=false

AC_ARG_WITH(sablot,
[  --with-sablot=/path/to/sablot build sablot library against Sablotron library])
if test "$with_sablot"; then
	HAVE_SABLOT=true
	SABLOT_CFLAGS="-I$with_sablot/include"
	SABLOT_LIBS="-L$with_sablot/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $SABLOT_LIBS"

	AC_CHECK_LIB([sablot], [SablotParse],
	 [SABLOT_LIBS="$SABLOT_LIBS -lsablot -lexpat"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the SablotParse function.])], HAVE_SABLOT=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_SABLOT" = "xfalse"; then
	HAVE_SABLOT=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	SABLOT_CFLAGS="-I$my_prefix/include"
	SABLOT_LIBS="-L$my_prefix/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $SABLOT_LIBS"

	AC_CHECK_LIB([sablot], [SablotParse],
	 [SABLOT_LIBS="$SABLOT_LIBS -lsablot -lexpat"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the SablotParse function.])], HAVE_SABLOT=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_SABLOT" = "xfalse"; then
	echo
	echo "Sablotron (sablot) was not found on your system. Please use the --with-sablot=/path/to/sablot."
	echo "Sablotron library can be downloaded from http://www.gingerall.org/sablotron.html."
	exit 1
fi

AC_SUBST(SABLOT_CFLAGS)
AC_SUBST(SABLOT_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
