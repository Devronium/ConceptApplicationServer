# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -DWITH_PARSON -fPIC
AM_CXXFLAGS = -O2 -DWITH_PARSON -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.json.so
standard_lib_json_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h parson.c parson.h

