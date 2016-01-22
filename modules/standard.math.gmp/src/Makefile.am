# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @GMP_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @GMP_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @GMP_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.math.gmp.so
standard_math_gmp_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

