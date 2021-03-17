# what flags you want to pass to the C compiler & linker
prefix = @prefix@

AM_CFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -I@apache_dir@/include -I@apache_dir@/include/apache22
AM_CXXFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -I@apache_dir@/include -I@apache_dir@/include/apache22
#AM_LDFLAGS = -shared -lpthread -lhttpd -lapr-1
#AM_LDFLAGS = -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libmodconcept.la
libmodconcept_la_SOURCES = AnsiString.cpp main.cpp AnsiString.h semhh.h

