# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @GD_CFLAGS@ -Igdchart -fPIC
AM_CXXFLAGS = -O2 @GD_CFLAGS@ -Igdchart -fPIC
AM_LDFLAGS = -shared @GD_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.chart.so
standard_lib_chart_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h gdchart/price_conv.c gdchart/gdc.c gdchart/gdc_pie.c gdchart/gdchart.c
