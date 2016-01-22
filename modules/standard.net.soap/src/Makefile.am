# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @CSOAP_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @CSOAP_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @CSOAP_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.soap.so
standard_net_soap_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

