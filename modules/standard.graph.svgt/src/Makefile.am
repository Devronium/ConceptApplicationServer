# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @SVGT_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @SVGT_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @SVGT_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.graph.svgt.so
standard_graph_svgt_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h svgtiny.c svgtiny.h svgtiny_gradient.c svgtiny_internal.h svgtiny_list.c

