# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @CODEC2_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @CODEC2_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @CODEC2_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.codec2.so
standard_arch_codec2_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h

