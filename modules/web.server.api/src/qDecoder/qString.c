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
** Usage : qPrintf(mode, format, arg);
** Return: Sucess number of output bytes, Fail EOF.
** Do    : Print message like printf.
**         Mode : see qPuts()
**********************************************/
int qPrintf(int mode, char *format, ...) {
    char    buf[1024 * 10];
    int     status;
    va_list arglist;

    va_start(arglist, format);
    if ((status = vsprintf(buf, format, arglist)) == EOF) return status;
    if (strlen(buf) + 1 > sizeof(buf)) qError("qPrintf(): Message is too long or invalid.");

    qPuts(mode, buf);

    return status;
}

/**********************************************
** Usage : qPuts(mode, string pointer);
** Do    : print HTML link as multi mode.
**
**         Mode 00 : Same as printf()
**         Mode 10  :Mode 0 + Convert
**
**         Mode 01 : Print HTML TAG
**         Mode 11 : Print HTML TAG + Convert
**
**         Mode 02 : Print HTML TAG + Auto Link
**         Mode 12 : Print HTML TAG + Auto Link + Convert
**
**         Mode 03 : Print HTML TAG + Auto Link(_top)
**         Mode 13 : Print HTML TAG + Auto Link(_top) + Convert
**         Mode 23 : Print HTML TAG + Auto Link(new window)
**         Mode 33 : Print HTML TAG + Auto Link(new window) + Convert
**
**         Mode 04 : Waste HTML TAG
**         Mode 14 : Waste HTML TAG + Convert
**
**         Mode 05 : Waste HTML TAG + Auto Link
**         Mode 15 : Waste HTML TAG + Auto Link + Convert
**
**         Mode 06 : Waste HTML TAG + Auto Link(_top)
**         Mode 16 : Waste HTML TAG + Auto Link(_top) + Convert
**         Mode 26 : Waste HTML TAG + Auto Link(new window)
**         Mode 36 : Waste HTML TAG + Auto Link(new window) + Convert
**
**         Convert : " "   -> " "
**                   "  "  -> " &nbsp;"
**                   "   " -> " &nbsp;&nbsp;"
**                   "\n"   -> "<br>\n"
**                   "\r\n" -> "<br>\n"
**
** You can use 1x mode, to wrap long lines with no <pre> tag.
** Note) It modify argument string.
**********************************************/
void qPuts(int mode, char *buf) {
    if (buf == NULL) return;

    if (mode == 0) printf("%s", buf);
    else if (mode == 10) {
        int i;
        for (i = 0; buf[i] != '\0'; i++) {
            switch (buf[i]) {
                case ' ':
                    {
                        if ((i > 0) && (buf[i - 1] == ' ')) printf("&nbsp;");
                        else printf(" ");
                        break;
                    }

                case '\r':
                    {
                        break;
                    }

                case '\n':
                    {
                        printf("<br>\n");
                        break;
                    }

                default:
                    {
                        printf("%c", buf[i]);
                        break;
                    }
            }
        }
    } else {
        char *ptr, retstop, lastretstop, *target, *deftarget, *token;
        int  printhtml, autolink, convert, linkflag, ignoreflag;

        /* set defaults, mode 2*/
        printhtml = 1;
        autolink  = 1;
        target    = "_top";
        deftarget = "qnewwin";
        convert   = 0;

        switch (mode) {
            case 01:
                {
                    printhtml = 1, autolink = 0, target = "";
                    convert   = 0;
                    break;
                }

            case 11:
                {
                    printhtml = 1, autolink = 0, target = "";
                    convert   = 1;
                    break;
                }

            case 02:
                {
                    printhtml = 1, autolink = 1, target = "";
                    convert   = 0;
                    break;
                }

            case 12:
                {
                    printhtml = 1, autolink = 1, target = "";
                    convert   = 1;
                    break;
                }

            case 03:
                {
                    printhtml = 1, autolink = 1, target = "_top";
                    convert   = 0;
                    break;
                }

            case 13:
                {
                    printhtml = 1, autolink = 1, target = "_top";
                    convert   = 1;
                    break;
                }

            case 23:
                {
                    printhtml = 1, autolink = 1, target = deftarget;
                    convert   = 0;
                    break;
                }

            case 33:
                {
                    printhtml = 1, autolink = 1, target = deftarget;
                    convert   = 1;
                    break;
                }

            case 04:
                {
                    printhtml = 0, autolink = 0, target = "";
                    convert   = 0;
                    break;
                }

            case 14:
                {
                    printhtml = 0, autolink = 0, target = "";
                    convert   = 1;
                    break;
                }

            case 05:
                {
                    printhtml = 0, autolink = 1, target = "";
                    convert   = 0;
                    break;
                }

            case 15:
                {
                    printhtml = 0, autolink = 1, target = "";
                    convert   = 1;
                    break;
                }

            case 06:
                {
                    printhtml = 0, autolink = 1, target = "_top";
                    convert   = 0;
                    break;
                }

            case 16:
                {
                    printhtml = 0, autolink = 1, target = "_top";
                    convert   = 1;
                    break;
                }

            case 26:
                {
                    printhtml = 0, autolink = 1, target = deftarget;
                    convert   = 0;
                    break;
                }

            case 36:
                {
                    printhtml = 0, autolink = 1, target = deftarget;
                    convert   = 1;
                    break;
                }

            default:
                {
                    qError("_autolink(): Invalid Mode (%d).", mode);
                    break;
                }
        }

        token       = " `(){}[]<>\"',\r\n";
        lastretstop = '0'; /* any character except space */
        ptr         = _strtok2(buf, token, &retstop);

        for (linkflag = ignoreflag = 0; ptr != NULL; ) {
            /* auto link */
            if (ignoreflag == 0) {
                if (autolink == 1) {
                    if (!strncmp(ptr, "http://", 7)) linkflag = 1;
                    else if (!strncmp(ptr, "https://", 8)) linkflag = 1;
                    else if (!strncmp(ptr, "ftp://", 6)) linkflag = 1;
                    else if (!strncmp(ptr, "telnet://", 9)) linkflag = 1;
                    else if (!strncmp(ptr, "news:", 5)) linkflag = 1;
                    else if (!strncmp(ptr, "mailto:", 7)) linkflag = 1;
                    else if (qCheckEmail(ptr) == 1) linkflag = 2;
                    else linkflag = 0;
                }
                if (linkflag == 1) printf("<a href=\"%s\" target=\"%s\">%s</a>", ptr, target, ptr);
                else if (linkflag == 2) printf("<a href=\"mailto:%s\">%s</a>", ptr, ptr);
                else printf("%s", ptr);
            }

            /* print */
            if (printhtml == 1) {
                if (retstop == '<') printf("&lt;");
                else if (retstop == '>') printf("&gt;");
                else if (retstop == '\"') printf("&quot;");
                else if (retstop == '&') printf("&amp;");

                else if ((retstop == ' ') && (convert == 1)) {
                    if ((lastretstop == ' ') && (strlen(ptr) == 0)) printf("&nbsp;");
                    else printf(" ");
                } else if ((retstop == '\r') && (convert == 1));  /* skip when convert == 1 */
                else if ((retstop == '\n') && (convert == 1)) printf("<br>\n");

                else if (retstop != '\0') printf("%c", retstop);
            } else {
                if (retstop == '<') ignoreflag = 1;
                else if (retstop == '>') ignoreflag = 0;

                else if ((retstop == '\"') && (ignoreflag == 0)) printf("&quot;");
                else if ((retstop == '&') && (ignoreflag == 0)) printf("&amp;");

                else if ((retstop == ' ') && (ignoreflag == 0) && (convert == 1)) {
                    if ((lastretstop == ' ') && (strlen(ptr) == 0)) printf("&nbsp;");
                    else printf(" ");
                } else if ((retstop == '\r') && (ignoreflag == 0) && (convert == 1));  /* skip when convert == 1 */
                else if ((retstop == '\n') && (ignoreflag == 0) && (convert == 1)) printf("<br>\n");

                else if ((retstop != '\0') && (ignoreflag == 0)) printf("%c", retstop);
            }

            lastretstop = retstop;
            ptr         = _strtok2(NULL, token, &retstop);
        }
    }
}

