# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @PQ_CFLAGS@ -fPIC -w
AM_CXXFLAGS = -O2 @PQ_CFLAGS@ -fPIC -w
AM_LDFLAGS = -shared @PQ_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.db.pq.so
standard_db_pq_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h

