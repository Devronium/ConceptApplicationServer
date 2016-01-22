/************************************************************************
   qDecoder - Web Application Interface for C/C++    http://www.qDecoder.org

   Copyright (C) 2001 The qDecoder Project.
   Copyright (C) 1999,2000 Hongik Internet, Inc.
   Copyright (C) 1998 Nobreak Technologies, Inc.
   Copyright (C) 1996,1997 Seung-young Kim.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   Copyright Disclaimer:
   Hongik Internet, Inc., hereby disclaims all copyright interest.
   President, Christopher Roh, 6 April 2000

   Nobreak Technologies, Inc., hereby disclaims all copyright interest.
   President, Yoon Cho, 6 April 2000

   Seung-young Kim, hereby disclaims all copyright interest.
   Author, Seung-young Kim, 6 April 2000
************************************************************************/

#include "qDecoder.h"
#include "qInternal.h"


/**********************************************
** Internal Functions Definition
**********************************************/

static int  _parse_urlencoded(void);
static char *_get_query(char *method);
static int  _parse_multipart_data(void);


/**********************************************
** Static Values Definition used only internal
**********************************************/

static Q_Entry *_first_entry      = NULL;
static Q_Entry *_multi_last_entry = NULL;
static char    _multi_last_key[1024];

static int _cookie_cnt = 0, _get_cnt = 0, _post_cnt = 0, _new_cnt = 0; /* counts per methods */

/**********************************************
** Usage : qDecoder();
** Return: Success number of values, Fail -1.
** Do    : Decode Query & Cookie then save it in linked list.
**         It doesn't care Method.
**********************************************/
int qDecoder(void) {
    int  amount = -1;
    char *content_type;

    if (_first_entry != NULL) return -1;

    content_type = "";
    content_type = getenv("CONTENT_TYPE");

    if (content_type == NULL) {
        amount = _parse_urlencoded();
    }
    /* for application/x-www-form-urlencoded */
    else if (!strncmp(content_type, "application/x-www-form-urlencoded", strlen("application/x-www-form-urlencoded"))) {
        amount = _parse_urlencoded();
    }
    /* for multipart/form-data */
    else if (!strncmp(content_type, "multipart/form-data", strlen("multipart/form-data"))) {
        amount = _parse_multipart_data();
    } else {  /* For Oracle Web Server */
        amount = _parse_urlencoded();
    }

    return amount;
}

/* For decode application/x-www-form-urlencoded, used by qDecoder() */
static int _parse_urlencoded(void) {
    Q_Entry *entries, *back;
    char    *query;
    int     amount, cnt;
    int     loop;

    entries = back = NULL;

    for (amount = 0, loop = 1; loop <= 3; loop++) {
        char sepchar;
        if (loop == 1) {
            query   = _get_query("COOKIE");
            sepchar = ';';
        } else if (loop == 2) {
            query   = _get_query("GET");
            sepchar = '&';
        } else if (loop == 3) {
            query   = _get_query("POST");
            sepchar = '&';
        } else break;
        for (cnt = 0; query && *query; amount++, cnt++) {
            back    = entries;
            entries = (Q_Entry *)malloc(sizeof(Q_Entry));
            if (back != NULL) back->next = entries;
            if (_first_entry == NULL) _first_entry = entries;

            entries->value = _makeword(query, sepchar);
            entries->name  = qRemoveSpace(_makeword(entries->value, '='));
            entries->next  = NULL;

            qURLdecode(entries->name);
            qURLdecode(entries->value);
        }
        if (query) free(query);

        if (loop == 1) _cookie_cnt = cnt;
        else if (loop == 2) _get_cnt = cnt;
        else if (loop == 3) _post_cnt = cnt;
    }

    return amount;
}

