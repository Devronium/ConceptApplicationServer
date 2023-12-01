# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC -march=haswell
AM_CXXFLAGS = -O3 -fPIC -march=haswell
AM_LDFLAGS = -shared -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.whisper.so
standard_lib_whisper_so_SOURCES = ggml.h ggml.c ggml-alloc.h ggml-alloc.c ggml-backend.h ggml-backend.c ggml-quants.h ggml-quants.c whisper.h whisper.cpp whisper.h stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
