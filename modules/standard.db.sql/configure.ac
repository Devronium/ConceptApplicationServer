#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.db.sql, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.db.sql, 1.0)
AC_CONFIG_SRCDIR([src/AnsiList.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

HAVE_ODBC=false
# Checks for libraries.
dnl check for iODBC

AC_ARG_WITH(iodbc,
[  --with-iodbc=/path/to/iodbc build odbc driver against iODBC])
if test "$with_iodbc"; then
	HAVE_ODBC=true
	ODBC_CFLAGS="-I$with_iodbc/include"
	ODBC_LIBS="-L$with_iodbc/lib"
	CFLAGS="$CFLAGS -DIODBC"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ODBC_LIBS"
	AC_CHECK_LIB(iodbc, SQLDataSources,
	 [ODBC_LIBS="$ODBC_LIBS -liodbc"
	 AC_DEFINE_UNQUOTED(HAVE_SQLDATASOURCES, 1,
	 [Define to 1 if you have the SQLDataSources function.])], HAVE_ODBC=false)
	LDFLAGS=$OLDLDFLAGS
fi

dnl check for unixODBC
AC_ARG_WITH(unixodbc,
[  --with-unixodbc=/path/to/unixodbc build odbc driver against unixODBC])
if test "$with_unixodbc"; then
	HAVE_ODBC=true
	ODBC_CFLAGS="-I$with_unixodbc/include"
	ODBC_LIBS="-L$with_unixodbc/lib"
	CFLAGS="$CFLAGS -DUNIXODBC"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ODBC_LIBS"
	AC_CHECK_LIB(odbc, SQLDataSources,
	 [ODBC_LIBS="$ODBC_LIBS -lodbc"
	 AC_DEFINE_UNQUOTED(HAVE_SQLGETPRIVATEPROFILESTRING, 1,
	 [Define to 1 if you have the SQLDataSources function.])], HAVE_ODBC=false)
	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_ODBC" = "xfalse"; then
	HAVE_ODBC=true
	sql_prefix=$prefix

	if test "x$sql_prefix" = "xNONE"; then
    		sql_prefix="/usr/local"
	fi

	ODBC_CFLAGS="-I$sql_prefix/include"
	ODBC_LIBS="-L$sql_prefix/lib"
	CFLAGS="$CFLAGS -DIODBC"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ODBC_LIBS"
	AC_CHECK_LIB(odbc, SQLDataSources,
	 [ODBC_LIBS="$ODBC_LIBS -lodbc"
	 AC_DEFINE_UNQUOTED(HAVE_SQLDATASOURCES, 1,
	 [Define to 1 if you have the SQLDataSources function.])], HAVE_ODBC=false)
	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_ODBC" = "xfalse"; then
	HAVE_ODBC=true
	sql_prefix=$prefix

	if test "x$sql_prefix" = "xNONE"; then
    		sql_prefix="/usr"
	fi

	ODBC_CFLAGS="-I$sql_prefix/include"
	ODBC_LIBS="-L$sql_prefix/lib"
	CFLAGS="$CFLAGS -DIODBC"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $ODBC_LIBS"
	AC_CHECK_LIB(odbc, SQLDataSources,
	 [ODBC_LIBS="$ODBC_LIBS -lodbc"
	 AC_DEFINE_UNQUOTED(HAVE_SQLDATASOURCES, 1,
	 [Define to 1 if you have the SQLDataSources function.])], HAVE_ODBC=false)
	LDFLAGS=$OLDLDFLAGS
fi


if test "x$HAVE_ODBC" = "xfalse"; then
	echo
	echo no ODBC manager found. Please install iODBC or unixODBC. Also, for MySQL you need the MyODBC driver.
	exit 1
fi

AC_SUBST(ODBC_CFLAGS)
AC_SUBST(ODBC_LIBS)
OPTDIRS="$OPTDIRS odbc"



# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
