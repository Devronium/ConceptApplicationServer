#ifndef __SQLSTMT_H
#define __SQLSTMT_H

#include <sqlite3.h>
#include <string.h>
#include "SQLDB.h"

class SQLSTMT {
private:
    SQLDB        *db;
    sqlite3_stmt *stmt;

    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        return 0;
    }

public:
    SQLSTMT(SQLDB *db) {
        this->db   = db;
        this->stmt = 0;
    }

    ~SQLSTMT() {
        Free();
    }

    int Free() {
        if (!stmt)
            return -1;
        int res = sqlite3_finalize(stmt);
        stmt = 0;
        return res;
    }

    int ParamCount() {
        if (!stmt)
            return -1;

        return sqlite3_bind_parameter_count(stmt);
    }

    int Parameter(int index, char *data, int len) {
        int res = -1;

        if (!stmt)
            return res;

        if (len >= 0xFF) {
            res = sqlite3_bind_blob(stmt, index, data, len, SQLITE_TRANSIENT);
            if (res != SQLITE_OK)
                res = sqlite3_bind_text(stmt, index, data, len, SQLITE_TRANSIENT);
        } else {
            if (strlen(data) != len)
                res = sqlite3_bind_blob(stmt, index, data, len, SQLITE_TRANSIENT);
            else
                res = sqlite3_bind_text(stmt, index, data, len, SQLITE_TRANSIENT);
        }
        return res;
    }

    int BlobParameter(int index, char *data, int len) {
        if (!stmt)
            return -1;
        return sqlite3_bind_blob(stmt, index, data, len, SQLITE_TRANSIENT);
    }

    int Parameter(char *name, char *data, int len) {
        if (!stmt)
            return -1;

        int index = sqlite3_bind_parameter_index(stmt, name);
        int res   = sqlite3_bind_blob(stmt, index, data, len, SQLITE_TRANSIENT);
        if (res != SQLITE_OK)
            res = sqlite3_bind_text(stmt, index, data, len, SQLITE_TRANSIENT);
        return res;
    }

    int BlobParameter(char *name, char *data, int len) {
        if (!stmt)
            return -1;
        int index = sqlite3_bind_parameter_index(stmt, name);
        return sqlite3_bind_blob(stmt, index, data, len, SQLITE_TRANSIENT);
    }

    int Parameter(char *name, int size) {
        if (!stmt)
            return -1;

        int index = sqlite3_bind_parameter_index(stmt, name);
        return sqlite3_bind_zeroblob(stmt, index, size);
    }

    int Parameter(int index, int size) {
        if (!stmt)
            return -1;

        return sqlite3_bind_zeroblob(stmt, index, size);
    }

    int Prepare(char *query, int size = -1) {
        if ((!db) || (!db->db))
            return -1;

        if (stmt) {
            sqlite3_finalize(stmt);
            stmt = 0;
        }
        int res = sqlite3_prepare_v2(db->db, query, size, &stmt, 0);

        if (res != SQLITE_OK)
            stmt = 0;

        return res;
    }

    int Next() {
        if (!stmt)
            return -1;

        int res = sqlite3_step(stmt);

        return res;
    }

    int Reset() {
        if (!stmt)
            return -1;

        return sqlite3_reset(stmt);
    }

    int First() {
        int res = Reset();

        if (res != SQLITE_OK)
            return res;
        return Next();
    }

    int ClearBindings() {
        if (!stmt)
            return -1;

        return sqlite3_clear_bindings(stmt);
    }

    int CountColumns() {
        if (!stmt)
            return -1;

        return sqlite3_column_count(stmt);
    }

    double GetColumn(int index) {
        if (!stmt)
            return -1;

        return sqlite3_column_double(stmt, index);
    }

    int ReadColumnBLOB(int index, int rowid, char **buffer, int size, int offset) {
        if (!stmt)
            return -1;

        if (!size)
            return 0;

        const char *table  = sqlite3_column_table_name(stmt, index);
        const char *column = sqlite3_column_origin_name(stmt, index);

        sqlite3_blob *blob;
        int          rf = sqlite3_blob_open(db->db, 0, table, column, rowid, 0, &blob);

        if ((rf != SQLITE_OK) || (!blob))
            return -1;

        int bytes = sqlite3_blob_bytes(blob);

        if (offset > bytes) {
            sqlite3_blob_close(blob);
            return 0;
        }

        int end = offset + size;

        if (end > bytes) {
            size = bytes - offset;
        }
        if (!size) {
            sqlite3_blob_close(blob);
            return 0;
        }


        *buffer = new char[size];

        bytes = size;

        int res = sqlite3_blob_read(blob, *buffer, size, offset);


        if (res != SQLITE_OK) {
            delete[] *buffer;
            *buffer = 0;
            bytes   = -1;
        }

        sqlite3_blob_close(blob);

        return bytes;
    }

    int WriteColumnBLOB(int index, int rowid, char *buffer, int size, int offset) {
        if (!stmt)
            return -1;
        if (!size)
            return 0;

        const char *table  = sqlite3_column_table_name(stmt, index);
        const char *column = sqlite3_column_origin_name(stmt, index);

        sqlite3_blob *blob;
        int          rf = sqlite3_blob_open(db->db, 0, table, column, rowid, 1, &blob);

        if ((rf != SQLITE_OK) || (!blob))
            return -1;

        int bytes = sqlite3_blob_bytes(blob);

        if (offset > bytes) {
            sqlite3_blob_close(blob);
            return -1;
        }

        int res = sqlite3_blob_write(blob, buffer, size, offset);

        sqlite3_blob_close(blob);

        return res;
    }

    const char *GetColumnText(int index) {
        if (!stmt)
            return 0;

        return (const char *)sqlite3_column_text(stmt, index);
    }

    const char *GetColumnBlob(int index, int *len) {
        *len = 0;
        if (!stmt)
            return 0;

        *len = sqlite3_column_bytes(stmt, index);
        return (const char *)sqlite3_column_blob(stmt, index);
    }

    const char *ColumnDeclType(int index) {
        if (!stmt)
            return 0;

        return sqlite3_column_decltype(stmt, index);
    }

    const char *ColumnName(int index) {
        if (!stmt)
            return 0;

        return sqlite3_column_name(stmt, index);
    }

    int ColumnType(int index) {
        if (!stmt)
            return -1;

        return sqlite3_column_type(stmt, index);
    }
};
#endif // __SQLSTMT_H
