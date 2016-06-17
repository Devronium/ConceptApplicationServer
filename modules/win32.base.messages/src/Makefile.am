# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC -DTLS_AMALGAMATION -DTLS_WITH_CHACHA20_POLY1305
AM_CXXFLAGS = -O2 -fPIC -DTLS_AMALGAMATION -DTLS_WITH_CHACHA20_POLY1305
AM_LDFLAGS = -shared -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = win32.base.messages.so
win32_base_messages_so_SOURCES = AES/AES.cpp AES/AES.h AnsiList.cpp AnsiList.h AnsiString.cpp AnsiString.h AnsiTypes.h INTERFACE_TO_AES.h library.h tlse.c tlse.h main.cpp stdlibrary.cpp stdlibrary.h