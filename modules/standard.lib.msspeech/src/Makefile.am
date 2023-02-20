# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @MSSPEECH_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @MSSPEECH_CFLAGS@ -fPIC
AM_LDFLAGS = -shared
standard_lib_msspeech_so_LDADD = @MSSPEECH_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.msspeech.so
standard_lib_msspeech_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
