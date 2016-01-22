# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -std=gnu99 -O2 -fPIC -Imongodriver -DMONGO_USE_LONG_LONG_INT=1
AM_CXXFLAGS = -Wall -O2 -fPIC -Imongodriver -DMONGO_USE_LONG_LONG_INT=1
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.db.mongo.so
standard_db_mongo_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h mongodriver/mongo.c mongodriver/bson.c mongodriver/encoding.c mongodriver/gridfs.c mongodriver/numbers.c mongodriver/bcon.c mongodriver/env.c mongodriver/md5.c mongodriver/md5.h
