# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @SSL_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @SSL_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @SSL_LIBS@ -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = win32.base.messages.so
win32_base_messages_so_SOURCES = AES/AES.cpp AES/AES.h AnsiList.cpp AnsiList.h AnsiString.cpp AnsiString.h AnsiTypes.h INTERFACE_TO_AES.h library.h main.cpp sha256.c sha256.h stdlibrary.cpp stdlibrary.h