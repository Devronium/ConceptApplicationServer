#ifndef __SQLDB_H
#define __SQLDB_H

#include <sqlite3.h>

typedef void (*CBFunc)(sqlite3_context *, int, sqlite3_value **);

class SQLDB {
    friend class SQLSTMT;
private:
    sqlite3 *db;
public:
    SQLDB() {
        db = 0;
    }

    int Open(char *data) {
        int res = sqlite3_open(data, &db);

        if (res) {
            sqlite3_close(db);
            db = 0;
            return -1;
        }
        if (db)
            sqlite3_busy_timeout(db, 3000);
        return 0;
    }

    int SetTimeout(int ms) {
        if (db) {
            return sqlite3_busy_timeout(db, ms);
        }
        return -1;
    }

    int Close() {
        if (db) {
            int res = sqlite3_close(db);
            if (!res)
                db = 0;
            return res;
        }
        return -1;
    }

    int ErrorCode() {
        return db ? sqlite3_errcode(db) : 0;
    }

    const char *Error() {
        if ((sqlite3_errcode(db) == SQLITE_OK) || (sqlite3_errcode(db) >= SQLITE_ROW))
            return 0;
        return db ? sqlite3_errmsg(db) : 0;
    }

    int Changes() {
        return db ? sqlite3_changes(db) : -1;
    }

    int LastRowID() {
        return db ? sqlite3_last_insert_rowid(db) : -1;
    }

    int WrapFunction(char *name, CBFunc func, void *data, int params) {
        return sqlite3_create_function(db, name, params, SQLITE_UTF8, data, func, 0, 0);
    }
};
#endif // __SQLDB_H
