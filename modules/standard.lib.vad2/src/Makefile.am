# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC @ONNX_CFLAGS@ -march=haswell
AM_CXXFLAGS = -O3 -fPIC @ONNX_CFLAGS@ -march=haswell
AM_LDFLAGS = -shared
standard_lib_vad2_so_LDADD = -lpthread @ONNX_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.vad2.so
standard_lib_vad2_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
