# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -DNO_DEPENDENCIES -fPIC
AM_CXXFLAGS = -O2 -DNO_DEPENDENCIES -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.zip.so
standard_arch_zip_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

