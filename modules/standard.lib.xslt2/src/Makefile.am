# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @XSLT_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @XSLT_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @XSLT_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.xslt2.so
standard_lib_xslt2_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

