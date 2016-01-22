# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 $(FREEIMAGE_CFLAGS) -fPIC
AM_CPPFLAGS = -Wall -O2 $(FREEIMAGE_CFLAGS) -fPIC
AM_CXXFLAGS = -Wall -O2 $(FREEIMAGE_CFLAGS) -fPIC
AM_LDFLAGS = -shared $(FREEIMAGE_LIBS)

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = win32.graph.freeimage.so
win32_graph_freeimage_so_SOURCES = AnsiTypes.h library.h main.cpp AnsiString.h AnsiString.cpp stdlibrary.cpp stdlibrary.h

