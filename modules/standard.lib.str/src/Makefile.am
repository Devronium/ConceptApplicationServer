# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC -DUTF8PROC_STATIC
AM_CXXFLAGS = --pedantic -Wall -O2 -fPIC -DUTF8PROC_STATIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.str.so
standard_lib_str_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h UriCodec.cpp utf8.c utf8.h utf8proc.c utf8proc.h
