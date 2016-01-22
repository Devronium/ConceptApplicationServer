# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.regex.so
standard_lib_regex_so_SOURCES = AnsiString.cpp AnsiString.h AnsiTypes.h library.h main.cpp memory.c regerror.c regexp.c regexp.h regexp_custom.h regexp_int.h regmagic.h regsub.c report.c stdlibrary.cpp stdlibrary.h widechar.c
