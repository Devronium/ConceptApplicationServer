# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 $(FB_CFLAGS) -fPIC
AM_CPPFLAGS = -Wall -O2 $(FB_CFLAGS) -fPIC
AM_CXXFLAGS = -Wall -O2 $(FB_CFLAGS) -fPIC
AM_LDFLAGS = -shared $(FB_LIBS)

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.db.firebird.so
standard_db_firebird_so_SOURCES = AnsiString.cpp AnsiString.h AnsiTypes.h library.h main.cpp stdlibrary.cpp stdlibrary.h

