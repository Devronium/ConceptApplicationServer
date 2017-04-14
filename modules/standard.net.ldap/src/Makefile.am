# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -std=c99 -O2 -fPIC @LDAP_CFLAGS@
AM_CXXFLAGS = -Wall -O2 -fPIC @LDAP_CFLAGS@
AM_LDFLAGS = -shared @LDAP_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.ldap.so
standard_net_ldap_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
