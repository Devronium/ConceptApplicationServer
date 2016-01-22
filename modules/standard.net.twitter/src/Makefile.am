# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 @CURL_CFLAGS@ -fPIC
AM_CXXFLAGS = -O2 @CURL_CFLAGS@ -fPIC
AM_LDFLAGS = -shared @CURL_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.net.twitter.so
standard_net_twitter_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h twitcurl.cpp twitcurl.h urlencode.cpp urlencode.h oauthlib.cpp oauthlib.h HMAC_SHA1.cpp HMAC_SHA1.h SHA1.cpp SHA1.h base64.cpp base64.h

