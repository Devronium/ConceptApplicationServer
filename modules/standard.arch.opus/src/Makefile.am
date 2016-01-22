# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @OPUS_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @OPUS_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @OPUS_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.opus.so
standard_arch_opus_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h g711.c

