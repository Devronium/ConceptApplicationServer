# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 @RWKV_CFLAGS@ -fPIC -march=haswell
AM_CXXFLAGS = -O3 @RWKV_CFLAGS@ -fPIC -march=haswell -std=c++17
AM_LDFLAGS = -shared
standard_lib_rwkv_so_LDADD = -lpthread @RWKV_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.rwkv.so
standard_lib_rwkv_so_SOURCES = stdlibrary.cpp stdlibrary.h AnsiTypes.h AnsiString.h AnsiString.cpp main.cpp
