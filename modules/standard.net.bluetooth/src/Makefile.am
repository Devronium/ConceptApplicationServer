# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @BT_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @BT_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @BT_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.bluetooth.so
standard_net_bluetooth_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
