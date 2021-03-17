# what flags you want to pass to the C compiler & linker
prefix = @prefix@
AM_CFLAGS = -O2 @MC_CFLAGS@ @GEOIP_CFLAGS@
AM_CXXFLAGS = -O2 @MC_CFLAGS@ @GEOIP_CFLAGS@
AM_LDFLAGS = -shared @MC_LIBS@ @GEOIP_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libmod_conceptcache.la
libmod_conceptcache_la_SOURCES = AnsiString.cpp main.cpp AnsiString.h md5/md5c.c md5/md5.h md5/md5_global.h

