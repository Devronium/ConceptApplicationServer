# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC -w
AM_CXXFLAGS = -O2 -fPIC -w
AM_LDFLAGS = -shared -lm

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.mustache.so
standard_lib_mustache_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h mustach.c mustach.h
