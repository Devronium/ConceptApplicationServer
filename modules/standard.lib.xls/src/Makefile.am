# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @XLS_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @XLS_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @XLS_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.xls.so
standard_lib_xls_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
