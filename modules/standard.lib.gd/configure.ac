#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lib.gd, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lib.gd, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_GD=false

AC_ARG_WITH(gd,
[  --with-gd=/path/to/gd build gd library against gd])
if test "$with_gd"; then
	HAVE_GD=true
	GD_CFLAGS="-I$with_gd/include"
	GD_LIBS="-L$with_gd/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $GD_LIBS"

	AC_CHECK_LIB([gd], [gdImageDestroy],
	 [GD_LIBS="$GD_LIBS -lgd"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the gdImageDestroy function.])], HAVE_GD=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_GD" = "xfalse"; then
	HAVE_GD=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	GD_CFLAGS="-I$my_prefix/include"
	GD_LIBS="-L$my_prefix/lib"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $GD_LIBS"

	AC_CHECK_LIB([gd], [gdImageDestroy],
	 [GD_LIBS="$GD_LIBS -lgd"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the gdImageDestroy function.])], HAVE_GD=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_GD" = "xfalse"; then
	echo
	echo "libgd was not found on your system. Please use the --with-gd=/path/to/libgd."
	exit 1
fi

AC_SUBST(GD_CFLAGS)
AC_SUBST(GD_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
