# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC
AM_CXXFLAGS = --pedantic -Wall -O2 -fPIC
AM_LDFLAGS = -shared
LIBS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.qr.so
standard_lib_qr_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h QR_Encode.c QR_Encode.h code128.c code128.h ean13.c ean13.h tiny_jpeg.h


