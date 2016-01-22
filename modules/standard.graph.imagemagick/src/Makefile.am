# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @MAGICK_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @MAGICK_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @MAGICK_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.graph.imagemagick.so
standard_graph_imagemagick_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

