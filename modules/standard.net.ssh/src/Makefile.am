# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @LIBSSH2_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @LIBSSH2_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @LIBSSH2_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.ssh.so
standard_net_ssh_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
