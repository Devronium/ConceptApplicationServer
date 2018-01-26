# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC -w
AM_CXXFLAGS = -O2 -fPIC -w
AM_LDFLAGS = -shared -lm

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lang.basic.so
standard_lang_basic_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h my_basic.c my_basic.h

