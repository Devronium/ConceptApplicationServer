# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC -w "-DCONFIG_VERSION=\"2024-01-13\""
AM_CXXFLAGS = -O2 -fPIC -w "-DCONFIG_VERSION=\"2024-01-13\""
AM_LDFLAGS = -shared
LIBS = -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lang.quickjs.so
standard_lang_quickjs_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h ./quickjs/cutils.c ./quickjs/cutils.h ./quickjs/libbf.c ./quickjs/libbf.h ./quickjs/libregexp-opcode.h ./quickjs/libregexp.c ./quickjs/libregexp.h ./quickjs/libunicode-table.h ./quickjs/libunicode.c ./quickjs/libunicode.h ./quickjs/list.h ./quickjs/quickjs-atom.h ./quickjs/quickjs-opcode.h ./quickjs/quickjs.c ./quickjs/quickjs.h ./quickjs/unicode_gen_def.h

