# what flags you want to pass to the C compiler & linker
# AM_CFLAGS = -O2 @SQLITE_CFLAGS@ -fPIC -DSQLITE_ENABLE_COLUMN_METADATA=1
# AM_CXXFLAGS = -O2 @SQLITE_CFLAGS@ -fPIC -DSQLITE_ENABLE_COLUMN_METADATA=1
# AM_LDFLAGS = -shared @SQLITE_LIBS@
AM_CFLAGS = -O2 -fPIC -DSQLITE_ENABLE_COLUMN_METADATA=1
AM_CXXFLAGS = -O2 -fPIC -DSQLITE_ENABLE_COLUMN_METADATA=1
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.db.sqlite.so 
standard_db_sqlite_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h AnsiString.cpp AnsiString.h SQLDB.h SQLSTMT.h sqlite3.c sqlite3.h sqlite3ext.h

