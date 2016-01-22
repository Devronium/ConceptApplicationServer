# what flags you want to pass to the C compiler & linker
AM_CFLAGS =  -O2 -fPIC
AM_CXXFLAGS = -fPIC -O2
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.amf.so
standard_lib_amf_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h amf_boolean.cpp amf_item.cpp amf_numeric.cpp amf_string.cpp amf_date.cpp amf_list.cpp amf_object.cpp amf_ecma_array.cpp amf_long_string.cpp amf_strict_array.cpp

