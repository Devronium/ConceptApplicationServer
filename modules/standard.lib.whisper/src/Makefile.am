# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.whisper.so
standard_lib_whisper_so_SOURCES = ggml.h ggml.c whisper.h whisper.cpp whisper.h stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