/**********************************************
** Usage : qRemoveSpace(string);
** Return: Pointer of string.
** Do    : Remove space(including CR, LF) from head & tail.
**********************************************/
char *qRemoveSpace(char *str) {
    int i, j;

    if (!str) return NULL;

    for (j = 0; str[j] == ' ' || str[j] == 9 || str[j] == '\r' || str[j] == '\n'; j++);
    for (i = 0; str[j] != '\0'; i++, j++) str[i] = str[j];
    for (i--; (i >= 0) && (str[i] == ' ' || str[i] == 9 || str[i] == '\r' || str[i] == '\n'); i--);
    str[i + 1] = '\0';

    return str;
}

/**********************************************
** Usage : qStr09AZaz(string);
** Return: Valid 1, Invalid 0.
** Do    : Check characters of string is in 0-9, A-Z, a-z.
**********************************************/
int qStr09AZaz(char *str) {
    for ( ; *str; str++) {
        if ((*str >= '0') && (*str <= '9')) continue;
        else if ((*str >= 'A') && (*str <= 'Z')) continue;
        else if ((*str >= 'a') && (*str <= 'z')) continue;
        else return 0;
    }
    return 1;
}

/**********************************************
** Usage : qStrupr(string);
** Return: Pointer of converted string.
** Do    : Convert small character to big character.
**********************************************/
char *qStrupr(char *str) {
    char *cp;

    if (!str) return NULL;
    for (cp = str; *cp; cp++) if ((*cp >= 'a') && (*cp <= 'z')) *cp -= 32;
    return str;
}

