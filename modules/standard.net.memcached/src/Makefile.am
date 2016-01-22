# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @MC_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @MC_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @MC_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.memcached.so
standard_net_memcached_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
