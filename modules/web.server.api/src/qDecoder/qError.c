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
** Static Values Definition used only internal
**********************************************/

static char *_error_contact_info = NULL;
static char *_error_log_filename = NULL;


/**********************************************
** Usage : qError(format, arg);
** Do    : Print error message.
**********************************************/
void qError(char *format, ...) {
    static int cnt = 0;
    char       buf[1024];
    int        status;
    int        logstatus;
    va_list    arglist;

    if (cnt != 0) return;
    cnt++;

    va_start(arglist, format);

    status = vsprintf(buf, format, arglist);
    if ((strlen(buf) + 1 > sizeof(buf)) || (status == EOF)) {
        CONCEPT_AWARE_print("qError(): Message is too long or invalid.");
        //exit(1);
        return;
    }

    logstatus = 0;
    if (_error_log_filename != NULL) {
        FILE *fp;

        if ((fp = fopen(_error_log_filename, "at")) == NULL) logstatus = -1;
        else {
            char      *http_user_agent, *remote_host;
            struct tm *time;

            time = qGetTime();

            if ((http_user_agent = getenv("HTTP_USER_AGENT")) == NULL) http_user_agent = "null";
            if ((remote_host = getenv("REMOTE_HOST")) == NULL) {
                /* Fetch for Apache 1.3 */
                if ((remote_host = getenv("REMOTE_ADDR")) == NULL) remote_host = "null";
            }

            fprintf(fp, "%04d/%02d/%02d(%02d:%02d) : '%s' from %s (%s)\n",
                    time->tm_year, time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min,
                    buf, remote_host, http_user_agent);

            fclose(fp);
            logstatus = 1;
        }
    }

    if (getenv("REMOTE_ADDR") == NULL) {
        //printf("Error: %s\n", buf);
        CONCEPT_AWARE_print("Error: ");
        CONCEPT_AWARE_print(buf);
    } else {
        //qContentType("text/html");

        if (_error_contact_info != NULL) {
            strcat(buf, _error_contact_info);
        }
        if (logstatus == -1) strcat(buf, " [ERROR LOGGING FAIL]");

        CONCEPT_AWARE_print("<html>\n");
        CONCEPT_AWARE_print("<head>\n");
        CONCEPT_AWARE_print("<title>Error: ");
        CONCEPT_AWARE_print(buf);
        CONCEPT_AWARE_print("</title>\n");
        CONCEPT_AWARE_print("<script language='JavaScript'>\n");
        CONCEPT_AWARE_print("  alert(\"");
        CONCEPT_AWARE_print(buf);
        CONCEPT_AWARE_print("\");\n");
        CONCEPT_AWARE_print("  history.back();\n");
        CONCEPT_AWARE_print("</script>\n");
        CONCEPT_AWARE_print("</head>\n");
        CONCEPT_AWARE_print("</html>\n");
    }

    //qFreeAll();

    //exit(1);
}

/**********************************************
** Usage : qErrorLog(log filename);
** Do    : Turn Error log on.
**********************************************/
void qErrorLog(char *filename) {
    _error_log_filename = filename;
}

/**********************************************
** Usage : qErrorContact(message);
** Do    : Error contact information.
**********************************************/
void qErrorContact(char *msg) {
    _error_contact_info = msg;
}
