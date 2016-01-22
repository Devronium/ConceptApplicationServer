# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = web.service.template.so
web_service_template_so_SOURCES = AnsiList.cpp AnsiList.h AnsiString.cpp AnsiString.h AnsiTypes.h TemplateElement.h Templatizer.cpp Templatizer.h library.h main.cpp stdlibrary.cpp stdlibrary.h
