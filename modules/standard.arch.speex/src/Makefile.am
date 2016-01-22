# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @SPEEX_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @SPEEX_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @SPEEX_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.speex.so
standard_arch_speex_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h g711.c