/* For fetch query used by _parse_urlencoded() */
static char *_get_query(char *method) {
    char *query;
    int  cl, i;

    if (!strcmp(method, "GET")) {
        if (getenv("QUERY_STRING") == NULL) return NULL;
        query = strdup(getenv("QUERY_STRING"));
        /* SSI query handling */
        if (!strcmp(query, "") && (getenv("REQUEST_URI") != NULL)) {
            char *cp;
            for (cp = getenv("REQUEST_URI"); *cp != '\0'; cp++) {
                if (*cp == '?') {
                    cp++;
                    break;
                }
            }
            free(query);
            query = strdup(cp);
        }

        return query;
    }
    if (!strcmp(method, "POST")) {
        if (getenv("REQUEST_METHOD") == NULL) return NULL;
        if (strcmp("POST", getenv("REQUEST_METHOD"))) return NULL;
        if (getenv("CONTENT_LENGTH") == NULL) qError("_get_query(): Your browser sent a non-HTTP compliant message.");

        cl    = atoi(getenv("CONTENT_LENGTH"));
        query = (char *)malloc(sizeof(char) * (cl + 1));
        for (i = 0; i < cl; i++) query[i] = CONCEPT_AWARE_fgetc(stdin);
        query[i] = '\0';
        return query;
    }
    if (!strcmp(method, "COOKIE")) {
        if (getenv("HTTP_COOKIE") == NULL) return NULL;
        query = strdup(getenv("HTTP_COOKIE"));
        return query;
    }

    return NULL;
}

