#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.db.mysql, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.db.mysql, 1.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

HAVE_MYSQL=false
# Checks for libraries.
dnl check for mysqlclient

AC_ARG_WITH(mysql,
[  --with-mysql=/path/to/libmysqlclient path to libmysqlclient])
if test "$with_mysql"; then
	HAVE_MYSQL=true
	MYSQL_CFLAGS="-I$with_mysql/include/mysql"
	MYSQL_LIBS="-L$with_mysql/lib/mysql"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $MYSQL_LIBS"
	AC_CHECK_LIB(mysqlclient, mysql_real_query,
	 [MYSQL_LIBS="$MYSQL_LIBS -lmysqlclient"
	 AC_DEFINE_UNQUOTED(HAVE_MYSQLREALQUERY, 1,
	 [Define to 1 if you have the mysql_real_query function.])], HAVE_MYSQL=false)
	LDFLAGS=$OLDLDFLAGS
fi


dnl trying to get default library ...
if test "x$HAVE_MYSQL" = "xfalse"; then
	HAVE_MYSQL=true
	sql_prefix=$prefix

	if test "x$sql_prefix" = "xNONE"; then
    		sql_prefix="/usr/local"
	fi

	MYSQL_CFLAGS="-I$sql_prefix/include/mysql"
	MYSQL_LIBS="-L$sql_prefix/lib/mysql"

	OLDLDFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS $MYSQL_LIBS"
	AC_CHECK_LIB(mysqlclient, mysql_real_query,
	 [MYSQL_LIBS="$MYSQL_LIBS -lmysqlclient"
	 AC_DEFINE_UNQUOTED(HAVE_MYSQLREALQUERY, 1,
	 [Define to 1 if you have the mysql_real_query function.])], HAVE_MYSQL=false)
	LDFLAGS=$OLDLDFLAGS
fi




if test "x$HAVE_MYSQL" = "xfalse"; then
	echo
	echo no mysql client library found.
	exit 1
fi

AC_SUBST(MYSQL_CFLAGS)
AC_SUBST(MYSQL_LIBS)
OPTDIRS="$OPTDIRS mysql"



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
