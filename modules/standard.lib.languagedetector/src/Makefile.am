# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @CLD_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @CLD_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @CLD_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.languagedetector.so
standard_lib_languagedetector_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