/* For decode multipart/form-data, used by qDecoder() */
static int _parse_multipart_data(void) {
    Q_Entry *entries, *back;
    char    *query;
    int     amount;
    int     loop;
    int     cnt;

    char *name, *value, *filename, *contenttype;
    int  valuelen;

    char boundary[256], boundaryEOF[256];
    char rnboundaryrn[256], rnboundaryEOF[256];
    int  boundarylen, boundaryEOFlen, maxboundarylen;

    char buf[1024];
    int  c, c_count;

    int finish;

#ifdef _WIN32
    setmode(fileno(stdin), _O_BINARY);
    setmode(fileno(stdout), _O_BINARY);
#endif

    entries = back = NULL;

    /* For parse GET method & Cookie */
    for (amount = 0, loop = 1; loop <= 2; loop++) {
        char sepchar;

        if (loop == 1) {
            query   = _get_query("COOKIE");
            sepchar = ';';
        } else if (loop == 2) {
            query   = _get_query("GET");
            sepchar = '&';
        } else break;

        for (cnt = 0; query && *query; amount++, cnt++) {
            back    = entries;
            entries = (Q_Entry *)malloc(sizeof(Q_Entry));
            if (back != NULL) back->next = entries;
            if (_first_entry == NULL) _first_entry = entries;

            entries->value = _makeword(query, sepchar);
            entries->name  = qRemoveSpace(_makeword(entries->value, '='));
            entries->next  = NULL;

            qURLdecode(entries->name);
            qURLdecode(entries->value);
        }
        if (query) free(query);

        if (loop == 1) _cookie_cnt = cnt;
        else if (loop == 2) _get_cnt = cnt;
    }

    /* For parse multipart/form-data method */

    /* Force to check the boundary string length to defense overflow attack */
    maxboundarylen  = strlen("--");
    maxboundarylen += strlen(strstr(getenv("CONTENT_TYPE"), "boundary=") + strlen("boundary="));
    maxboundarylen += strlen("--");
    maxboundarylen += strlen("\r\n");
    if (maxboundarylen >= sizeof(boundary)) qError("_parse_multipart_data(): The boundary string is too long. Stopping process.");

    /* find boundary string */
    sprintf(boundary, "--%s", strstr(getenv("CONTENT_TYPE"), "boundary=") + strlen("boundary="));
    /* This is not necessary but, I can not trust MS Explore */
    qRemoveSpace(boundary);

    sprintf(boundaryEOF, "%s--", boundary);

    sprintf(rnboundaryrn, "\r\n%s\r\n", boundary);
    sprintf(rnboundaryEOF, "\r\n%s", boundaryEOF);

    boundarylen    = strlen(boundary);
    boundaryEOFlen = strlen(boundaryEOF);

    /* If you want to observe the string from stdin, enable this section. */
    /* This section is made for debugging.                                */
    if (0) {
        int i, j;
        qContentType("text/html");

        printf("Content Length = %s<br>\n", getenv("CONTENT_LENGTH"));
        printf("Boundary len %d : %s<br>\n", (int)strlen(boundary), boundary);
        for (i = 0; boundary[i] != '\0'; i++) printf("%02X ", boundary[i]);
        printf("<p>\n");

        for (j = 1; CONCEPT_AWARE_fgets(buf, sizeof(buf), stdin) != NULL; j++) {
            printf("Line %d, len %d : %s<br>\n", j, (int)strlen(buf), buf);
            for (i = 0; buf[i] != '\0'; i++) printf("%02X ", buf[i]);
            printf("<p>\n");
        }
        exit(0);
    }

    /* check boundary */
    if (CONCEPT_AWARE_fgets(buf, sizeof(buf), stdin) == NULL) qError("_parse_multipart_data(): Your browser sent a non-HTTP compliant message.");

    /* for explore 4.0 of NT, it sent \r\n before starting, fucking Micro$oft */
    if (!strcmp(buf, "\r\n")) CONCEPT_AWARE_fgets(buf, sizeof(buf), stdin);

    if (strncmp(buf, boundary, boundarylen) != 0) qError("_parse_multipart_data(): String format invalid.");

    for (finish = 0, cnt = 0; finish != 1; amount++, cnt++) {
        /* parse header */
        name        = "";
        filename    = "";
        contenttype = "";

        while (CONCEPT_AWARE_fgets(buf, sizeof(buf), stdin)) {
            if (!strcmp(buf, "\r\n")) break;
            else if (!qStrincmp(buf, "Content-Disposition: ", strlen("Content-Disposition: "))) {
                /* get name field */
                name = strdup(buf + strlen("Content-Disposition: form-data; name=\""));
                for (c_count = 0; (name[c_count] != '\"') && (name[c_count] != '\0'); c_count++);
                name[c_count] = '\0';

                /* get filename field */
                if (strstr(buf, "; filename=\"") != NULL) {
                    int erase;
                    filename = strdup(strstr(buf, "; filename=\"") + strlen("; filename=\""));
                    for (c_count = 0; (filename[c_count] != '\"') && (filename[c_count] != '\0'); c_count++);
                    filename[c_count] = '\0';
                    /* erase '\' */
                    for (erase = 0, c_count = strlen(filename) - 1; c_count >= 0; c_count--) {
                        if (erase == 1) filename[c_count] = ' ';
                        else {
                            if (filename[c_count] == '\\') {
                                erase             = 1;
                                filename[c_count] = ' ';
                            }
                        }
                    }
                    qRemoveSpace(filename);
                }
            } else if (!qStrincmp(buf, "Content-Type: ", strlen("Content-Type: "))) {
                contenttype = strdup(buf + strlen("Content-Type: "));
                qRemoveSpace(contenttype);
            }
        }

        /* get value field */
        for (value = NULL, valuelen = (1024 * 16), c_count = 0; (c = CONCEPT_AWARE_fgetc(stdin)) != EOF; ) {
            if (c_count == 0) {
                value = (char *)malloc(sizeof(char) * valuelen);
                if (value == NULL) qError("_parse_multipart_data(): Memory allocation fail.");
            } else if (c_count == valuelen - 1) {
                char *valuetmp;

                valuelen *= 2;

                /* Here, we do not use realloc(). Because sometimes it is unstable. */
                valuetmp = (char *)malloc(sizeof(char) * valuelen);
                if (valuetmp == NULL) qError("_parse_multipart_data(): Memory allocation fail.");
                memcpy(valuetmp, value, c_count);
                free(value);
                value = valuetmp;
            }
            value[c_count++] = (char)c;

            /* check end */
            if ((c == '\n') || (c == '-')) {
                value[c_count] = '\0';

                if ((c_count - (2 + boundarylen + 2)) >= 0) {
                    if (!strcmp(value + (c_count - (2 + boundarylen + 2)), rnboundaryrn)) {
                        value[c_count - (2 + boundarylen + 2)] = '\0';
                        valuelen = c_count - (2 + boundarylen + 2);
                        break;
                    }
                }
                if ((c_count - (2 + boundaryEOFlen)) >= 0) {
                    if (!strcmp(value + (c_count - (2 + boundaryEOFlen)), rnboundaryEOF)) {
                        value[c_count - (2 + boundaryEOFlen)] = '\0';
                        valuelen = c_count - (2 + boundaryEOFlen);
                        finish   = 1;
                        break;
                    }
                }

                /* For Micro$oft Explore on MAC, they do not follow rules */
                if ((c_count - (boundarylen + 2)) == 0) {
                    char boundaryrn[256];
                    sprintf(boundaryrn, "%s\r\n", boundary);
                    if (!strcmp(value, boundaryrn)) {
                        value[0] = '\0';
                        valuelen = 0;
                        break;
                    }
                }
                if ((c_count - boundaryEOFlen) == 0) {
                    if (!strcmp(value, boundaryEOF)) {
                        value[0] = '\0';
                        valuelen = 0;
                        finish   = 1;
                        break;
                    }
                }
            }
        }

        if (c == EOF) qError("_parse_multipart_data(): Broken stream at end of '%s'.", name);

        /* store in linked list */
        /* store data */
        back    = entries;
        entries = (Q_Entry *)malloc(sizeof(Q_Entry));
        if (back != NULL) back->next = entries;
        if (_first_entry == NULL) _first_entry = entries;

        entries->name  = name;
        entries->value = value;
        entries->next  = NULL;

        if (strcmp(filename, "") != 0) {
            /* store data length, 'NAME.length'*/
            back       = entries;
            entries    = (Q_Entry *)malloc(sizeof(Q_Entry));
            back->next = entries;

            entries->name  = (char *)malloc(sizeof(char) * (strlen(name) + strlen(".length") + 1));
            entries->value = (char *)malloc(sizeof(char) * 20 + 1);
            sprintf(entries->name, "%s.length", name);
            sprintf(entries->value, "%d", valuelen);

            /* store filename, 'NAME.filename'*/
            back       = entries;
            entries    = (Q_Entry *)malloc(sizeof(Q_Entry));
            back->next = entries;

            entries->name  = (char *)malloc(sizeof(char) * (strlen(name) + strlen(".filename") + 1));
            entries->value = filename;
            sprintf(entries->name, "%s.filename", name);
            entries->next = NULL;

            /* store contenttype, 'NAME.contenttype'*/
            back       = entries;
            entries    = (Q_Entry *)malloc(sizeof(Q_Entry));
            back->next = entries;

            entries->name  = (char *)malloc(sizeof(char) * (strlen(name) + strlen(".contenttype") + 1));
            entries->value = contenttype;
            sprintf(entries->name, "%s.contenttype", name);
            entries->next = NULL;

            cnt += 3;
        }
    }

    _post_cnt = cnt;

    return amount;
}

