# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @MODBUS_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @MODBUS_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @MODBUS_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.modbus.so
standard_net_modbus_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
