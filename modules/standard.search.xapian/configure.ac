#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(standard.search.xapian, 1.0, eduard@radgs.ro)
AM_INIT_AUTOMAKE(standard.search.xapian, 1.0)
AC_CONFIG_SRCDIR([src/AnsiTypes.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

#### Look for Xapian
AC_PATH_PROG(XAPIAN_CONFIG, xapian-config, no)
if test "$XAPIAN_CONFIG" = "no" ; then
   AC_MSG_ERROR([Cannot find xapian-config command in $PATH. Is
xapian-core installed ?])
   exit 1
fi
LIBXAPIAN=`$XAPIAN_CONFIG --libs`
# Workaround for problem in xapian-config in some versions: wrongly lists
# libstdc++.la in the lib list
for i in $LIBXAPIAN ; do
    case $i in
    *stdc++*|-lm|-lgcc_s|-lc);;
    *) tmpxaplib="$tmpxaplib $i";;
    esac
done
LIBXAPIAN=$tmpxaplib
# Also recent xapian libs need lz even when they think they don't...
LIBXAPIAN="$LIBXAPIAN -lz"
XAPIANCXXFLAGS=`$XAPIAN_CONFIG --cxxflags`

XAPIAN_CFLAGS="$XAPIANCXXFLAGS"
XAPIAN_LIBS="$LIBXAPIAN"

AC_SUBST(XAPIAN_CFLAGS)
AC_SUBST(XAPIAN_LIBS)



# Checks for header files.
AC_CHECK_HEADERS([wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
