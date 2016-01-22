# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @ICONV_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @ICONV_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @ICONV_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.iconv.so
standard_lib_iconv_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

