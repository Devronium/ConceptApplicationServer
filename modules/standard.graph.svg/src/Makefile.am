# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @SVG_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @SVG_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @SVG_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.graph.svg.so
standard_graph_svg_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h nanosvg.h nanosvgrast.h stb_image_writer.h

