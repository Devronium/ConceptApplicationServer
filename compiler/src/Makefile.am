# what flags you want to pass to the C compiler & linker
prefix = @prefix@

AM_CFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -DLIBRARY_PATH=\"@prefix@/lib/libconceptcore.2.0.so\" -DINI_PATH=\"@prefix@/etc/concept.ini\"
AM_CXXFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -DLIBRARY_PATH=\"@prefix@/lib/libconceptcore.2.0.so\" -DINI_PATH=\"@prefix@/etc/concept.ini\"
AM_LDFLAGS = -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = accel
accel_SOURCES = AnsiString.cpp main.cpp AnsiString.h

