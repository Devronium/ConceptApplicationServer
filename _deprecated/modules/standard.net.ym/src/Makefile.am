# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @YM_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @YM_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @YM_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.ym.so
standard_net_ym_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h libyahoo2b/yahoo_util.h libyahoo2b/crypt.c libyahoo2b/libyahoo2.c libyahoo2b/md5.c libyahoo2b/md5.h libyahoo2b/sha1.c libyahoo2b/sha1.h libyahoo2b/yahoo2.h libyahoo2b/yahoo2_callbacks.h libyahoo2b/yahoo2_types.h libyahoo2b/yahoo_debug.h libyahoo2b/yahoo_fn.c libyahoo2b/yahoo_fn.h libyahoo2b/yahoo_httplib.c libyahoo2b/yahoo_httplib.h libyahoo2b/yahoo_list.c libyahoo2b/yahoo_list.h libyahoo2b/yahoo_util.c libyahoo2b/yahoo_util.h