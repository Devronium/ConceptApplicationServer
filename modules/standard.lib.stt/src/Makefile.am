# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @STT_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @STT_CFLAGS@ -fPIC
AM_LDFLAGS = -shared
standard_lib_stt_so_LDADD = @STT_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.stt.so
standard_lib_stt_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
