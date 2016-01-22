# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 @OPENCV_CFLAGS@ -fPIC
AM_CXXFLAGS = -Wall -O2 @OPENCV_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @OPENCV_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.face.so
standard_lib_face_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h