/**********************************************
** Usage : qValue(query name);
** Return: Success pointer of value string, Fail NULL.
** Do    : Find value string pointer.
**         It find value in linked list.
**********************************************/
char *qValue(char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qValue(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();
    return _EntryValue(_first_entry, name);
}

/**********************************************
** Usage : qiValue(query name);
** Return: Success integer of value string, Fail 0.
** Do    : Find value string pointer and convert to integer.
**********************************************/
int qiValue(char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qiValue(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();
    return _EntryiValue(_first_entry, name);
}

/**********************************************
** Usage : qValueDefault(default string, query name);
** Return: Success pointer of value string, Fail using default string.
** Do    : If the query is not found, default string is used instead.
**********************************************/
char *qValueDefault(char *defstr, char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;
    char    *value;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qValueDefault(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();
    if ((value = _EntryValue(_first_entry, name)) == NULL) value = defstr;

    return value;
}

/**********************************************
** Usage : qValueNotEmpty(error message, query name);
** Return: Success pointer of value string, Fail error message.
** Do    : Find value string pointer which is not empty and NULL.
**         When the query is not found or the value string is
**         empty, error message will be shown using qError().
**********************************************/
char *qValueNotEmpty(char *errmsg, char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;
    char    *value;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qValueNotEmpty(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();
    if ((value = _EntryValue(_first_entry, name)) == NULL) qError("%s", errmsg);
    if (!strcmp(value, "")) qError("%s", errmsg);

    return value;
}

/**********************************************
** Usage : qValueReplace(mode, query name, token string, word);
** Return: String pointer which is new or replaced.
** Do    : Replace string or tokens as word from linked list
**         with given mode.
**
** Refer the description of qStrReplace() for more detail.
**********************************************/
char *qValueReplace(char *mode, char *name, char *tokstr, char *word) {
    Q_Entry *entries;
    char    *retstr, *repstr, method, memuse, newmode[2 + 1];

    /* initialize pointers to avoid compile warnings */
    retstr = repstr = NULL;

    if (_first_entry == NULL) qDecoder();

    if (strlen(mode) != 2) qError("qValueReplace(): Unknown mode \"%s\".", mode);
    method     = mode[0], memuse = mode[1];
    newmode[0] = method, newmode[1] = 'n', newmode[2] = '\0';

    if ((method != 't') && (method != 's')) qError("qValueReplace(): Unknown mode \"%s\".", mode);
    if (memuse == 'n') { /* new */
        if ((repstr = _EntryValue(_first_entry, name)) != NULL) {
            retstr = qStrReplace(newmode, repstr, tokstr, word);
        } else retstr = NULL;
    } else if (memuse == 'r') {   /* replace */
        /* To support multiful queries, it searches whole list and convert all of
           matched ones due to the possibility of duplicated query name.
           So when you need to do this replacement for duplicated query name,
           you can call this once before qValueFirst(). */
        for (retstr = NULL, entries = _first_entry; entries; entries = entries->next) {
            if (!strcmp(name, entries->name)) {
                repstr = qStrReplace(newmode, entries->value, tokstr, word);
                free(entries->value);
                entries->value = repstr;
                if (retstr == NULL) retstr = repstr; /* To catch first matched one */
            }
        }
    } else qError("qValueReplace(): Unknown mode \"%s\".", mode);

    /* Return the value of first matched one */
    return retstr;
}

/**********************************************
** Usage : qValueFirst(query name);
** Return: Success pointer of first value string, Fail NULL.
** Do    : Find first value string pointer.
**********************************************/
char *qValueFirst(char *format, ...) {
    int     status;
    va_list arglist;

    va_start(arglist, format);
    status = vsprintf(_multi_last_key, format, arglist);
    if ((strlen(_multi_last_key) + 1 > sizeof(_multi_last_key)) || (status == EOF)) qError("qValueFirst(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();
    _multi_last_entry = _first_entry;

    return qValueNext();
}

/**********************************************
** Usage : qValueNext();
** Return: Success pointer of next value string, Fail NULL.
** Do    : Find next value string pointer.
**********************************************/
char *qValueNext(void) {
    Q_Entry *entries;

    for (entries = _multi_last_entry; entries; entries = entries->next) {
        if (!strcmp(_multi_last_key, entries->name)) {
            _multi_last_entry = entries->next;
            return entries->value;
        }
    }
    _multi_last_entry = NULL;
    strcpy(_multi_last_key, "");

    return NULL;
}

/**********************************************
** Usage : qValueAdd(name, value);
** Do    : Force to add given name and value to linked list.
**         If same name exists, it'll be replaced.
**
** ex) qValueAdd("NAME", "Seung-young Kim");
**********************************************/
char *qValueAdd(char *name, char *format, ...) {
    Q_Entry *new_entry;
    char    value[1024];
    int     status;
    va_list arglist;

    if (!strcmp(name, "")) qError("qValueAdd(): can not add empty name.");

    va_start(arglist, format);
    status = vsprintf(value, format, arglist);
    if ((strlen(value) + 1 > sizeof(value)) || (status == EOF)) qError("qValueAdd(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();

    if (qValue(name) == NULL) _new_cnt++; // if it's new entry, count up.
    new_entry = _EntryAdd(_first_entry, name, value);
    if (!_first_entry) _first_entry = new_entry;

    return qValue(name);
}

/**********************************************
** Usage : qValueRemove(name);
** Do    : Remove entry from linked list.
**
** ex) qValueRemove("NAME");
**********************************************/
void qValueRemove(char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qSessionRemove(): Message is too long or invalid.");
    va_end(arglist);

    if (!strcmp(name, "")) qError("qValueRemove(): can not remove empty name.");

    switch (qValueType(name)) {
        case 'C':
            {
                _cookie_cnt--;
                break;
            }

        case 'G':
            {
                _get_cnt--;
                break;
            }

        case 'P':
            {
                _post_cnt--;
                break;
            }

        case 'N':
            {
                _new_cnt--;
                break;
            }
    }

    _first_entry = _EntryRemove(_first_entry, name);
}

/**********************************************
** Usage : qValueType(name);
** Return: Cookie 'C', Get method 'G', Post method 'P', New data 'N', Not found '-'
** Do    : Returns type of query.
**
** ex) qValueRemove("NAME");
**********************************************/
char qValueType(char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;
    int     v_no;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qValue(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();

    v_no = _EntryNo(_first_entry, name);
    if ((1 <= v_no) && (v_no <= _cookie_cnt)) return 'C';
    else if ((_cookie_cnt + 1 <= v_no) && (v_no <= _cookie_cnt + _get_cnt)) return 'G';
    else if ((_cookie_cnt + _get_cnt + 1 <= v_no) && (v_no <= _cookie_cnt + _get_cnt + _post_cnt)) return 'P';
    else if ((_cookie_cnt + _get_cnt + _post_cnt <= v_no)) return 'N';

    return '-';
}

/**********************************************
** Usage : qGetFirstEntry();
** Do    : Return _first_entry.
**********************************************/
Q_Entry *qGetFirstEntry(void) {
    if (_first_entry == NULL) qDecoder();
    return _first_entry;
}

/**********************************************
** Usage : qPrint(pointer of the first Entry);
** Return: Amount of entries.
** Do    : Print all parsed values & names for debugging.
**********************************************/
int qPrint(void) {
    int amount;

    if (_first_entry == NULL) qDecoder();
    amount = _EntryPrint(_first_entry);
    printf("<hr>\n");
    printf("COOKIE = %d , GET = %d , POST = %d, NEW = %d\n", _cookie_cnt, _get_cnt, _post_cnt, _new_cnt);
    return amount;
}

/**********************************************
** Usage : qFree(pointer of the first Entry);
** Do    : Make free of linked list memory.
**********************************************/
void qFree(void) {
    _EntryFree(_first_entry);
    _first_entry      = NULL;
    _multi_last_entry = NULL;
    strcpy(_multi_last_key, "");
    _cookie_cnt = 0, _get_cnt = 0, _post_cnt = 0, _new_cnt = 0;
}

/**********************************************
** Usage : qCookieSet(name, value, expire days, path, domain, secure);
** Do    : Set cookie.
**
** The 'exp_days' is number of days which expire the cookie.
** The current time + exp_days will be set.
** This function should be called before qContentType().
**
** ex) qCookieSet("NAME", "Kim", 30, NULL, NULL, NULL);
**********************************************/
void qCookieSet(char *name, char *value, int exp_days, char *path, char *domain, char *secure) {
    char *Name, *Value;
    char cookie[(4 * 1024) + 256];

    /* check content flag */
    if (qGetContentFlag() == 1) qError("qCookieSet(): must be called before qContentType() and any stream out.");

    /* Name=Value */
    Name = qURLencode(name), Value = qURLencode(value);
    sprintf(cookie, "%s=%s", Name, Value);
    free(Name), free(Value);

    if (exp_days != 0) {
        time_t plus_sec;
        char   gmt[256];
        plus_sec = (time_t)(exp_days * 24 * 60 * 60);
        qGetGMTime(gmt, plus_sec);
        strcat(cookie, "; expires=");
        strcat(cookie, gmt);
    }

    if (path != NULL) {
        if (path[0] != '/') qError("qCookieSet(): Path string(%s) must start with '/' character.", path);
        strcat(cookie, "; path=");
        strcat(cookie, path);
    }

    if (domain != NULL) {
        if ((strstr(domain, "/") != NULL) || (strstr(domain, ".") == NULL)) qError("qCookieSet(): Invalid domain name(%s).", domain);
        strcat(cookie, "; domain=");
        strcat(cookie, domain);
    }

    if (secure != NULL) {
        strcat(cookie, "; secure");
    }

    //printf("Set-Cookie: %s\n", cookie);
    CONCEPT_AWARE_header("Set-Cookie", cookie);

    /* if you want to use cookie variable immediately, uncommnet below */

    /*
       qValueAdd(name, value);
     */
}

/**********************************************
** Usage : qCookieRemove(name);
** Do    : Remove cookie.
**
** ex) qCookieRemove("NAME");
**********************************************/
void qCookieRemove(char *name, char *path, char *domain, char *secure) {
    /* check content flag */
    if (qGetContentFlag() == 1) qError("qCookieRemove(): must be called before qContentType() and any stream out.");

    qCookieSet(name, "", -1, path, domain, secure);

    /* if you want to remove cookie variable immediately, uncomment below */

    /*
       qValueRemove(name);
     */
}

/**********************************************
** Usage : qCookieValue(cookie name);
** Return: Success pointer of value string, Fail NULL.
** Do    : It only finds cookie value in the linked-list.
**********************************************/
char *qCookieValue(char *format, ...) {
    char    name[1024];
    int     status;
    va_list arglist;

    va_start(arglist, format);
    status = vsprintf(name, format, arglist);
    if ((strlen(name) + 1 > sizeof(name)) || (status == EOF)) qError("qValue(): Message is too long or invalid.");
    va_end(arglist);

    if (_first_entry == NULL) qDecoder();

    if (qValueType(name) == 'C') {
        return _EntryValue(_first_entry, name);
    }

    return NULL;
}
