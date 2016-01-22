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
** Linked List(Entry) Routines
**********************************************/


/**********************************************
** Usage : _EntryAdd(first entry, name, value);
** Return: New entry pointer.
** Do    : Add entry at last but if same name exists, replace it.
**********************************************/
Q_Entry *_EntryAdd(Q_Entry *first, char *name, char *value) {
    Q_Entry *entries;

    if (!strcmp(name, "")) return NULL;

    /* check same name */
    for (entries = first; entries; entries = entries->next) {
        if (!strcmp(entries->name, name)) {
            free(entries->value);
            entries->value = strdup(value);
            return entries;
        }
    }

    /* new entry */
    entries        = (Q_Entry *)malloc(sizeof(Q_Entry));
    entries->name  = strdup(name);
    entries->value = strdup(value);
    entries->next  = NULL;

    /* If first is not NULL, find last entry then make a link*/
    if (first) {
        for ( ; first->next; first = first->next);
        first->next = entries;
    }

    return entries;
}

/**********************************************
** Usage : _EntryRemove(first entry, name to remove);
** Return: first entry pointer.
** Do    : Remove entry if same name exists, remove all.
**********************************************/
Q_Entry *_EntryRemove(Q_Entry *first, char *name) {
    Q_Entry *entries, *prev_entry;

    if (!strcmp(name, "")) return first;

    for (prev_entry = NULL, entries = first; entries; ) {
        if (!strcmp(entries->name, name)) { /* found */
            Q_Entry *next;

            next = entries->next;

            /* remove entry itself*/
            free(entries->name);
            free(entries->value);
            free(entries);

            /* remove entry link from linked-list */
            if (prev_entry == NULL) first = next;
            else prev_entry->next = next;
            entries = next;
        } else {  /* next */
            prev_entry = entries;
            entries    = entries->next;
        }
    }

    return first;
}

/**********************************************
** Usage : _EntryValue(pointer of the first entry, name);
** Return: Success pointer of value string, Fail NULL.
** Do    : Find value string pointer.
**         It find value in linked list.
**********************************************/
char *_EntryValue(Q_Entry *first, char *name) {
    Q_Entry *entries;

    for (entries = first; entries; entries = entries->next) {
        if (!strcmp(name, entries->name)) return entries->value;
    }
    return NULL;
}

/**********************************************
** Usage : _EntryiValue(pointer of the first entry, name);
** Return: Success integer of value string, Fail 0.
** Do    : Find value string pointer and convert to integer.
**********************************************/
int _EntryiValue(Q_Entry *first, char *name) {
    char *str;

    str = _EntryValue(first, name);
    if (str == NULL) return 0;
    return atoi(str);
}

/**********************************************
** Usage : _EntryNo(pointer of the first entry, name);
** Return: Success no. Fail 0;
** Do    : Find sequence number of value string pointer.
**********************************************/
int _EntryNo(Q_Entry *first, char *name) {
    Q_Entry *entries;
    int     no;

    for (no = 1, entries = first; entries; no++, entries = entries->next) {
        if (!strcmp(name, entries->name)) return no;
    }
    return 0;
}

/**********************************************
** Usage : _EntryPrint(pointer of the first entry);
** Return: Amount of entries.
** Do    : Print all parsed value & name for debugging.
**********************************************/
int _EntryPrint(Q_Entry *first) {
    Q_Entry *entries;
    int     amount;

    qContentType("text/html");

    for (amount = 0, entries = first; entries; amount++, entries = entries->next) {
        printf("'%s' = '%s'<br>\n", entries->name, entries->value);
    }

    return amount;
}

/**********************************************
** Usage : _EntryFree(pointer of the first entry);
** Do    : Make free of linked list memory.
**********************************************/
void _EntryFree(Q_Entry *first) {
    Q_Entry *entries;

    for ( ; first; first = entries) {
        entries = first->next; /* copy next to tmp */
        free(first->name);
        free(first->value);
        free(first);
    }
}

/**********************************************
** Usage : _EntrySave(pointer of the first entry, filename);
** Return: Success 1, Fail 0.
** Do    : Save entries into file.
**********************************************/
int _EntrySave(Q_Entry *first, char *filename) {
    FILE *fp;
    char gmt[32];

    qGetGMTime(gmt, (time_t)0);
    if ((fp = qfopen(filename, "w")) == NULL) return 0;

    fprintf(fp, "# automatically generated by qDecoder at %s.\n", gmt);
    fprintf(fp, "# %s\n", filename);
    for ( ; first; first = first->next) {
        char *encvalue;

        encvalue = qURLencode(first->value);
        fprintf(fp, "%s=%s\n", first->name, encvalue);
        free(encvalue);
    }

    qfclose(fp);
    return 1;
}

/**********************************************
** Usage : _EntryLoad(filename);
** Return: Success pointer of first entry, Fail NULL.
** Do    : Load entries from given filename.
**********************************************/
Q_Entry *_EntryLoad(char *filename) {
    Q_Entry *first, *entries;

    if ((first = qfDecoder(filename)) == NULL) return NULL;

    for (entries = first; entries; entries = entries->next) {
        qURLdecode(entries->value);
    }

    return first;
}
