# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC
AM_CXXFLAGS = --pedantic -Wall-O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.rtp.lite.so
standard_net_rtp_lite_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h RTP.c RTP.h Utils.c Network.c Network.h Types.h Hdr_Builder.c Macros.h Config.h Proto.h

