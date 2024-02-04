# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC -march=haswell @ONNX_CFLAGS@ @ESPEAK_CFLAGS@
AM_CXXFLAGS = -O3 -fPIC -march=haswell -std=c++17 -I./include/ -I./include/piper-phonemize/ @ONNX_CFLAGS@ @ESPEAK_CFLAGS@
AM_LDFLAGS = -shared
standard_lib_piper_so_LDADD = -lpthread @ONNX_LIBS@ @ESPEAK_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.piper.so
standard_lib_piper_so_SOURCES = phonemize.cpp phoneme_ids.cpp tashkeel.cpp piper.cpp stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
