# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @SNMP_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @SNMP_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @SNMP_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.snmp.so
standard_net_snmp_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