/**********************************************
** Usage : qStrlwr(string);
** Return: Pointer of converted string.
** Do    : Convert big character to small character.
**********************************************/
char *qStrlwr(char *str) {
    char *cp;

    if (!str) return NULL;
    for (cp = str; *cp; cp++) if ((*cp >= 'A') && (*cp <= 'Z')) *cp += 32;
    return str;
}

/**********************************************
** Usage : qStristr(big, small);
** Return: Pointer of token string located in original string, Fail NULL.
** Do    : Find token with no case-censitive.
**********************************************/
char *qStristr(char *big, char *small) {
    char *bigp, *smallp, *retp;

    if ((big == NULL) || (small == NULL)) return 0;

    if ((bigp = strdup(big)) == NULL) return 0;
    if ((smallp = strdup(small)) == NULL) {
        free(bigp);
        return 0;
    }

    qStrupr(bigp), qStrupr(smallp);

    retp = strstr(bigp, smallp);
    if (retp != NULL) retp = big + (retp - bigp);
    free(bigp), free(smallp);

    return retp;
}

/**********************************************
** Usage : qStricmp(char *orgstr, char *tokstr);
** Return: s1 < s2 less than 0, s1 = s2 0, s1 > s2 greater than 0.
** Do    : qDecoder implementation of stricmp() because
**         some systems do not support this function.
**********************************************/
int qStricmp(char *s1, char *s2) {
    char *bs1, *bs2;
    int  result;

    if ((s1 == NULL) || (s2 == NULL)) return 0;
    if ((bs1 = strdup(s1)) == NULL) return 0;
    if ((bs2 = strdup(s2)) == NULL) {
        free(bs1);
        return 0;
    }

    qStrupr(bs1), qStrupr(bs2);
    result = strcmp(bs1, bs2);
    free(bs1), free(bs2);

    return result;
}

/**********************************************
** Usage : qStrnicmp(s1, s2, length);
** Return: Same as strncmp().
** Do    : Compare n-byte strings with no case-censitive.
**********************************************/
int qStrincmp(char *s1, char *s2, size_t len) {
    char *s1p, *s2p;
    int  result;

    if ((s1 == NULL) || (s2 == NULL)) return 0;

    if ((s1p = strdup(s1)) == NULL) return 0;
    if ((s2p = strdup(s2)) == NULL) {
        free(s1p);
        return 0;
    }

    qStrupr(s1p), qStrupr(s2p);

    result = strncmp(s1p, s2p, len);
    free(s1p), free(s2p);

    return result;
}

/**********************************************
** Usage : qitocomma(value);
** Return: String pointer.
** Do    : Convert integer to comma string.
**
** ex) printf("Output: %s", qitocomma(1234567));
**     Output: -1,234,567,000
**********************************************/
char *qitocomma(int value) {
    static char str[14 + 1];
    char        buf[10 + 1], *strp = str, *bufp;

    if (value < 0) *strp++ = '-';
    sprintf(buf, "%d", abs(value));
    for (bufp = buf; *bufp != '\0'; strp++, bufp++) {
        *strp = *bufp;
        if (((strlen(bufp)) % 3 == 1) && (*(bufp + 1) != '\0')) *(++strp) = ',';
    }
    *strp = '\0';

    return str;
}

