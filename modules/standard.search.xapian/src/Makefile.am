# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 $(XAPIAN_CFLAGS) -fPIC
AM_CPPFLAGS = -Wall -O2 $(XAPIAN_CFLAGS) -fPIC
AM_LDFLAGS = -shared $(XAPIAN_LIBS)

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.search.xapian.so
standard_search_xapian_so_SOURCES = AnsiString.cpp AnsiString.h AnsiTypes.h library.h main.cpp stdlibrary.cpp stdlibrary.h

