# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = web.server.api.so
web_server_api_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiString.cpp AnsiString.h AnsiTypes.h qDecoder/qArg.c qDecoder/qAwk.c qDecoder/qCount.c qDecoder/qDecoder.c qDecoder/qDecoder.h qDecoder/qDownload.c qDecoder/qEncode.c qDecoder/qEnv.c qDecoder/qError.c qDecoder/qFile.c qDecoder/qHttpHeader.c qDecoder/qInternal.h qDecoder/qInternalCommon.c qDecoder/qInternalEntry.c qDecoder/qMisc.c qDecoder/qSed.c qDecoder/qSession.c qDecoder/qSocket.c qDecoder/qString.c qDecoder/qTime.c qDecoder/qValid.c qDecoder/qfDecoder.c qDecoder/qsDecoder.c qDecoder/md5/md5.h qDecoder/md5/md5_global.h qDecoder/md5/md5c.c
