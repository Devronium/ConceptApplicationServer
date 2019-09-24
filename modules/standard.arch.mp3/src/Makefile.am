# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -std=c99 -O2 -fPIC
AM_CXXFLAGS = -Wall -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.mp3.so
standard_arch_mp3_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h shine/bitstream.c shine/huffman.c shine/l3bitstream.c shine/l3loop.c shine/l3mdct.c shine/l3subband.c shine/layer3.c shine/reservoir.c shine/tables.c