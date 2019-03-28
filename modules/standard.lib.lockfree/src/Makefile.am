# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC
AM_CXXFLAGS = --pedantic -Wall -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.lockfree.so
standard_lib_lockfree_so_SOURCES = stdlibrary.c stdlibrary.h lfq.c lfq.h lfq-platform.h main.c
