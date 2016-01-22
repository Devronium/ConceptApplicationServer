#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.lang.serialize, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.lang.serialize, 1.0)
AC_CONFIG_SRCDIR([src/AnsiString.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lxml2':
# AC_CHECK_LIB([xml2], [xmlReaderForFile])

# pkg_modules="libxml-2.0 >= 2.6"
# PKG_CHECK_MODULES(XML, [$pkg_modules])

#$GTKMM_CFLAGS=`pkg-config --cflags gtkmm-2.4`
#$GTKMM_LIBS=`pkg-config --libs gtkmm-2.4`
# AC_SUBST(XML_CFLAGS)
# AC_SUBST(XML_LIBS)


#HAVE_XML=false

#AC_ARG_WITH(xml2,
#[  --with-xml2=/path/to/xml2 build odbc driver against libxml2])
#if test "$with_xml2"; then
#	HAVE_XML=true
#	XML_CFLAGS="-I$with_xml2/include"
#	XML_LIBS="-L$with_xml2/lib"
#
#	OLDLDFLAGS=$LDFLAGS
#	LDFLAGS="$LDFLAGS $XML_LIBS"
#
#	AC_CHECK_LIB(xml2, xmlReaderForFile,
#	 [XML_LIBS="$XML_LIBS -lxml2"
#	 AC_DEFINE_UNQUOTED(HAVE_XMLREADERFORFILE, 1,
#	 [Define to 1 if you have the xmlReaderForFile function.])], HAVE_XML=false)
#	LDFLAGS=$OLDLDFLAGS
#fi
#
#if test "x$HAVE_XML" = "xfalse"; then
#	HAVE_XML=true
#	XML_CFLAGS="-I/usr/include"
#	XML_LIBS="-L/usr/lib"
#
#	OLDLDFLAGS=$LDFLAGS
#	LDFLAGS="$LDFLAGS $XML_LIBS"
#
#	AC_CHECK_LIB(xml2, xmlReaderForFile,
#	 [XML_LIBS="$XML_LIBS -lxml2"
#	 AC_DEFINE_UNQUOTED(HAVE_XMLREADERFORFILE, 1,
#	 [Define to 1 if you have the xmlReaderForFile function.])], HAVE_XML=false)
#	LDFLAGS=$OLDLDFLAGS
#fi
#
#if test "x$HAVE_XML" = "xfalse"; then
#	HAVE_XML=true
#	XML_CFLAGS="-I/usr/local/include"
#	XML_LIBS="-L/usr/local/lib"
#
#	OLDLDFLAGS=$LDFLAGS
#	LDFLAGS="$LDFLAGS $XML_LIBS"
#
#	AC_CHECK_LIB(xml2, xmlReaderForFile,
#	 [XML_LIBS="$XML_LIBS -lxml2"
#	 AC_DEFINE_UNQUOTED(HAVE_XMLREADERFORFILE, 1,
#	 [Define to 1 if you have the xmlReaderForFile function.])], HAVE_XML=false)
#	LDFLAGS=$OLDLDFLAGS
#fi
#
#if test "x$HAVE_XML" = "xfalse"; then
#	echo
#	echo No libxml2 was found on this system. Please specify the full path to libxml2 by using the --with-xml2=/path/to/xml2
#	exit 1
#fi
#
#AC_SUBST(XML_CFLAGS)
#AC_SUBST(XML_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
