# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 -fPIC
AM_CXXFLAGS = -Wall -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lang.serialize.so
standard_lang_serialize_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h pugixml.cpp pugixml.hpp pugiconfig.hpp
