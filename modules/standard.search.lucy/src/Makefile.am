# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @LUCY_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @LUCY_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @LUCY_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.search.lucy.so
standard_search_lucy_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp library.h
