# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O2 -fPIC
AM_CXXFLAGS = -O2 -fPIC
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.stemmer.so
stemmer_sources = \
  libstemmer/src_c/stem_ISO_8859_1_danish.c \
  libstemmer/src_c/stem_UTF_8_danish.c \
  libstemmer/src_c/stem_ISO_8859_1_dutch.c \
  libstemmer/src_c/stem_UTF_8_dutch.c \
  libstemmer/src_c/stem_ISO_8859_1_english.c \
  libstemmer/src_c/stem_UTF_8_english.c \
  libstemmer/src_c/stem_ISO_8859_1_finnish.c \
  libstemmer/src_c/stem_UTF_8_finnish.c \
  libstemmer/src_c/stem_ISO_8859_1_french.c \
  libstemmer/src_c/stem_UTF_8_french.c \
  libstemmer/src_c/stem_ISO_8859_1_german.c \
  libstemmer/src_c/stem_UTF_8_german.c \
  libstemmer/src_c/stem_ISO_8859_2_hungarian.c \
  libstemmer/src_c/stem_UTF_8_hungarian.c \
  libstemmer/src_c/stem_ISO_8859_1_italian.c \
  libstemmer/src_c/stem_UTF_8_italian.c \
  libstemmer/src_c/stem_ISO_8859_1_norwegian.c \
  libstemmer/src_c/stem_UTF_8_norwegian.c \
  libstemmer/src_c/stem_ISO_8859_1_porter.c \
  libstemmer/src_c/stem_UTF_8_porter.c \
  libstemmer/src_c/stem_ISO_8859_1_portuguese.c \
  libstemmer/src_c/stem_UTF_8_portuguese.c \
  libstemmer/src_c/stem_ISO_8859_2_romanian.c \
  libstemmer/src_c/stem_UTF_8_romanian.c \
  libstemmer/src_c/stem_KOI8_R_russian.c \
  libstemmer/src_c/stem_UTF_8_russian.c \
  libstemmer/src_c/stem_ISO_8859_1_spanish.c \
  libstemmer/src_c/stem_UTF_8_spanish.c \
  libstemmer/src_c/stem_ISO_8859_1_swedish.c \
  libstemmer/src_c/stem_UTF_8_swedish.c \
  libstemmer/src_c/stem_UTF_8_turkish.c \
  libstemmer/runtime/api.c \
  libstemmer/runtime/utilities.c \
  libstemmer/libstemmer/libstemmer.c

standard_lib_stemmer_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h $(stemmer_sources)