/**********************************************
** Usage : qStrReplace(mode, source string, token string to replace, word);
** Return: String pointer which is new or replaced.
** Do    : Replace string or tokens as word from source string
**         with given mode.
**
** The mode argument has two separated character. First character
** is used to decide replacing method and can be 't' or 's'.
** The character 't' and 's' stand on [t]oken and [s]tring.
**
** When 't' is given each character of the token string(third argument)
** will be compared with source string individually. If matched one
** is found. the character will be replaced with given work.
**
** If 's' is given instead of 't'. Token string will be analyzed
** only one chunk word. So the replacement will be occured when
** the case of whole word matched.
**
** Second character is used to decide returning memory type and
** can be 'n' or 'r' which are stand on [n]ew and [r]eplace.
**
** When 'n' is given the result will be placed into new array so
** you should free the return string after using. Instead of this,
** you can also use 'r' character to modify source string directly.
** In this case, given source string should have enough space. Be
** sure that untouchable value can not be used for source string.
**
** So there are four associatable modes such like below.
**
** Mode "tn" : [t]oken replacing & putting the result into [n]ew array.
** Mode "tr" : [t]oken replacing & [r]eplace source string directly.
** Mode "sn" : [s]tring replacing & putting the result into [n]ew array.
** Mode "sr" : [s]tring replacing & [r]eplace source string directly.
**
** ex) int  i;
**     char srcstr[256], *retstr;
**     char mode[4][2+1] = {"tn", "tr", "sn", "sr"};
**
**     for(i = 0; i < 4; i++) {
**       strcpy(srcstr, "Welcome to the qDecoder project.");
**       printf("before %s : srcstr = %s\n", mode[i], srcstr);
**
**       retstr = qStrReplace(mode[i], srcstr, "the", "_");
**       printf("after  %s : srcstr = %s\n", mode[i], srcstr);
**       printf("            retstr = %s\n\n", retstr);
**       if(mode[i][1] == 'n') free(retstr);
**     }
**     --[Result]--
**     before tn : srcstr = Welcome to the qDecoder project.
**     after  tn : srcstr = Welcome to the qDecoder project.
**                 retstr = W_lcom_ _o ___ qD_cod_r proj_c_.
**
**     before tr : srcstr = Welcome to the qDecoder project.
**     after  tr : srcstr = W_lcom_ _o ___ qD_cod_r proj_c_.
**                 retstr = W_lcom_ _o ___ qD_cod_r proj_c_.
**
**     before sn : srcstr = Welcome to the qDecoder project.
**     after  sn : srcstr = Welcome to the qDecoder project.
**                 retstr = Welcome to _ qDecoder project.
**
**     before sr : srcstr = Welcome to the qDecoder project.
**     after  sr : srcstr = Welcome to _ qDecoder project.
**                 retstr = Welcome to _ qDecoder project.
**********************************************/
char *qStrReplace(char *mode, char *srcstr, char *tokstr, char *word) {
    char method, memuse;
    int  maxstrlen, tokstrlen;
    char *newstr, *newp, *srcp, *tokenp, *retp;

    /* initialize pointers to avoid compile warnings */
    newstr = newp = srcp = tokenp = retp = NULL;

    if (strlen(mode) != 2) qError("qStrReplace(): Unknown mode \"%s\".", mode);
    method = mode[0], memuse = mode[1];

    /* Put replaced string into malloced 'newstr' */
    if (method == 't') { /* Token replace */
        maxstrlen = strlen(srcstr) * strlen(word);
        newstr    = (char *)malloc(maxstrlen + 1);

        for (srcp = srcstr, newp = newstr; *srcp; srcp++) {
            for (tokenp = tokstr; *tokenp; tokenp++) {
                if (*srcp == *tokenp) {
                    char *wordp;
                    for (wordp = word; *wordp; wordp++) *newp++ = *wordp;
                    break;
                }
            }
            if (!*tokenp) *newp++ = *srcp;
        }
        *newp = '\0';
    } else if (method == 's') {   /* String replace */
        if (strlen(word) > strlen(tokstr))
            maxstrlen
                = ((strlen(srcstr) / strlen(tokstr)) * strlen(word))
                  + (strlen(srcstr)
                     % strlen(tokstr));
        else maxstrlen = strlen(srcstr);
        newstr    = (char *)malloc(maxstrlen + 1);
        tokstrlen = strlen(tokstr);

        for (srcp = srcstr, newp = newstr; *srcp; srcp++) {
            if (!strncmp(srcp, tokstr, tokstrlen)) {
                char *wordp;
                for (wordp = word; *wordp; wordp++) *newp++ = *wordp;
                srcp += tokstrlen - 1;
            } else *newp++ = *srcp;
        }
        *newp = '\0';
    } else qError("qStrReplace(): Unknown mode \"%s\".", mode);

    /* Decide whether newing the memory or replacing into exist one */
    if (memuse == 'n') retp = newstr;
    else if (memuse == 'r') {
        strcpy(srcstr, newstr);
        free(newstr);
        retp = srcstr;
    } else qError("qStrReplace(): Unknown mode \"%s\".", mode);

    return retp;
}
