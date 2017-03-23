# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 -fPIC -Wno-unused-variable
AM_CXXFLAGS = -Wall -O2 -fPIC -Wno-unused-variable
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.poll.so
standard_net_poll_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h

