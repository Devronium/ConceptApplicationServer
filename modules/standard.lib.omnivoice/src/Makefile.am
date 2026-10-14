# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC -march=haswell @OMNIVOICE_CFLAGS@
AM_CXXFLAGS = -O3 -fPIC -march=haswell -std=c++17 @OMNIVOICE_CFLAGS@
AM_LDFLAGS = -shared
standard_lib_omnivoice_so_LDADD = -lpthread @OMNIVOICE_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.omnivoice.so
standard_lib_omnivoice_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
