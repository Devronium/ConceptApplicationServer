# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @WK_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @WK_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @WK_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.graph.wk.so
standard_graph_wk_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

