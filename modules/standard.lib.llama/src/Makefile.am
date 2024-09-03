# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 @LLAMA_CFLAGS@ -fPIC -march=haswell
AM_CXXFLAGS = -O3 @LLAMA_CFLAGS@ -fPIC -march=haswell
AM_LDFLAGS = -shared -lpthread
standard_lib_llama_so_LDADD = -lpthread @LLAMA_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.llama.so
standard_lib_llama_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
