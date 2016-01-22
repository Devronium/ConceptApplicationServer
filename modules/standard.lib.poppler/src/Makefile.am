# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @POPPLER_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @POPPLER_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @POPPLER_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.poppler.so
standard_lib_poppler_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiString.cpp AnsiString.h
