# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.cbor.so
standard_lib_cbor_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h cbor.c cbor.h cbor/arrays.c cbor/bytestrings.c cbor/callbacks.c cbor/common.c cbor/configuration.h cbor/encoding.c cbor/floats_ctrls.c cbor/internal cbor/ints.h cbor/maps.h cbor/serialization.h cbor/streaming.h cbor/strings.h cbor/tags.h cbor/arrays.h cbor/bytestrings.h cbor/callbacks.h common.h cbor/data.h cbor/encoding.h cbor/floats_ctrls.h cbor/ints.c cbor/maps.c cbor/serialization.c cbor/streaming.c cbor/strings.c cbor/tags.c cbor/internal/builder_callbacks.c cbor/internal/encoders.c cbor/internal/loaders.c cbor/internal/memory_utils.c cbor/internal/stack.c cbor/internal/unicode.c cbor/internal/builder_callbacks.h cbor/internal/encoders.h cbor/internal/loaders.h cbor/internal/memory_utils.h cbor/internal/stack.h cbor/internal/unicode.h