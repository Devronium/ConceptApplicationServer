# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 $(ODBC_CFLAGS) -fPIC
AM_CPPFLAGS = -Wall -O2 $(ODBC_CFLAGS) -fPIC
AM_CXXFLAGS = -Wall -O2 $(ODBC_CFLAGS) -fPIC
AM_LDFLAGS = -shared $(ODBC_LIBS)

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.db.sql.so
standard_db_sql_so_SOURCES = AnsiList.cpp AnsiList.h AnsiString.cpp AnsiString.h AnsiTypes.h SQLStatament.cpp SQLStatament.h library.h main.cpp odbcman.cpp odbcman.h stdlibrary.cpp stdlibrary.h

