# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.samplerate.so
samplerate_sources = \
  libsamplerate/samplerate.c \
  libsamplerate/src_sinc.c \
  libsamplerate/src_zoh.c \
  libsamplerate/src_linear.c

standard_lib_samplerate_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h $(samplerate_sources)

