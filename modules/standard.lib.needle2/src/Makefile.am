# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -pthread -O3 -fPIC -march=haswell
AM_CXXFLAGS = -pthread -O3 -fPIC -march=haswell
AM_LDFLAGS = -shared
standard_lib_needle2_so_LDADD = -lpthread -L. -lneedle

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.needle2.so
standard_lib_needle2_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
