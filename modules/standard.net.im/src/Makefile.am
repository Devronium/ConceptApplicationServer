# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @PURPLE_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @PURPLE_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @PURPLE_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.im.so
standard_net_im_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

