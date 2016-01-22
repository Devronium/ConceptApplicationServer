# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 -fPIC
AM_CXXFLAGS = -Wall -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.routeros.so
standard_net_routeros_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h md5.c mikrotik-api.c mikrotik-api.h md5.h
