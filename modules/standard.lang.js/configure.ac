#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lang.js, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lang.js, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
HAVE_JS=false

AC_ARG_WITH(js,
[  --with-js=/path/to/js build js library against SpiderMonkey])
if test "$with_js"; then
	HAVE_JS=true
	JS_CFLAGS="-I$with_js/include"
	JS_LIBS="-L$with_js/lib -lpthread -lm"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $JS_LIBS"

	AC_CHECK_LIB([js], [JS_DefineFunction],
	 [JS_LIBS="$JS_LIBS -ljs"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the JS_DefineFunction function.])], HAVE_JS=false)

	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_JS" = "xfalse"; then
	HAVE_JS=true
	my_prefix=$prefix

	if test "x$my_prefix" = "xNONE"; then
    		my_prefix="/usr/local"
	fi

	JS_CFLAGS="-I$my_prefix/include"
	JS_LIBS="-L$my_prefix/lib -lpthread -lm"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $JS_LIBS"

	AC_CHECK_LIB([js], [JS_DefineFunction],
	 [JS_LIBS="$JS_LIBS -ljs"
	 AC_DEFINE_UNQUOTED(HAVE_FUNCTION, 1,
	 [Define to 1 if you have the JS_DefineFunction function.])], HAVE_JS=false)

	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_JS" = "xfalse"; then
	echo
	echo "SpiderMonkey (js) was not found on your system. Please use the --with-js=/path/to/js."
	echo "SpiderMonkey can be downloaded from http://www.mozilla.org/js/spidermonkey/."
	exit 1
fi

AC_SUBST(JS_CFLAGS)
AC_SUBST(JS_LIBS)



# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
