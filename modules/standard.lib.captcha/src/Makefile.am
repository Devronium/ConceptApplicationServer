# what flags you want to pass to the C compiler & linker
AM_CFLAGS =  -O2 -fPIC
AM_CXXFLAGS = -fPIC -O2
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.captcha.so
standard_lib_captcha_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h libcaptcha.c

