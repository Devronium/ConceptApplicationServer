# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC -march=haswell -Iggml/include -Iggml/src
AM_CXXFLAGS = -O3 -fPIC -march=haswell -Iggml/include -Iggml/src
AM_LDFLAGS = -shared -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.llama.so
standard_lib_llama_so_SOURCES = ggml/src/ggml.c ggml/src/ggml-alloc.c ggml/src/ggml-backend.c ggml/src/ggml-quants.c ggml/src/ggml-aarch64.c llama-grammar.cpp llama-grammar.h llama-impl.h llama-sampling.cpp llama-sampling.h llama-vocab.cpp llama-vocab.h llama.cpp llama.h unicode.cpp unicode.h unicode-data.cpp unicode-data.h stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
