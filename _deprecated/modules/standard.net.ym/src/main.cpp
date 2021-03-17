/*
   <libdesc>
   Yahoo! Messenger client library based on the libyahoo library.

   TODO: Documentation. Only example is available for now.
   </libdesc>
 */
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

#include <sys/types.h>

#ifdef _WIN32
 #include <io.h>
#endif

//#include <iostream>

INVOKE_CALL   InvokePtr = 0;
unsigned long recv_id   = 0;

#define INIT_EVENT       \
    int result = 0;      \
    void *RES       = 0; \
    void *EXCEPTION = 0;



#define GET_EVENT_RESULT                                             \
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);                      \
    if (RES) {                                                       \
        NUMBER  ndata   = 0;                                         \
        char    *szdata = 0;                                         \
        INTEGER TYPE    = 0;                                         \
        InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata); \
        switch (TYPE) {                                              \
            case VARIABLE_NUMBER:                                    \
                result = (((int)ndata) != 0);                        \
                break;                                               \
            case VARIABLE_STRING:                                    \
                result = AnsiString(szdata).Length() != 0;           \
                break;                                               \
            case VARIABLE_ARRAY:                                     \
            case VARIABLE_DELEGATE:                                  \
            case VARIABLE_CLASS:                                     \
                result = 1;                                          \
                break;                                               \
        }                                                            \
        InvokePtr(INVOKE_FREE_VARIABLE, RES);                        \
    }


#define GET_EVENT_RESULT2(str)                                       \
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);                      \
    if (RES) {                                                       \
        NUMBER  ndata   = 0;                                         \
        char    *szdata = 0;                                         \
        INTEGER TYPE    = 0;                                         \
        InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata); \
        switch (TYPE) {                                              \
            case VARIABLE_NUMBER:                                    \
                result = (((int)ndata) != 0);                        \
                break;                                               \
            case VARIABLE_STRING:                                    \
                str    = szdata;                                     \
                result = AnsiString(szdata).Length() != 0;           \
                break;                                               \
            case VARIABLE_ARRAY:                                     \
            case VARIABLE_DELEGATE:                                  \
            case VARIABLE_CLASS:                                     \
                result = 1;                                          \
                break;                                               \
        }                                                            \
        InvokePtr(INVOKE_FREE_VARIABLE, RES);                        \
    }


#ifdef _WIN32
 #include <windows.h>
 #include <winsock.h>
 #include <winerror.h>
 #define THREAD_TYPE       DWORD WINAPI

 #define ECONNREFUSED      WSAECONNREFUSED
 #define ETIMEDOUT         WSAETIMEDOUT
 #define ENETUNREACH       WSAENETUNREACH
 #define EINPROGRESS       WSAEINPROGRESS

#else
 #define SOCKET            int
 #define INVALID_SOCKET    -1
 #define SOCKET_ERROR      -1
 #define DWORD             long
 #define LPVOID            void *
 #define WINAPI
 #define THREAD_TYPE       LPVOID
 #define closesocket       close

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/wait.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <dlfcn.h>      // dlopen
 #include <unistd.h>
 #include <pthread.h>

 #include <sys/select.h>
 #include <netinet/tcp.h>
 #include <arpa/inet.h>

 #define HANDLE     void *
 #define HMODULE    HANDLE

 #define Sleep(x)    usleep(x * 1000)
#endif



#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <openssl/ssl.h>
/* Get these from http://libyahoo2.sourceforge.net/ */
extern "C" {
#include "libyahoo2b/yahoo2.h"
#include "libyahoo2b/yahoo2_callbacks.h"
#include "libyahoo2b/yahoo_util.h"
}

static char *local_host = NULL;

//int fileno(FILE * stream);

#ifdef _WIN32
 #define socklen_t      int
#endif

#define tcflag_t        int
#define oflags          int

#define MAX_PREF_LEN    255


#define EVENTS_COUNT    41

void *event_handlers[EVENTS_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

enum YM_EVENTS {
    YM_ON_LOGIN_RESPONSE=0,
    YM_ON_GOT_BUDDIES,
    YM_ON_GOT_IGNORE,
    YM_ON_GOT_IDENTITIES,
    YM_ON_GOT_COOKIES,
    YM_ON_STATUS_CHANGED,
    YM_ON_GOT_IM,
    YM_ON_GOT_CONF_INVITE,
    YM_ON_CONF_USER_DECLINE,
    YM_ON_CONF_USER_JOIN,
    YM_ON_CONF_USER_LEAVE,
    YM_ON_CONF_MESSAGE,
    YM_ON_CHAT_CAT_XML,
    YM_ON_CHAT_JOIN,
    YM_ON_CHAT_USERJOIN,
    YM_ON_CHAT_USERLEAVE,
    YM_ON_CHAT_MESSAGE,
    YM_ON_CHAT_YAHOO_LOGOUT,
    YM_ON_CHAT_YAHOO_ERROR,
    YM_ON_GOT_WEBCAM_IMAGE,
    YM_ON_WEBCAM_INVITE,
    YM_ON_WEBCAM_INVITE_REPLY,
    YM_ON_WEBCAM_CLOSED,
    YM_ON_WEBCAM_VIEWER,
    YM_ON_WEBCAM_DATA_REQUEST,
    YM_ON_GOT_FILE,
    YM_ON_CONTACT_ADDED,
    YM_ON_REJECTED,
    YM_ON_TYPING_NOTIFY,
    YM_ON_GAME_NOTIFY,
    YM_ON_MAIL_NOTIFY,
    YM_ON_GOT_SEARCH_RESULT,
    YM_ON_SYSTEM_MESSAGE,
    YM_ON_YAHOO_ERROR,
    YM_ON_YAHOO_LOG,
    YM_ON_ADD_HANDLER,
    YM_ON_REMOVE_HANDLER,
    YM_ON_CONNECT,
    YM_ON_CONNECT_ASYNC,
    YM_ON_GOT_FT_DATA,
    YM_ON_GOT_FILES,
    YM_ON_FILE_TRANSFER_DONE
};

YList *ArrToYList(void *arr) {
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    YList *lst = 0;
    YList *ptr = 0;

    for (INTEGER i = 0; i < count; i++) {
        INTEGER type = -1;
        char    *str = 0;
        NUMBER  nr   = 0;

        InvokePtr(INVOKE_GET_ARRAY_ELEMENT, arr, i, &type, &str, &nr);
        if (type == VARIABLE_STRING) {
            YList *ylist = y_new0(YList, sizeof(YList));

            ylist->next = 0;
            ylist->prev = ptr;
            ylist->data = str;

            if (ptr)
                ptr->next = ylist;

            ptr = ylist;

            if (!lst)
                lst = ptr;
        }
    }

    return lst;
}

//---------------------------------------------------------------------------
void YListToArr(const YList *lst, void *arr) {
    InvokePtr(INVOKE_CREATE_ARRAY, arr);

    YList *b;

    INTEGER index = 0;
    for (b = (YList *)lst; b; b = b->next) {
        void *arr_var = 0;
        char *str     = (char *)b->data;

        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, arr, index, (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        index++;
    }
}

//---------------------------------------------------------------------------
void BuddiesToArr(const YList *lst, void *arr) {
    InvokePtr(INVOKE_CREATE_ARRAY, arr);

    YList *b;

    INTEGER index = 0;
    for (b = (YList *)lst; b; b = b->next) {
        void        *arr_var = 0;
        yahoo_buddy *yb      = (yahoo_buddy *)b->data;

        if (yb) {
            InvokePtr(INVOKE_ARRAY_VARIABLE, arr, index, &arr_var);

            if (arr_var) {
                InvokePtr(INVOKE_CREATE_ARRAY, arr_var);

                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "group", (INTEGER)VARIABLE_STRING, (char *)yb->group, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "id", (INTEGER)VARIABLE_STRING, (char *)yb->id, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "real_name", (INTEGER)VARIABLE_STRING, (char *)yb->real_name, (NUMBER)0);


                if (yb->yab_entry) {
                    void *arr_var2 = 0;
                    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "yab_entry", &arr_var2);

                    InvokePtr(INVOKE_CREATE_ARRAY, arr_var2);

                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "id", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->id, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "fname", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->fname, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "lname", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->lname, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "nname", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->nname, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "email", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->email, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "hphone", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->hphone, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "wphone", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->wphone, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "mphone", (INTEGER)VARIABLE_STRING, (char *)yb->yab_entry->mphone, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var2, "dbid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)yb->yab_entry->dbid);
                }
            }
        }
        index++;
    }
}

//---------------------------------------------------------------------------
void MembersToArr(const YList *lst, void *arr) {
    InvokePtr(INVOKE_CREATE_ARRAY, arr);

    YList *b;

    INTEGER index = 0;
    for (b = (YList *)lst; b; b = b->next) {
        void *arr_var         = 0;
        yahoo_chat_member *yb = (yahoo_chat_member *)b->data;

        if (yb) {
            InvokePtr(INVOKE_ARRAY_VARIABLE, arr, index, &arr_var);

            if (arr_var) {
                InvokePtr(INVOKE_CREATE_ARRAY, arr_var);

                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "id", (INTEGER)VARIABLE_STRING, (char *)yb->id, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "age", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)yb->age);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "attribs", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)yb->attribs);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "alias", (INTEGER)VARIABLE_STRING, (char *)yb->alias, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "location", (INTEGER)VARIABLE_STRING, (char *)yb->location, (NUMBER)0);
            }
        }
        index++;
    }
}

//---------------------------------------------------------------------------
int FilesToArr(YList *lst, void *arr) {
    InvokePtr(INVOKE_CREATE_ARRAY, arr);
    int count = 0;

    YList *b;

    INTEGER index = 0;
    for (b = (YList *)lst; b; b = b->next) {
        void *arr_var = 0;
        struct yahoo_file_info *fi = (struct yahoo_file_info *)b->data;

        if (fi) {
            InvokePtr(INVOKE_ARRAY_VARIABLE, arr, index, &arr_var);

            if (arr_var) {
                InvokePtr(INVOKE_CREATE_ARRAY, arr_var);

                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "filename", (INTEGER)VARIABLE_STRING, (char *)fi->filename, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "filesize", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)fi->filesize);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "fid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)recv_id++);
                count++;
            }
        }
        index++;
        FREE(fi);
    }
    y_list_free_1(lst);
    return count;
}

//---------------------------------------------------------------------------



static int    do_mail_notify        = 0;
static int    do_yahoo_debug        = 0;
static int    ignore_system         = 0;
static int    do_typing_notify      = 1;
static int    accept_webcam_viewers = 1;
static int    send_webcam_images    = 0;
static int    webcam_direction      = YAHOO_WEBCAM_DOWNLOAD;
static time_t curTime     = 0;
static time_t pingTimer   = 0;
static time_t webcamTimer = 0;
static double webcamStart = 0;

/* id of the webcam connection (needed for uploading) */
static int webcam_id = 0;

static int poll_loop = 1;

static void register_callbacks();

typedef struct {
    char yahoo_id[255];
    char password[255];
    int  id;
    int  fd;
    int  status;
    char *msg;
} yahoo_local_account;

typedef struct {
    char yahoo_id[255];
    char name[255];
    int  status;
    int  away;
    char *msg;
    char group[255];
} yahoo_account;

typedef struct {
    int  id;
    char *label;
} yahoo_idlabel;

typedef struct {
    int        id;
    const char *who;
} yahoo_authorize_data;

yahoo_idlabel yahoo_status_codes[] = {
    { YAHOO_STATUS_AVAILABLE,   "Available"     },
    { YAHOO_STATUS_BRB,         "BRB"           },
    { YAHOO_STATUS_BUSY,        "Busy"          },
    { YAHOO_STATUS_NOTATHOME,   "Not Home"      },
    { YAHOO_STATUS_NOTATDESK,   "Not at Desk"   },
    { YAHOO_STATUS_NOTINOFFICE, "Not in Office" },
    { YAHOO_STATUS_ONPHONE,     "On Phone"      },
    { YAHOO_STATUS_ONVACATION,  "On Vacation"   },
    { YAHOO_STATUS_OUTTOLUNCH,  "Out to Lunch"  },
    { YAHOO_STATUS_STEPPEDOUT,  "Stepped Out"   },
    { YAHOO_STATUS_INVISIBLE,   "Invisible"     },
    { YAHOO_STATUS_IDLE,        "Idle"          },
    { YAHOO_STATUS_OFFLINE,     "Offline"       },
    { YAHOO_STATUS_CUSTOM,      "[Custom]"      },
    //{YAHOO_STATUS_NOTIFY, "Notify"},
    {                        0, NULL            }
};

char *yahoo_status_code(enum yahoo_status s) {
    int i;

    for (i = 0; yahoo_status_codes[i].label; i++)
        if (yahoo_status_codes[i].id == s)
            return yahoo_status_codes[i].label;
    return "Unknown";
}

#define YAHOO_DEBUGLOG    ext_yahoo_log

int ext_yahoo_log(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fflush(stderr);
    va_end(ap);
    return 0;
}

#define LOG(x)                                                           \
    if (do_yahoo_debug) { YAHOO_DEBUGLOG("%s:%d: ", __FILE__, __LINE__); \
                          YAHOO_DEBUGLOG x;                              \
                          YAHOO_DEBUGLOG("\n"); }

#define WARNING(x)                                                                \
    if (do_yahoo_debug) { YAHOO_DEBUGLOG("%s:%d: warning: ", __FILE__, __LINE__); \
                          YAHOO_DEBUGLOG x;                                       \
                          YAHOO_DEBUGLOG("\n"); }

#define print_message(x)    { printf x; printf("\n"); }

static yahoo_local_account *ylad = NULL;
static YList *buddies            = NULL;

extern "C" {
void ext_yahoo_got_ping(int id, const char *errormsg) {
    LOG(("got ping errormsg %s", errormsg));
}

void ext_yahoo_got_search_result(int id, int found, int start, int total, YList *contacts) {
    LOG(("got search result"));
}

void ext_yahoo_got_buddyicon_checksum(int id, const char *a, const char *b, int checksum) {
    LOG(("got buddy icon checksum"));
}

void ext_yahoo_got_buddyicon(int id, const char *a, const char *b, const char *c, int checksum) {
    LOG(("got buddy icon"));
}

void ext_yahoo_buddyicon_uploaded(int id, const char *url) {
    LOG(("buddy icon uploaded"));
}

void ext_yahoo_got_buddyicon_request(int id, const char *me, const char *who) {
    LOG(("got buddy icon request from %s", who));
}
}


static int expired(time_t timer) {
    if (timer && (curTime >= timer))
        return 1;

    return 0;
}

static void rearm(time_t *timer, int seconds) {
    time(timer);
    *timer += seconds;
}

//FILE *popen(const char *command, const char *type);
//int pclose(FILE *stream);
//int gethostname(char *name, size_t len);

static double get_time() {
    struct timeval ct;

    //gettimeofday(&ct, 0);

    /* return time in milliseconds */
    return ct.tv_sec * 1E3 + ct.tv_usec / 1E3;
}

static int yahoo_ping_timeout_callback() {
    //print_message(("Sending a keep alive message"));
    yahoo_keepalive(ylad->id);
    rearm(&pingTimer, 600);
    return 1;
}

static int yahoo_webcam_timeout_callback(int id) {
    static unsigned image_num = 0;
    unsigned char   *image    = NULL;
    unsigned int    length    = 0;
    unsigned int    timestamp = get_time() - webcamStart;
    char            fname[1024];
    FILE            *f_image = NULL;
    struct stat     s_image;

    if (send_webcam_images) {
        sprintf(fname, "images/image_%.3d.jpg", image_num++);
        if (image_num > 999) image_num = 0;
        if (stat(fname, &s_image) == -1)
            return -1;
        length = s_image.st_size;
        image  = y_new0(unsigned char, length);

        if ((f_image = fopen(fname, "r")) != NULL) {
            fread(image, length, 1, f_image);
            fclose(f_image);
        } else {
            printf("Error reading from %s\n", fname);
        }
    }

    //print_message(("Sending a webcam image (%d bytes)", length));
    yahoo_webcam_send_image(id, image, length, timestamp);
    FREE(image);
    rearm(&webcamTimer, 2);
    return 1;
}

YList *conferences = NULL;
typedef struct {
    int   id;
    char  *room_name;
    char  *host;
    YList *members;
    int   joined;
} conf_room;

static char *get_buddy_name(char *yid) {
    YList *b;

    for (b = buddies; b; b = b->next) {
        yahoo_account *ya = (yahoo_account *)b->data;
        if (!strcmp(yid, ya->yahoo_id))
            return ya->name && *ya->name ? ya->name : ya->yahoo_id;
    }

    return yid;
}

static conf_room *find_conf_room_by_name_and_id(int id, const char *name) {
    YList *l;

    for (l = conferences; l; l = l->next) {
        conf_room *cr = (conf_room *)l->data;
        if ((cr->id == id) && !strcmp(name, cr->room_name)) {
            return cr;
        }
    }

    return NULL;
}

void ext_yahoo_got_conf_invite(int id, const char *me, const char *who, const char *room, const char *msg, YList *members) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_CONF_INVITE]) {
        void *var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        MembersToArr(members, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_CONF_INVITE], &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
    }

    if (!result) {
        conf_room *cr = y_new0(conf_room, 1);
        cr->room_name = strdup(room);
        cr->host      = strdup(who);
        cr->members   = members;
        cr->id        = id;

        conferences = y_list_append(conferences, cr);

        /*print_message(("%s has invited you to a conference: %s\n"
                                "with the message: %s",
                                who, room, msg));

           for(; members; members=members->next)
                print_message(("\t%s", (char *)members->data));*/
    }
}

void ext_yahoo_conf_userdecline(int id, const char *me, const char *who, const char *room, const char *msg) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CONF_USER_DECLINE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CONF_USER_DECLINE], &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        YList     *l;
        conf_room *cr = find_conf_room_by_name_and_id(id, room);

        if (cr)
            for (l = cr->members; l; l = l->next) {
                char *w = (char *)l->data;
                if (!strcmp(w, who)) {
                    FREE(l->data);
                    cr->members = y_list_remove_link(cr->members, l);
                    y_list_free_1(l);
                    break;
                }
            }

        /*print_message(("%s declined the invitation to %s\n"
                                "with the message: %s", who, room, msg));*/
    }
}

void ext_yahoo_conf_userjoin(int id, const char *me, const char *who, const char *room) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CONF_USER_JOIN]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CONF_USER_JOIN], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        conf_room *cr = find_conf_room_by_name_and_id(id, room);
        if (cr) {
            YList *l = NULL;
            for (l = cr->members; l; l = l->next) {
                char *w = (char *)l->data;
                if (!strcmp(w, who))
                    break;
            }
            if (!l)
                cr->members = y_list_append(cr->members, strdup(who));
        }

        //print_message(("%s joined the conference %s", who, room));
    }
}

void ext_yahoo_conf_userleave(int id, const char *me, const char *who, const char *room) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CONF_USER_LEAVE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CONF_USER_LEAVE], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        YList     *l;
        conf_room *cr = find_conf_room_by_name_and_id(id, room);

        if (cr)
            for (l = cr->members; l; l = l->next) {
                char *w = (char *)l->data;
                if (!strcmp(w, who)) {
                    FREE(l->data);
                    cr->members = y_list_remove_link(cr->members, l);
                    y_list_free_1(l);
                    break;
                }
            }

        //print_message(("%s left the conference %s", who, room));
    }
}

void ext_yahoo_conf_message(int id, const char *me, const char *who, const char *room, const char *msg, int utf8) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CONF_MESSAGE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CONF_MESSAGE], &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)utf8
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        /*char * umsg = msg;

           if(utf8)
                umsg = y_utf8_to_str(msg);

           who = get_buddy_name(who);

           print_message(("%s (in %s): %s", who, room, umsg));

           if(utf8)
                FREE(umsg);*/
    }
}

static void print_chat_member(struct yahoo_chat_member *ycm) {
    printf("%s (%s) ", ycm->id, ycm->alias);
    printf(" Age: %d Sex: ", ycm->age);
    if (ycm->attribs & YAHOO_CHAT_MALE) {
        printf("Male");
    } else if (ycm->attribs & YAHOO_CHAT_FEMALE) {
        printf("Female");
    } else {
        printf("Unknown");
    }
    if (ycm->attribs & YAHOO_CHAT_WEBCAM) {
        printf(" with webcam");
    }

    printf("  Location: %s", ycm->location);
}

void ext_yahoo_chat_cat_xml(int id, const char *xml) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_CAT_XML]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_CAT_XML], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)xml, (double)0
                  );


        GET_EVENT_RESULT;
    }
}

void ext_yahoo_chat_join(int id, const char *me, const char *room, const char *topic, YList *members, int fd) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_JOIN]) {
        void *var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        MembersToArr(members, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_JOIN], &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)topic, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)fd,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
    }

    if (!result) {
        //print_message(("You have joined the chatroom %s with topic %s", room, topic));

        while (members) {
            YList *n = members->next;

            //printf("\t");
            //print_chat_member((yahoo_chat_member *)members->data);
            //printf("\n");
            FREE(((struct yahoo_chat_member *)members->data)->id);
            FREE(((struct yahoo_chat_member *)members->data)->alias);
            FREE(((struct yahoo_chat_member *)members->data)->location);
            FREE(members->data);
            FREE(members);
            members = n;
        }
    }
}

void ext_yahoo_chat_userjoin(int id, const char *me, const char *room, struct yahoo_chat_member *who) {
    //print_chat_member(who);
    //print_message((" joined the chatroom %s", room));
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_USERJOIN]) {
        void *arr_var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &arr_var);
        InvokePtr(INVOKE_CREATE_ARRAY, arr_var);

        if (who) {
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "id", (INTEGER)VARIABLE_STRING, (char *)who->id, (NUMBER)0);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "age", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)who->age);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "attribs", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)who->attribs);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "alias", (INTEGER)VARIABLE_STRING, (char *)who->alias, (NUMBER)0);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr_var, "location", (INTEGER)VARIABLE_STRING, (char *)who->location, (NUMBER)0);
        }

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_USERJOIN], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_UNDEFINED, arr_var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, arr_var);

        GET_EVENT_RESULT;
    }

    if (!result) {
        FREE(who->id);
        FREE(who->alias);
        FREE(who->location);
        FREE(who);
    }
}

void ext_yahoo_chat_userleave(int id, const char *me, const char *room, const char *who) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_USERLEAVE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_USERLEAVE], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0
                  );


        GET_EVENT_RESULT;
    }
    //print_message(("%s left the chatroom %s", who, room));
}

void ext_yahoo_chat_message(int id, const char *me, const char *who, const char *room, const char *msg, int msgtype, int utf8) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_MESSAGE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_MESSAGE], &RES, &EXCEPTION, (INTEGER)6,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)room, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)msgtype,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)utf8
                  );


        GET_EVENT_RESULT;
    }
    if (!result) {
        /*char * umsg = msg;
           char * charpos;

           if(utf8)
                umsg = y_utf8_to_str(msg);

           charpos = umsg;
           while(*charpos) {
                if (*charpos == 0x1b) {
         * charpos = ' ';
                }
                charpos++;
           }

           if(utf8)
                FREE(umsg);*/
    }
}

void ext_yahoo_status_changed(int id, const char *who, int stat, const char *msg, int away, int idle, int mobile) {
    INIT_EVENT;

    if (event_handlers[YM_ON_STATUS_CHANGED]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_STATUS_CHANGED], &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)stat,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)away,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)idle,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)mobile
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        yahoo_account *ya = NULL;
        YList         *b;
        for (b = buddies; b; b = b->next) {
            if (!strcmp(((yahoo_account *)b->data)->yahoo_id, who)) {
                ya = (yahoo_account *)b->data;
                break;
            }
        }

        /*if(msg)
                print_message(("%s (%s) is now %s", ya?ya->name:who, who, msg))
           else if(stat == YAHOO_STATUS_IDLE)
                print_message(("%s (%s) idle for %d:%02d:%02d", ya?ya->name:who, who,
                                        away/3600, (away/60)%60, away%60))
           else
                print_message(("%s (%s) is now %s", ya?ya->name:who, who,
                                        (int)yahoo_status_code(stat)))*/

        if (ya) {
            ya->status = stat;
            ya->away   = away;
            if (msg) {
                FREE(ya->msg);
                ya->msg = strdup(msg);
            }
        }
    }
}

void ext_yahoo_got_buddies(int id, YList *buds) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_BUDDIES]) {
        void *var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        BuddiesToArr(buds, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_BUDDIES], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
    }

    if (!result) {
        while (buddies) {
            FREE(buddies->data);
            buddies = buddies->next;
            if (buddies)
                FREE(buddies->prev);
        }
        for ( ; buds; buds = buds->next) {
            yahoo_account      *ya  = y_new0(yahoo_account, 1);
            struct yahoo_buddy *bud = (yahoo_buddy *)buds->data;
            strncpy(ya->yahoo_id, bud->id, 255);
            if (bud->real_name)
                strncpy(ya->name, bud->real_name, 255);
            strncpy(ya->group, bud->group, 255);
            ya->status = YAHOO_STATUS_OFFLINE;
            buddies    = y_list_append(buddies, ya);

            /*		print_message(("%s is %s", bud->id, bud->real_name));*/
        }
    }
}

void ext_yahoo_got_ignore(int id, YList *igns) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_IGNORE]) {
        void *var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        MembersToArr(igns, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_IGNORE], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
    }
}

void ext_yahoo_got_im(int id, const char *me, const char *who, const char *msg, long tm, int stat, int utf8) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_IM]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_IM], &RES, &EXCEPTION, (INTEGER)6,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)tm,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)stat,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)utf8
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        /*char *umsg = msg;

           if(stat == 2) {
                LOG(("Error sending message to %s", who));
                return;
           }

           if(!msg)
                return;

           if(utf8)
                umsg = y_utf8_to_str(msg);

           who = get_buddy_name(who);

           if(tm) {
                char timestr[255];

                strncpy(timestr, ctime((time_t *)&tm), sizeof(timestr));
                timestr[strlen(timestr) - 1] = '\0';

                print_message(("[Offline message at %s from %s]: %s",
                                timestr, who, umsg))
           } else {
                if(!strcmp(umsg, "<ding>"))
                        printf("\a");
                print_message(("%s: %s", who, umsg))
           }

           if(utf8)
                FREE(umsg);*/
    }
}

void ext_yahoo_rejected(int id, const char *who, const char *msg) {
    INIT_EVENT;

    if (event_handlers[YM_ON_REJECTED]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_REJECTED], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0
                  );


        GET_EVENT_RESULT;
    }
}

void ext_yahoo_contact_added(int id, const char *myid, const char *who, const char *msg) {
    INIT_EVENT;

    if (event_handlers[YM_ON_CONTACT_ADDED]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CONTACT_ADDED], &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)myid, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0
                  );


        GET_EVENT_RESULT;
    }
    //yahoo_reject_buddy(id, who, "Thanks, but no thanks.");
}

void ext_yahoo_typing_notify(int id, const char *me, const char *who, int stat) {
    INIT_EVENT;

    if (event_handlers[YM_ON_TYPING_NOTIFY]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_TYPING_NOTIFY], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)stat
                  );


        GET_EVENT_RESULT;
    }
}

void ext_yahoo_game_notify(int id, const char *me, const char *who, int stat) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GAME_NOTIFY]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GAME_NOTIFY], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)stat
                  );


        GET_EVENT_RESULT;
    }
}

void ext_yahoo_mail_notify(int id, const char *from, const char *subj, int cnt) {
    INIT_EVENT;

    if (event_handlers[YM_ON_MAIL_NOTIFY]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_MAIL_NOTIFY], &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)from, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)subj, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cnt
                  );


        GET_EVENT_RESULT;
    }
    if (!result) {
        /*char buff[1024] = {0};

           if(!do_mail_notify)
                return;

           if(from && subj)
                snprintf(buff, sizeof(buff),
                                "You have new mail from %s about %s\n",
                                from, subj);
           if(cnt) {
                char buff2[100];
                snprintf(buff2, sizeof(buff2),
                                "You have %d message%s\n",
                                cnt, cnt==1?"":"s");
                strcat(buff, buff2);
           }

           if(buff[0])
                print_message((buff));*/
    }
}

void ext_yahoo_got_webcam_image(int id, const char *who,
                                const unsigned char *image, unsigned int image_size, unsigned int real_size,
                                unsigned int timestamp) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_WEBCAM_IMAGE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_WEBCAM_IMAGE], &RES, &EXCEPTION, (INTEGER)6,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)image, (double)image_size,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)image_size,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)real_size,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)timestamp
                  );


        GET_EVENT_RESULT;
    }

/*
    if (!result) {
        static unsigned char *cur_image = NULL;
        static unsigned int cur_image_len = 0;
        static unsigned int image_num = 0;
        FILE* f_image;
        char fname[1024];

        if (real_size)
        {
                if (!cur_image) cur_image = y_new0(unsigned char, image_size);
                memcpy(cur_image + cur_image_len, image, real_size);
                cur_image_len += real_size;
        }

        if (image_size == cur_image_len)
        {
                print_message(("Received a image update at %d (%d bytes)",
                         timestamp, image_size));

                if (image_size)
                {
                        sprintf(fname, "images/%s_%.3d.jpc", who, image_num++);

                        if ((f_image = fopen(fname, "w")) != NULL) {
                                fwrite(cur_image, image_size, 1, f_image);
                                fclose(f_image);
                        } else {
                                printf("Error writing to %s\n", fname);
                        }
                        FREE(cur_image);
                        cur_image_len = 0;
                        if (image_num > 999) image_num = 0;
                }
        }
 */
}

void ext_yahoo_webcam_viewer(int id, const char *who, int connect) {
    INIT_EVENT;

    if (event_handlers[YM_ON_WEBCAM_VIEWER]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_WEBCAM_VIEWER], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)connect
                  );


        GET_EVENT_RESULT;
    }

    /*switch (connect)
       {
            case 0:
                    print_message(("%s has stopped viewing your webcam", who));
                    break;
            case 1:
                    print_message(("%s has started viewing your webcam", who));
                    break;
            case 2:
                    print_message(("%s is trying to view your webcam", who));
                    yahoo_webcam_accept_viewer(id, who, accept_webcam_viewers);
                    break;
       }*/
}

void ext_yahoo_webcam_closed(int id, const char *who, int reason) {
    INIT_EVENT;

    if (event_handlers[YM_ON_WEBCAM_CLOSED]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_WEBCAM_CLOSED], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)reason
                  );


        GET_EVENT_RESULT;
    }

    /*switch(reason)
       {
            case 1:
                    print_message(("%s stopped broadcasting", who));
                    break;
            case 2:
                    print_message(("%s cancelled viewing permission", who));
                    break;
            case 3:
                    print_message(("%s declines permission to view his/her webcam", who));
                    break;
            case 4:
                    print_message(("%s does not have his/her webcam online", who));
                    break;
       }*/
}

void ext_yahoo_webcam_data_request(int id, int send) {
    INIT_EVENT;

    if (event_handlers[YM_ON_WEBCAM_DATA_REQUEST]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_WEBCAM_DATA_REQUEST], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)send
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        webcam_id = id;

        if (send) {
            //print_message(("Got request to start sending images"));
            if (!webcamTimer)
                rearm(&webcamTimer, 2);
        } else {
            //print_message(("Got request to stop sending images"));
        }
        send_webcam_images = send;
    }
}

void ext_yahoo_webcam_invite(int id, const char *me, const char *from) {
    INIT_EVENT;

    if (event_handlers[YM_ON_WEBCAM_INVITE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_WEBCAM_INVITE], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)from, (double)0
                  );


        GET_EVENT_RESULT;
    }

    //print_message(("Got a webcam invitation from %s", from));
}

void ext_yahoo_webcam_invite_reply(int id, const char *me, const char *from, int accept) {
    INIT_EVENT;

    if (event_handlers[YM_ON_WEBCAM_INVITE_REPLY]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_WEBCAM_INVITE_REPLY], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)from, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)accept
                  );


        GET_EVENT_RESULT;
    }

    /*if(accept) {
            print_message(("%s accepted webcam invitation...", from));
       } else {
            print_message(("%s declined webcam invitation...", from));
       }*/
}

void ext_yahoo_system_message(int id, const char *msg) {
    INIT_EVENT;

    if (event_handlers[YM_ON_SYSTEM_MESSAGE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_SYSTEM_MESSAGE], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0
                  );


        GET_EVENT_RESULT;
    }

    /*if(ignore_system)
            return;

       print_message(("Yahoo System Message: %s", msg));*/
}

void yahoo_logout() {
    if (ylad->id <= 0) {
        return;
    }

    pingTimer = 0;

    while (conferences) {
        YList     *n  = conferences->next;
        conf_room *cr = (conf_room *)conferences->data;
        if (cr->joined)
            yahoo_conference_logoff(ylad->id, NULL, cr->members, cr->room_name);
        FREE(cr->room_name);
        FREE(cr->host);
        while (cr->members) {
            YList *n = cr->members->next;
            FREE(cr->members->data);
            FREE(cr->members);
            cr->members = n;
        }
        FREE(cr);
        FREE(conferences);
        conferences = n;
    }

    yahoo_logoff(ylad->id);
    yahoo_close(ylad->id);

    ylad->status = YAHOO_STATUS_OFFLINE;
    ylad->id     = 0;

    poll_loop = 0;

    //print_message(("logged_out"));
}

static char *get_local_addresses() {
    static char    addresses[1024];
    char           buff[1024];
    struct hostent *hn;

#ifndef _WIN32
    char gateway[16];
    char *c;
    int  i;
    FILE *f;
    f = popen("netstat -nr", "r");
    if (!f)
        goto IP_TEST_2;
    while (fgets(buff, sizeof(buff), f) != NULL) {
        c = strtok(buff, " ");
        if ((strstr(c, "default") || strstr(c, "0.0.0.0")) &&
            !strstr(c, "127.0.0"))
            break;
    }
    c = strtok(NULL, " ");
    pclose(f);

    strncpy(gateway, c, 16);



    for (i = strlen(gateway); gateway[i] != '.'; i--)
        gateway[i] = 0;

    gateway[i] = 0;

    for (i = strlen(gateway); gateway[i] != '.'; i--)
        gateway[i] = 0;

    f = popen("/sbin/ifconfig -a", "r");
    if (!f)
        goto IP_TEST_2;

    while (fgets(buff, sizeof(buff), f) != NULL) {
        if (strstr(buff, "inet") && strstr(buff, gateway))
            break;
    }
    pclose(f);

    c = strtok(buff, " ");
    c = strtok(NULL, " ");

    strncpy(addresses, c, sizeof(addresses));
    c = strtok(addresses, ":");
    strncpy(buff, c, sizeof(buff));
    if ((c = strtok(NULL, ":")))
        strncpy(buff, c, sizeof(buff));

    strncpy(addresses, buff, sizeof(addresses));

    return addresses;


IP_TEST_2:
#endif /* _WIN32 */

    gethostname(buff, sizeof(buff));

    hn = gethostbyname(buff);
    if (hn)
        strncpy(addresses, inet_ntoa(*((struct in_addr *)hn->h_addr)), sizeof(addresses));
    else
        addresses[0] = 0;

    return addresses;
}

void ext_yahoo_login(yahoo_local_account *ylad, int login_mode) {
    LOG(("ext_yahoo_login"));

    //ylad->id = yahoo_init(ylad->yahoo_id, ylad->password);
    local_host = strdup(get_local_addresses());

    ylad->id = yahoo_init_with_attributes(ylad->yahoo_id, ylad->password,
                                          "local_host", local_host,
                                          "pager_port", 23,
                                          NULL);

    ylad->status = YAHOO_STATUS_OFFLINE;
    yahoo_login(ylad->id, login_mode);

/*	if (ylad->id <= 0) {
                print_message(("Could not connect to Yahoo server.  Please verify that you are connected to the net and the pager host and port are correctly entered."));
                return;
        }
 */
    rearm(&pingTimer, 600);
}

void ext_yahoo_got_cookies(int id) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_COOKIES]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_COOKIES], &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id
                  );


        GET_EVENT_RESULT;
    }
    /*yahoo_get_yab(id);*/
}

void ext_yahoo_login_response(int id, int succ, const char *url) {
    INIT_EVENT;

    if (event_handlers[YM_ON_LOGIN_RESPONSE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_LOGIN_RESPONSE], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)succ,
                  (INTEGER)VARIABLE_STRING, (char *)url, (double)0
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        //char buff[1024];

        if (succ == YAHOO_LOGIN_OK) {
            ylad->status = yahoo_current_status(id);
            //print_message(("logged in"));
            return;
        }     /*else if(succ == YAHOO_LOGIN_UNAME) {

                    snprintf(buff, sizeof(buff), "Could not log into Yahoo service - username not recognised.  Please verify that your username is correctly typed.");
                 } else if(succ == YAHOO_LOGIN_PASSWD) {

                    snprintf(buff, sizeof(buff), "Could not log into Yahoo service - password incorrect.  Please verify that your password is correctly typed.");

                 } else if(succ == YAHOO_LOGIN_LOCK) {

                    snprintf(buff, sizeof(buff), "Could not log into Yahoo service.  Your account has been locked.\nVisit %s to reactivate it.", url);

                 } else if(succ == YAHOO_LOGIN_DUPL) {

                    snprintf(buff, sizeof(buff), "You have been logged out of the yahoo service, possibly due to a duplicate login.");
                 } else if(succ == YAHOO_LOGIN_SOCK) {

                    snprintf(buff, sizeof(buff), "The server closed the socket.");
                 } else {
                    snprintf(buff, sizeof(buff), "Could not log in, unknown reason: %d.", succ);
                 }*/

        ylad->status = YAHOO_STATUS_OFFLINE;
        //print_message((buff));
        yahoo_logout();
        poll_loop = 0;
    }
}

void ext_yahoo_error(int id, const char *err, int fatal, int num) {
    INIT_EVENT;

    if (event_handlers[YM_ON_YAHOO_ERROR]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_YAHOO_ERROR], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)err, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)fatal,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)num
                  );


        GET_EVENT_RESULT;
    }

    if (!result) {
        //fprintf(stdout, "Yahoo Error: ");
        //fprintf(stdout, "%s", err);

        /*
           switch(num) {
                case E_CONFNOTAVAIL:
                        fprintf(stdout, "%s is not available for the conference", err);
                        break;
                case E_IGNOREDUP:
                        fprintf(stdout, "%s is already ignored", err);
                        break;
                case E_IGNORENONE:
                        fprintf(stdout, "%s is not in the ignore list", err);
                        break;
                case E_IGNORECONF:
                        fprintf(stdout, "%s is in buddy list - cannot ignore ", err);
                        break;
           }
         */
        //fprintf(stdout, "\n");
        if (fatal)
            yahoo_logout();
    }
}

void yahoo_set_current_state(int yahoo_state) {
    if ((ylad->status == YAHOO_STATUS_OFFLINE) && (yahoo_state != YAHOO_STATUS_OFFLINE)) {
        ext_yahoo_login(ylad, yahoo_state);
        return;
    } else if ((ylad->status != YAHOO_STATUS_OFFLINE) && (yahoo_state == YAHOO_STATUS_OFFLINE)) {
        yahoo_logout();
        return;
    }

    ylad->status = yahoo_state;
    if (yahoo_state == YAHOO_STATUS_CUSTOM) {
        if (ylad->msg)
            yahoo_set_away(ylad->id, (yahoo_status)yahoo_state, ylad->msg, 1);
        else
            yahoo_set_away(ylad->id, (yahoo_status)yahoo_state, "delta p * delta x too large", 1);
    } else
        yahoo_set_away(ylad->id, (yahoo_status)yahoo_state, NULL, 1);
}

int ext_yahoo_connect(const char *host, int port) {
    struct sockaddr_in    serv_addr;
    static struct hostent *server;
    static char           last_host[256];
    int  servfd;
    char **p;

    if (last_host[0] || (strcasecmp(last_host, host) != 0)) {
        if (!(server = gethostbyname(host))) {
            WARNING(("failed to look up server (%s:%d)\n%d: %s",
                     host, port,
                     h_errno, strerror(h_errno)));
            return -1;
        }
        strncpy(last_host, host, 255);
    }

    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        WARNING(("Socket create error (%d): %s", errno, strerror(errno)));
        return -1;
    }

    LOG(("connecting to %s:%d", host, port));

    for (p = server->h_addr_list; *p; p++) {
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, *p, server->h_length);
        serv_addr.sin_port = htons(port);

        LOG(("trying %s", inet_ntoa(serv_addr.sin_addr)));
        if (connect(servfd, (struct sockaddr *)&serv_addr,
                    sizeof(serv_addr)) == -1) {
            if ((errno != ECONNREFUSED) && (errno != ETIMEDOUT) &&
                (errno != ENETUNREACH)) {
                break;
            }
        } else {
            LOG(("connected"));
            return servfd;
        }
    }

    WARNING(("Could not connect to %s:%d\n%d:%s", host, port, errno,
             strerror(errno)));
    close(servfd);
    return -1;
}

/*************************************
 * Callback handling code starts here
 */
YList *connections = NULL;

/*struct _conn {
        int tag;
        int fd;
        int id;
        yahoo_input_condition cond;
        void *data;
        int remove;
   };*/
struct _conn {
    int fd;
    SSL *ssl;
    int use_ssl;
    int remove;
};

struct conn_handler {
    struct _conn          *con;
    int                   id;
    int                   tag;
    yahoo_input_condition cond;
    int                   remove;
    void                  *data;
};

static int connection_tags = 0;

int ext_yahoo_add_handler(int id, void *d, yahoo_input_condition cond, void *data) {
    struct conn_handler *h = y_new0(struct conn_handler, 1);

    h->id   = id;
    h->tag  = ++connection_tags;
    h->con  = (_conn *)d;
    h->cond = cond;
    h->data = data;

    LOG(("Add %d(%d) for %d, tag %d", h->con->fd, cond, id, h->tag));

    connections = y_list_prepend(connections, h);

    return h->tag;
}

void ext_yahoo_remove_handler(int id, int tag) {
    YList *l;

    if (!tag)
        return;

    for (l = connections; l; l = y_list_next(l)) {
        struct conn_handler *c = (conn_handler *)l->data;
        if (c->tag == tag) {
            /* don't actually remove it, just mark it for removal */
            /* we'll remove when we start the next poll cycle */
            LOG(("Marking id:%d fd:%p tag:%d for removal",
                 c->id, c->con, c->tag));
            c->remove = 1;
            return;
        }
    }
}

static SSL *do_ssl_connect(int fd) {
    SSL     *ssl;
    SSL_CTX *ctx;

    LOG(("SSL Handshake"));

    SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());
    ssl = SSL_new(ctx);
    SSL_CTX_free(ctx);
    SSL_set_fd(ssl, fd);

    if (SSL_connect(ssl) == 1)
        return ssl;

    return NULL;
}

struct connect_callback_data {
    yahoo_connect_callback callback;
    void                   *callback_data;
    int                    id;
    int                    tag;
};


static void connect_complete(void *data, struct _conn *source, yahoo_input_condition condition) {
    struct connect_callback_data *ccd = (connect_callback_data *)data;
    int error, err_size = sizeof(error);

    ext_yahoo_remove_handler(0, ccd->tag);
    getsockopt(source->fd, SOL_SOCKET, SO_ERROR, (char *)&error, (socklen_t *)&err_size);

    if (error)
        goto err;

    LOG(("Connected fd: %d, error: %d", source->fd, error));

    if (source->use_ssl) {
        source->ssl = do_ssl_connect(source->fd);

        if (!source->ssl) {
err:
            LOG(("SSL Handshake Failed!"));
            ext_yahoo_close(source);

            ccd->callback(NULL, 0, ccd->callback_data);
            FREE(ccd);
            return;
        }
    }
#ifdef _WIN32
    u_long arg = 1;

    ioctlsocket(source->fd, FIONBIO, &arg);
#else
    fcntl(source->fd, F_SETFL, O_NONBLOCK);
#endif

    ccd->callback(source, error, ccd->callback_data);
    FREE(ccd);
}

/*


   static void connect_complete(void *data, int source, yahoo_input_condition condition)
   {
        struct connect_callback_data *ccd = (connect_callback_data *)data;
        int error;
   #ifdef _WIN32
    int err_size = sizeof(error);
   #else
    socklen_t err_size = sizeof(error);
   #endif

        ext_yahoo_remove_handler(0, ccd->tag);
   #ifdef _WIN32
        getsockopt(source, SOL_SOCKET, SO_ERROR, (char *)&error, &err_size);
   #else
        getsockopt(source, SOL_SOCKET, SO_ERROR, &error, &err_size);
   #endif

        if(error) {
                close(source);
                source = -1;
        }

        LOG(("Connected fd: %d, error: %d", source, error));

        ccd->callback(NULL, error, ccd->callback_data);
        FREE(ccd);
   }
 */
void yahoo_callback(struct conn_handler *c, yahoo_input_condition cond) {
    int  ret        = 1;
    char buff[1024] = { 0 };

    if (c->id < 0) {
        connect_complete(c->data, c->con, cond);
    } else {
        if (cond & YAHOO_INPUT_READ)
            ret = yahoo_read_ready(c->id, c->con, c->data);
        if ((ret > 0) && cond & YAHOO_INPUT_WRITE)
            ret = yahoo_write_ready(c->id, c->con, c->data);

        if (ret == -1)
            snprintf(buff, sizeof(buff),
                     "Yahoo read error (%d): %s", errno, strerror(errno));
        else if (ret == 0)
            snprintf(buff, sizeof(buff),
                     "Yahoo read error: Server closed socket");

        if (buff[0])
            print_message((buff));
    }
}

void ext_yahoo_close(void *fd) {
    struct _conn *c = (_conn *)fd;
    YList        *l;

    close(c->fd);
    c->fd = 0;

    /* Remove all handlers */
    for (l = connections; l; l = y_list_next(l)) {
        struct conn_handler *h = (conn_handler *)l->data;

        if (h->con == c)
            h->remove = 1;
    }

    c->remove = 1;
}

int ext_yahoo_connect_async(int id, const char *host, int port,
                            yahoo_connect_callback callback, void *data, int use_ssl) {
    struct sockaddr_in    serv_addr;
    static struct hostent *server;
    int servfd;
    struct connect_callback_data *ccd;
    int error;
    SSL *ssl = NULL;

    struct _conn *c;

    LOG(("Connecting to %s", host));

    if (!(server = gethostbyname(host))) {
        errno = h_errno;
        return -1;
    }

    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, *server->h_addr_list, server->h_length);
    serv_addr.sin_port = htons(port);

    c          = y_new0(struct _conn, 1);
    c->fd      = servfd;
    c->use_ssl = use_ssl;

    error = connect(servfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    LOG(("Trying to connect: fd:%d error:%d", servfd, error));
    if (!error) {
        LOG(("Connected"));
        if (use_ssl) {
            ssl = do_ssl_connect(servfd);

            if (!ssl) {
                LOG(("SSL Handshake Failed!"));
                ext_yahoo_close(c);

                callback(NULL, 0, data);
                return -1;
            }
        }

        c->ssl = ssl;
#ifdef _WIN32
        u_long arg = 1;

        ioctlsocket(c->fd, FIONBIO, &arg);
#else
        fcntl(c->fd, F_SETFL, O_NONBLOCK);
#endif

        callback(c, 0, data);
        return 0;
#ifdef _WIN32
    } else if ((error == -1) && (errno == WSAEINPROGRESS)) {
#else
    } else if ((error == -1) && (errno == EINPROGRESS)) {
#endif
        ccd                = (connect_callback_data *)calloc(1, sizeof(struct connect_callback_data));
        ccd->callback      = callback;
        ccd->callback_data = data;
        ccd->id            = id;

        ccd->tag = ext_yahoo_add_handler(-1, c, YAHOO_INPUT_WRITE, ccd);
        return ccd->tag;
    } else {
        if (error == -1)
            LOG(("Connection failure: %s", strerror(errno)));

        ext_yahoo_close(c);

        callback(NULL, 0, data);
        return -1;
    }
}

/*
 * Callback handling code ends here
 ***********************************/

static void process_commands(char *line) {
    char *cmd, *to, *msg;

    char *tmp, *start;
    char *copy = strdup(line);

    enum yahoo_status state;

    start = cmd = copy;
    tmp   = strchr(copy, ' ');
    if (tmp) {
        *tmp = '\0';
        copy = tmp + 1;
    } else {
        copy = NULL;
    }

    if (!strncasecmp(cmd, "MSG", strlen("MSG"))) {
        /* send a message */
        to  = copy;
        tmp = strchr(copy, ' ');
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 1;
        }
        msg = copy;
        if (to && msg) {
            if (!strcmp(msg, "\a"))
                yahoo_send_im(ylad->id, NULL, to, "<ding>", 0, 0);
            else {
                //msg = y_str_to_utf8(msg);
                yahoo_send_im(ylad->id, NULL, to, msg, 1, 0);
                FREE(msg);
            }
        }
    } else if (!strncasecmp(cmd, "CMS", strlen("CMS"))) {
        /* send a message */
        conf_room *cr;
        to  = copy;
        tmp = strchr(copy, ' ');
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 1;
        }
        msg = copy;
        cr  = find_conf_room_by_name_and_id(ylad->id, to);
        if (!cr) {
            print_message(("no such room: %s", copy));
            goto end_parse;
        }
        if (msg)
            yahoo_conference_message(ylad->id, NULL, cr->members, to, msg, 0);
    } else if (!strncasecmp(cmd, "CLS", strlen("CLS"))) {
        YList *l;
        if (copy) {
            conf_room *cr = find_conf_room_by_name_and_id(ylad->id, copy);
            if (!cr) {
                print_message(("no such room: %s", copy));
                goto end_parse;
            }
            print_message(("Room: %s", copy));
            for (l = cr->members; l; l = l->next) {
                print_message(("%s", (char *)l->data))
            }
        } else {
            print_message(("All Rooms:"));
            for (l = conferences; l; l = l->next) {
                conf_room *cr = (conf_room *)l->data;
                print_message(("%s", cr->room_name));
            }
        }
    } else if (!strncasecmp(cmd, "CCR", strlen("CCR"))) {
        conf_room *cr = y_new0(conf_room, 1);
        while ((tmp = strchr(copy, ' ')) != NULL) {
            *tmp = '\0';
            if (!cr->room_name)
                cr->room_name = strdup(copy);
            else
                cr->members = y_list_append(cr->members,
                                            strdup(copy));
            copy = tmp + 1;
        }
        cr->members = y_list_append(cr->members, strdup(copy));

        if (!cr->room_name || !cr->members) {
            FREE(cr);
        } else {
            cr->id      = ylad->id;
            cr->joined  = 1;
            conferences = y_list_append(conferences, cr);
            yahoo_conference_invite(ylad->id, NULL, cr->members, cr->room_name, "Join my conference");
            cr->members = y_list_append(cr->members, strdup(ylad->yahoo_id));
        }
    } else if (!strncasecmp(cmd, "CIN", strlen("CIN"))) {
        conf_room *cr;
        char      *room = copy;
        YList     *l1, *l = NULL;

        while ((tmp = strchr(copy, ' ')) != NULL) {
            *tmp = '\0';
            copy = tmp + 1;
            l    = y_list_append(l, copy);
        }

        cr = find_conf_room_by_name_and_id(ylad->id, room);
        if (!cr) {
            print_message(("no such room: %s", room));
            y_list_free(l);
            goto end_parse;
        }

        for (l1 = l; l1; l1 = l1->next) {
            char *w = (char *)l1->data;
            yahoo_conference_addinvite(ylad->id, NULL, w, room, cr->members, "Join my conference");
            cr->members = y_list_append(cr->members, strdup(w));
        }
        y_list_free(l);
    } else if (!strncasecmp(cmd, "CLN", strlen("CLN"))) {
        conf_room *cr = find_conf_room_by_name_and_id(ylad->id, copy);
        YList     *l;
        if (!cr) {
            print_message(("no such room: %s", copy));
            goto end_parse;
        }

        cr->joined = 1;
        for (l = cr->members; l; l = l->next) {
            char *w = (char *)l->data;
            if (!strcmp(w, ylad->yahoo_id))
                break;
        }
        if (!l)
            cr->members = y_list_append(cr->members, strdup(ylad->yahoo_id));
        yahoo_conference_logon(ylad->id, NULL, cr->members, copy);
    } else if (!strncasecmp(cmd, "CLF", strlen("CLF"))) {
        conf_room *cr = find_conf_room_by_name_and_id(ylad->id, copy);

        if (!cr) {
            print_message(("no such room: %s", copy));
            goto end_parse;
        }

        yahoo_conference_logoff(ylad->id, NULL, cr->members, copy);

        conferences = y_list_remove(conferences, cr);
        FREE(cr->room_name);
        FREE(cr->host);
        while (cr->members) {
            YList *n = cr->members->next;
            FREE(cr->members->data);
            FREE(cr->members);
            cr->members = n;
        }
        FREE(cr);
    } else if (!strncasecmp(cmd, "CDC", strlen("CDC"))) {
        conf_room *cr;
        char      *room = copy;
        tmp = strchr(copy, ' ');
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 1;
            msg  = copy;
        } else {
            msg = "Thanks, but no thanks!";
        }

        cr = find_conf_room_by_name_and_id(ylad->id, room);
        if (!cr) {
            print_message(("no such room: %s", room));
            goto end_parse;
        }

        yahoo_conference_decline(ylad->id, NULL, cr->members, room, msg);

        conferences = y_list_remove(conferences, cr);
        FREE(cr->room_name);
        FREE(cr->host);
        while (cr->members) {
            YList *n = cr->members->next;
            FREE(cr->members->data);
            FREE(cr->members);
            cr->members = n;
        }
        FREE(cr);
    } else if (!strncasecmp(cmd, "CHL", strlen("CHL"))) {
        int roomid;
        roomid = atoi(copy);
        yahoo_get_chatrooms(ylad->id, roomid);
    } else if (!strncasecmp(cmd, "CHJ", strlen("CHJ"))) {
        char *roomid, *roomname;
/* Linux, FreeBSD, Solaris:1 */
/* 1600326591 */
        roomid = copy;
        tmp    = strchr(copy, ' ');
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 1;
        }
        roomname = copy;
        if (roomid && roomname) {
            yahoo_chat_logon(ylad->id, NULL, roomname, roomid);
        }
    } else if (!strncasecmp(cmd, "CHM", strlen("CHM"))) {
        char *msg, *roomname;
        roomname = copy;
        tmp      = strstr(copy, "  ");
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 2;
        }
        msg = copy;
        if (roomname && msg) {
            yahoo_chat_message(ylad->id, NULL, roomname, msg, 1, 0);
        }
    } else if (!strncasecmp(cmd, "CHX", strlen("CHX"))) {
        yahoo_chat_logoff(ylad->id, NULL);
    } else if (!strncasecmp(cmd, "STA", strlen("STA"))) {
        if (isdigit(copy[0])) {
            state = (enum yahoo_status)atoi(copy);
            copy  = strchr(copy, ' ');
            if (state == 99) {
                if (copy)
                    msg = copy;
                else
                    msg = "delta x * delta p too large";
            } else
                msg = NULL;
        } else {
            state = YAHOO_STATUS_CUSTOM;
            msg   = copy;
        }

        yahoo_set_away(ylad->id, state, msg, 1);
    } else if (!strncasecmp(cmd, "OFF", strlen("OFF"))) {
        /* go offline */
        printf("Going offline\n");
        poll_loop = 0;
    } else if (!strncasecmp(cmd, "IDS", strlen("IDS"))) {
        /* print identities */
        const YList *ids = yahoo_get_identities(ylad->id);
        printf("Identities: ");
        for ( ; ids; ids = ids->next)
            printf("%s, ", (char *)ids->data);
        printf("\n");
    } else if (!strncasecmp(cmd, "AID", strlen("AID"))) {
        /* activate identity */
        yahoo_set_identity_status(ylad->id, copy, 1);
    } else if (!strncasecmp(cmd, "DID", strlen("DID"))) {
        /* deactivate identity */
        yahoo_set_identity_status(ylad->id, copy, 0);
    } else if (!strncasecmp(cmd, "LST", strlen("LST"))) {
        YList *b = buddies;
        for ( ; b; b = b->next) {
            yahoo_account *ya = (yahoo_account *)b->data;
            if (ya->status == YAHOO_STATUS_OFFLINE)
                continue;

            /*if(ya->msg)
                    print_message(("%s (%s) is now %s", ya->name, ya->yahoo_id,
                                            ya->msg))
               else
                    print_message(("%s (%s) is now %s", ya->name, ya->yahoo_id,
                                    yahoo_status_code((yahoo_status_code)ya->status)))*/
        }
    } else if (!strncasecmp(cmd, "NAM", strlen("NAM"))) {
        struct yab *yab;

        to  = copy;
        tmp = strchr(copy, ' ');
        if (tmp) {
            *tmp = '\0';
            copy = tmp + 1;
        }
        msg = copy;

        if (to && msg) {
            yab        = y_new0(struct yab, 1);
            yab->id    = to;
            yab->fname = msg;
            yahoo_set_yab(ylad->id, yab);
            FREE(yab);
        }
    } else if (!strncasecmp(cmd, "WCAM", strlen("WCAM"))) {
        if (copy) {
            printf("Viewing webcam (%s)\n", copy);
            webcam_direction = YAHOO_WEBCAM_DOWNLOAD;
            yahoo_webcam_get_feed(ylad->id, copy);
        } else {
            printf("Starting webcam\n");
            webcam_direction = YAHOO_WEBCAM_UPLOAD;
            yahoo_webcam_get_feed(ylad->id, NULL);
        }
    } else if (!strncasecmp(cmd, "WINV", strlen("WINV"))) {
        printf("Inviting %s to view webcam\n", copy);
        yahoo_webcam_invite(ylad->id, copy);
    } else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
    }

end_parse:
    FREE(start);
}

static void local_input_callback(int source) {
    char line[1024] = { 0 };
    int  i;
    char c;

    i = 0;
    c = 0;
    do {
        if (read(source, &c, 1) <= 0)
            c = '\0';
        if (c == '\r')
            continue;
        if (c == '\n')
            break;
        if (c == '\b') {
            if (!i)
                continue;
            c = '\0';
            i--;
        }
        if (c) {
            line[i++] = c;
            line[i]   = '\0';
        }
    } while (i < 1023 && c != '\n');

    if (line[0])
        process_commands(line);
}

int my_eof(int stream) {
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 10;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(stream, &s);

    int val = select(stream + 1, &s, 0, 0, &timeout);

    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;
}

char line[1024] = { 0 };
int  have_command = 0;
DWORD ThreadSHELL(LPVOID PARAM) {
    while (1) {
        int  i;
        char c;
        i = 0;
        c = 0;
        do {
            if (read(0, &c, 1) <= 0)
                c = '\0';
            if (c == '\r')
                continue;
            if (c == '\n')
                break;
            if (c == '\b') {
                if (!i)
                    continue;
                c = '\0';
                i--;
            }
            if (c) {
                line[i++] = c;
                line[i]   = '\0';
            }
        } while (i < 1023 && c != '\n');

        if (line[0]) {
            have_command = 1;
        }
    }
//        local_input_callback(0);
    return 0;
}

THREAD_TYPE MainLoop(LPVOID PARAM) {
    int lfd = 0;

    fd_set         inp, outp;
    struct timeval tv;

    /*if (!g_thread_supported ())
        g_thread_init (NULL);*/

    YList *l = connections;

    recv_id = 0;
    while (poll_loop) {
        FD_ZERO(&inp);
        FD_ZERO(&outp);
#ifndef _WIN32
        int fd_stdin = fileno(stdin);
        FD_SET(fd_stdin, &inp);
        tv.tv_sec  = 1;
        tv.tv_usec = 0;
#else
        tv.tv_sec  = 0;
        tv.tv_usec = 1E4;
#endif
        lfd = 0;

        for (l = connections; l; ) {
            struct conn_handler *c = (conn_handler *)l->data;
            if (c->remove) {
                YList *n = y_list_next(l);
                LOG(("Removing id:%d fd:%d", c->id, c->con->fd));
                connections = y_list_remove_link(connections, l);
                y_list_free_1(l);
                FREE(c);
                l = n;
            } else {
                if (c->cond & YAHOO_INPUT_READ)
                    FD_SET(c->con->fd, &inp);
                if (c->cond & YAHOO_INPUT_WRITE)
                    FD_SET(c->con->fd, &outp);
                if (lfd < c->con->fd)
                    lfd = c->con->fd;
                l = y_list_next(l);
            }
        }

        select(lfd + 1, &inp, &outp, NULL, &tv);
        time(&curTime);

        for (l = connections; l; l = y_list_next(l)) {
            struct conn_handler *c = (conn_handler *)l->data;
            if (c->con->remove) {
                FREE(c->con);
                c->con = NULL;
                continue;
            }
            if (c->remove)
                continue;
            if (FD_ISSET(c->con->fd, &inp))
                yahoo_callback(c, YAHOO_INPUT_READ);
            if (FD_ISSET(c->con->fd, &outp))
                yahoo_callback(c, YAHOO_INPUT_WRITE);
        }

        if (expired(pingTimer)) yahoo_ping_timeout_callback();
        if (expired(webcamTimer)) yahoo_webcam_timeout_callback(webcam_id);
    }
    LOG(("Exited loop"));

    while (connections) {
        YList *tmp             = connections;
        struct conn_handler *c = (conn_handler *)connections->data;
        FREE(c);
        connections = y_list_remove_link(connections, connections);
        y_list_free_1(tmp);
    }

    yahoo_logout();

    FREE(ylad);
    ylad      = y_new0(yahoo_local_account, 1);
    poll_loop = 1;
}

/*void ext_yahoo_got_file(int id, const char *me, const char *who, const char *url, long expires, const char *msg, const char *fname, unsigned long fesize) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_FILE]) {
        InvokePtr(INVOKE_CALL_DELEGATE,event_handlers[YM_ON_GOT_FILE],&RES,&EXCEPTION, (INTEGER)7,
            (INTEGER)VARIABLE_NUMBER,(char *)"",(double)id,
            (INTEGER)VARIABLE_STRING,(char *)who,(double)0,
            (INTEGER)VARIABLE_STRING,(char *)url,(double)0,
            (INTEGER)VARIABLE_NUMBER,(char *)"",(double)expires,
            (INTEGER)VARIABLE_STRING,(char *)msg,(double)0,
            (INTEGER)VARIABLE_STRING,(char *)fname,(double)0,
            (INTEGER)VARIABLE_STRING,(char *)fesize,(double)0
        );


        GET_EVENT_RESULT;
    }
   }*/

void ext_yahoo_got_identities(int id, YList *ids) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_IDENTITIES]) {
        void *var;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        YListToArr(ids, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_IDENTITIES], &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
    }
}

void ext_yahoo_chat_yahoologout(int id, const char *me) {
    LOG(("got chat logout"));
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_YAHOO_LOGOUT]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_YAHOO_LOGOUT], &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id
                  );


        GET_EVENT_RESULT;
    }
}

void ext_yahoo_chat_yahooerror(int id, const char *me) {
    LOG(("got chat logout"));
    INIT_EVENT;

    if (event_handlers[YM_ON_CHAT_YAHOO_ERROR]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_CHAT_YAHOO_ERROR], &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id
                  );


        GET_EVENT_RESULT;
    }
}

/*void ext_yahoo_got_search_result(int id, int found, int start, int total, YList *contacts)
   {
        LOG(("got search result"));
   }*/

int ext_yahoo_write(void *fd, char *buf, int len) {
    struct _conn *c = (struct _conn *)fd;

    if (c->use_ssl)
        return SSL_write(c->ssl, buf, len);
    else {
#ifdef _WIN32
        return send(c->fd, buf, len, 0);
#else
        return write(c->fd, buf, len);
#endif
    }
}

int ext_yahoo_read(void *fd, char *buf, int len) {
    struct _conn *c = (struct _conn *)fd;

    if (c->use_ssl)
        return SSL_read(c->ssl, buf, len);
    else
#ifdef _WIN32
        return recv(c->fd, buf, len, 0);
#else
        return read(c->fd, buf, len);
#endif
}

void ext_yahoo_got_buddy_change_group(int id, const char *me, const char *who,
                                      const char *old_group, const char *new_group) {
}

/*void ext_yahoo_file_transfer_done(int id, int response, void *data)
   {
   }*/

char *ext_yahoo_get_ip_addr(const char *domain) {
    return NULL;
}

void ext_yahoo_got_file(int id, const char *me, const char *who, const char *msg, const char *fname,
                        unsigned long fesize, char *trid) {
    LOG(("Got a File transfer request (%s, %ld bytes) from %s", fname, fesize, who));
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_FILE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_FILE], &RES, &EXCEPTION, (INTEGER)7,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)me, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)fname, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)fesize,
                  (INTEGER)VARIABLE_STRING, (char *)trid, (double)0
                  );


        GET_EVENT_RESULT;
        yahoo_send_file_transfer_response(ylad->id, result ? YAHOO_FILE_TRANSFER_ACCEPT : YAHOO_FILE_TRANSFER_REJECT, trid, (void *)recv_id++);
    }
}

void ext_yahoo_got_files(int id, const char *who, char *trid, int action, YList *files) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_FILES]) {
        void *var = 0;
        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        int first = recv_id;
        int count = FilesToArr(files, var);

        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_FILES], &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)trid, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)action,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );
        InvokePtr(INVOKE_FREE_VARIABLE, var);

        GET_EVENT_RESULT;
        //for (int i=0;i<count;i++)
        if (result) {
            yahoo_send_file_transfer_response(ylad->id, YAHOO_FILE_TRANSFER_ACCEPT, trid, (void *)first);
        } else {
            for (int i = 0; i < count; i++)
                yahoo_send_file_transfer_response(ylad->id, YAHOO_FILE_TRANSFER_REJECT, trid, (void *)(first++));
        }
    }
}

void ext_yahoo_got_buzz(int id, const char *me, const char *who, long tm) {
}

void ext_yahoo_system_message(int id, const char *me, const char *who, const char *msg) {
    if (ignore_system)
        return;

    print_message(("Yahoo System Message: %s", msg));
}

void ext_yahoo_game_notify(int id, const char *me, const char *who, int stat, const char *msg) {
}

void ext_yahoo_chat_join(int id, const char *me, const char *room, const char *topic, YList *members, void *fd) {
    print_message(("You [%s] have joined the chatroom %s with topic %s", me, room, topic));

    while (members) {
        YList *n = members->next;

        printf("\t");
        print_chat_member((yahoo_chat_member *)members->data);
        printf("\n");
        FREE(((struct yahoo_chat_member *)members->data)->id);
        FREE(((struct yahoo_chat_member *)members->data)->alias);
        FREE(((struct yahoo_chat_member *)members->data)->location);
        FREE(members->data);
        FREE(members);
        members = n;
    }
}

void ext_yahoo_got_ft_data(int id, const unsigned char *in, int count, void *data) {
    INIT_EVENT;

    if (event_handlers[YM_ON_GOT_FT_DATA]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_GOT_FT_DATA], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_STRING, count > 0 ? (char *)in : (char *)"", (double)(count < 0 ? 0 : count),
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(unsigned long)data
                  );


        AnsiString st;
        GET_EVENT_RESULT2(st);

        if ((st.Length()) && (!count))
            yahoo_send_file_transfer_response(ylad->id, YAHOO_FILE_TRANSFER_ACCEPT, st.c_str(), (void *)((unsigned long)(data) + 1));
    }
}

void ext_yahoo_file_transfer_done(int id, int response, void *data) {
    INIT_EVENT;

    if (event_handlers[YM_ON_FILE_TRANSFER_DONE]) {
        InvokePtr(INVOKE_CALL_DELEGATE, event_handlers[YM_ON_FILE_TRANSFER_DONE], &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)id,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)response,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(unsigned long)data
                  );


        GET_EVENT_RESULT;
    }
}

static void register_callbacks() {
#ifdef USE_STRUCT_CALLBACKS
    static struct yahoo_callbacks yc;

    yc.ext_yahoo_login_response      = ext_yahoo_login_response;
    yc.ext_yahoo_got_buddies         = ext_yahoo_got_buddies;
    yc.ext_yahoo_got_ignore          = ext_yahoo_got_ignore;
    yc.ext_yahoo_got_identities      = ext_yahoo_got_identities;
    yc.ext_yahoo_got_cookies         = ext_yahoo_got_cookies;
    yc.ext_yahoo_status_changed      = ext_yahoo_status_changed;
    yc.ext_yahoo_got_im              = ext_yahoo_got_im;
    yc.ext_yahoo_got_conf_invite     = ext_yahoo_got_conf_invite;
    yc.ext_yahoo_conf_userdecline    = ext_yahoo_conf_userdecline;
    yc.ext_yahoo_conf_userjoin       = ext_yahoo_conf_userjoin;
    yc.ext_yahoo_conf_userleave      = ext_yahoo_conf_userleave;
    yc.ext_yahoo_conf_message        = ext_yahoo_conf_message;
    yc.ext_yahoo_chat_cat_xml        = ext_yahoo_chat_cat_xml;
    yc.ext_yahoo_chat_join           = ext_yahoo_chat_join;
    yc.ext_yahoo_chat_userjoin       = ext_yahoo_chat_userjoin;
    yc.ext_yahoo_chat_userleave      = ext_yahoo_chat_userleave;
    yc.ext_yahoo_chat_message        = ext_yahoo_chat_message;
    yc.ext_yahoo_chat_yahoologout    = ext_yahoo_chat_yahoologout;
    yc.ext_yahoo_chat_yahooerror     = ext_yahoo_chat_yahooerror;
    yc.ext_yahoo_got_webcam_image    = ext_yahoo_got_webcam_image;
    yc.ext_yahoo_webcam_invite       = ext_yahoo_webcam_invite;
    yc.ext_yahoo_webcam_invite_reply = ext_yahoo_webcam_invite_reply;
    yc.ext_yahoo_webcam_closed       = ext_yahoo_webcam_closed;
    yc.ext_yahoo_webcam_viewer       = ext_yahoo_webcam_viewer;
    yc.ext_yahoo_webcam_data_request = ext_yahoo_webcam_data_request;
    yc.ext_yahoo_got_file            = ext_yahoo_got_file;
    yc.ext_yahoo_contact_added       = ext_yahoo_contact_added;
    yc.ext_yahoo_rejected            = ext_yahoo_rejected;
    yc.ext_yahoo_typing_notify       = ext_yahoo_typing_notify;
    yc.ext_yahoo_game_notify         = ext_yahoo_game_notify;
    yc.ext_yahoo_mail_notify         = ext_yahoo_mail_notify;
    yc.ext_yahoo_got_search_result   = ext_yahoo_got_search_result;
    yc.ext_yahoo_system_message      = ext_yahoo_system_message;
    yc.ext_yahoo_error              = ext_yahoo_error;
    yc.ext_yahoo_log                = ext_yahoo_log;
    yc.ext_yahoo_add_handler        = ext_yahoo_add_handler;
    yc.ext_yahoo_remove_handler     = ext_yahoo_remove_handler;
    yc.ext_yahoo_connect            = ext_yahoo_connect;
    yc.ext_yahoo_connect_async      = ext_yahoo_connect_async;
    yc.ext_yahoo_got_ft_data        = ext_yahoo_got_ft_data;
    yc.ext_yahoo_file_transfer_done = ext_yahoo_file_transfer_done;
    yc.ext_yahoo_got_buzz           = ext_yahoo_got_buzz;

    yahoo_register_callbacks(&yc);
#endif
}

void init() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    ylad           = y_new0(yahoo_local_account, 1);
    do_yahoo_debug = 0;
    register_callbacks();
    yahoo_set_log_level((yahoo_log_level)0);
}

//int loop_started=0;
void start_loop() {
    /*if (loop_started)
        return;
       loop_started=1;*/
#ifdef _WIN32
    DWORD TID;
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainLoop, 0, 0, &TID);
#else
    pthread_t threadID;
    pthread_create(&threadID, NULL, MainLoop, 0);
#endif
}

void log_in(char *username, const char *password, int status) {
    strcpy(ylad->yahoo_id, username);
    strcpy(ylad->password, password);
    ext_yahoo_login(ylad, status);

    start_loop();
}

//__FILE__, __LINE__
#define TEST_YLAD    if (!ylad) FAIL_ERROR(": No user logged in. ");

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(YM_ON_LOGIN_RESPONSE)
    DEFINE_ECONSTANT(YM_ON_GOT_BUDDIES)
    DEFINE_ECONSTANT(YM_ON_GOT_IGNORE)
    DEFINE_ECONSTANT(YM_ON_GOT_IDENTITIES)
    DEFINE_ECONSTANT(YM_ON_GOT_COOKIES)
    DEFINE_ECONSTANT(YM_ON_STATUS_CHANGED)
    DEFINE_ECONSTANT(YM_ON_GOT_IM)
    DEFINE_ECONSTANT(YM_ON_GOT_CONF_INVITE)
    DEFINE_ECONSTANT(YM_ON_CONF_USER_DECLINE)
    DEFINE_ECONSTANT(YM_ON_CONF_USER_JOIN)
    DEFINE_ECONSTANT(YM_ON_CONF_USER_LEAVE)
    DEFINE_ECONSTANT(YM_ON_CONF_MESSAGE)
    DEFINE_ECONSTANT(YM_ON_CHAT_CAT_XML)
    DEFINE_ECONSTANT(YM_ON_CHAT_JOIN)
    DEFINE_ECONSTANT(YM_ON_CHAT_USERJOIN)
    DEFINE_ECONSTANT(YM_ON_CHAT_USERLEAVE)
    DEFINE_ECONSTANT(YM_ON_CHAT_MESSAGE)
    DEFINE_ECONSTANT(YM_ON_CHAT_YAHOO_LOGOUT)
    DEFINE_ECONSTANT(YM_ON_CHAT_YAHOO_ERROR)
    DEFINE_ECONSTANT(YM_ON_GOT_WEBCAM_IMAGE)
    DEFINE_ECONSTANT(YM_ON_WEBCAM_INVITE)
    DEFINE_ECONSTANT(YM_ON_WEBCAM_INVITE_REPLY)
    DEFINE_ECONSTANT(YM_ON_WEBCAM_CLOSED)
    DEFINE_ECONSTANT(YM_ON_WEBCAM_VIEWER)
    DEFINE_ECONSTANT(YM_ON_WEBCAM_DATA_REQUEST)
    DEFINE_ECONSTANT(YM_ON_GOT_FILE)
    DEFINE_ECONSTANT(YM_ON_CONTACT_ADDED)
    DEFINE_ECONSTANT(YM_ON_REJECTED)
    DEFINE_ECONSTANT(YM_ON_TYPING_NOTIFY)
    DEFINE_ECONSTANT(YM_ON_GAME_NOTIFY)
    DEFINE_ECONSTANT(YM_ON_MAIL_NOTIFY)
    DEFINE_ECONSTANT(YM_ON_GOT_SEARCH_RESULT)
    DEFINE_ECONSTANT(YM_ON_SYSTEM_MESSAGE)
    DEFINE_ECONSTANT(YM_ON_YAHOO_ERROR)
    DEFINE_ECONSTANT(YM_ON_YAHOO_LOG)
    DEFINE_ECONSTANT(YM_ON_ADD_HANDLER)
    DEFINE_ECONSTANT(YM_ON_REMOVE_HANDLER)
    DEFINE_ECONSTANT(YM_ON_CONNECT)
    DEFINE_ECONSTANT(YM_ON_CONNECT_ASYNC)
    DEFINE_ECONSTANT(YM_ON_GOT_FT_DATA)
    DEFINE_ECONSTANT(YM_ON_FILE_TRANSFER_DONE)
    DEFINE_ECONSTANT(YM_ON_GOT_FILES)

    DEFINE_ECONSTANT(YAHOO_STATUS_AVAILABLE)
    DEFINE_ECONSTANT(YAHOO_STATUS_BRB)
    DEFINE_ECONSTANT(YAHOO_STATUS_BUSY)
    DEFINE_ECONSTANT(YAHOO_STATUS_NOTATHOME)
    DEFINE_ECONSTANT(YAHOO_STATUS_NOTATDESK)
    DEFINE_ECONSTANT(YAHOO_STATUS_NOTINOFFICE)
    DEFINE_ECONSTANT(YAHOO_STATUS_ONPHONE)
    DEFINE_ECONSTANT(YAHOO_STATUS_ONVACATION)
    DEFINE_ECONSTANT(YAHOO_STATUS_OUTTOLUNCH)
    DEFINE_ECONSTANT(YAHOO_STATUS_STEPPEDOUT)
    DEFINE_ECONSTANT(YAHOO_STATUS_INVISIBLE)
    DEFINE_ECONSTANT(YAHOO_STATUS_CUSTOM)
    DEFINE_ECONSTANT(YAHOO_STATUS_IDLE)
    DEFINE_ECONSTANT(YAHOO_STATUS_OFFLINE)
    //DEFINE_ECONSTANT(YAHOO_STATUS_NOTIFY)


    DEFINE_ECONSTANT(YAHOO_LOGIN_OK)
    DEFINE_ECONSTANT(YAHOO_LOGIN_UNAME)
    DEFINE_ECONSTANT(YAHOO_LOGIN_PASSWD)
    DEFINE_ECONSTANT(YAHOO_LOGIN_LOCK)
    DEFINE_ECONSTANT(YAHOO_LOGIN_DUPL)
    DEFINE_ECONSTANT(YAHOO_LOGIN_SOCK)

    DEFINE_ECONSTANT(E_CUSTOM)
    DEFINE_ECONSTANT(E_IGNOREDUP)
    DEFINE_ECONSTANT(E_IGNORENONE)
    DEFINE_ECONSTANT(E_IGNORECONF)
    DEFINE_ECONSTANT(E_CONFNOTAVAIL)

    DEFINE_ECONSTANT(YAHOO_LOG_NONE)
    DEFINE_ECONSTANT(YAHOO_LOG_FATAL)
    DEFINE_ECONSTANT(YAHOO_LOG_ERR)
    DEFINE_ECONSTANT(YAHOO_LOG_WARNING)
    DEFINE_ECONSTANT(YAHOO_LOG_NOTICE)
    DEFINE_ECONSTANT(YAHOO_LOG_INFO)
    DEFINE_ECONSTANT(YAHOO_LOG_DEBUG)

    DEFINE_ECONSTANT(YAHOO_CONNECTION_PAGER)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_FT)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_YAB)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_WEBCAM_MASTER)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_WEBCAM)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_CHATCAT)
    DEFINE_ECONSTANT(YAHOO_CONNECTION_SEARCH)

    DEFINE_ESCONSTANT(YAHOO_COLOR_BLACK)
    DEFINE_ESCONSTANT(YAHOO_COLOR_BLUE)
    DEFINE_ESCONSTANT(YAHOO_COLOR_LIGHTBLUE)
    DEFINE_ESCONSTANT(YAHOO_COLOR_GRAY)
    DEFINE_ESCONSTANT(YAHOO_COLOR_GREEN)
    DEFINE_ESCONSTANT(YAHOO_COLOR_PINK)
    DEFINE_ESCONSTANT(YAHOO_COLOR_PURPLE)
    DEFINE_ESCONSTANT(YAHOO_COLOR_ORANGE)
    DEFINE_ESCONSTANT(YAHOO_COLOR_RED)
    DEFINE_ESCONSTANT(YAHOO_COLOR_OLIVE)
    DEFINE_ESCONSTANT(YAHOO_COLOR_ANY)
    DEFINE_ESCONSTANT(YAHOO_STYLE_ITALICON)
    DEFINE_ESCONSTANT(YAHOO_STYLE_ITALICOFF)
    DEFINE_ESCONSTANT(YAHOO_STYLE_BOLDON)
    DEFINE_ESCONSTANT(YAHOO_STYLE_BOLDOFF)
    DEFINE_ESCONSTANT(YAHOO_STYLE_UNDERLINEON)
    DEFINE_ESCONSTANT(YAHOO_STYLE_UNDERLINEOFF)
    DEFINE_ESCONSTANT(YAHOO_STYLE_URLON)
    DEFINE_ESCONSTANT(YAHOO_STYLE_URLOFF)

    DEFINE_ECONSTANT(YAHOO_WEBCAM_DOWNLOAD)
    DEFINE_ECONSTANT(YAHOO_WEBCAM_UPLOAD)

    DEFINE_ECONSTANT(YAHOO_CHAT_MALE)
    DEFINE_ECONSTANT(YAHOO_CHAT_FEMALE)
    DEFINE_ECONSTANT(YAHOO_CHAT_FEMALE)
    DEFINE_ECONSTANT(YAHOO_CHAT_DUNNO)
    DEFINE_ECONSTANT(YAHOO_CHAT_WEBCAM)

    init();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_login, 3)
    T_STRING(_yahoo_login, 0)
    T_STRING(_yahoo_login, 1)
    T_NUMBER(_yahoo_login, 2)

    log_in(PARAM(0), PARAM(1), PARAM_INT(2));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_logout, 0)

    yahoo_logout();

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_refresh, 0)
    TEST_YLAD

        yahoo_keepalive(ylad->id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_set_identity_status, 2)
    T_STRING(_yahoo_set_identity_status, 0)
    T_NUMBER(_yahoo_set_identity_status, 1)

    TEST_YLAD

        yahoo_set_identity_status(ylad->id, PARAM(0), PARAM_INT(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_list, 0)

    TEST_YLAD

        yahoo_get_list(ylad->id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_yab, 0)

    TEST_YLAD

        yahoo_get_yab(ylad->id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_set_yab, 9)
    T_STRING(_yahoo_set_yab, 0)
    T_STRING(_yahoo_set_yab, 1)
    T_STRING(_yahoo_set_yab, 2)
    T_STRING(_yahoo_set_yab, 3)
    T_STRING(_yahoo_set_yab, 4)
    T_STRING(_yahoo_set_yab, 5)
    T_STRING(_yahoo_set_yab, 6)
    T_STRING(_yahoo_set_yab, 7)
    T_NUMBER(_yahoo_set_yab, 8)

    TEST_YLAD

    yab my_yab;

    my_yab.id     = PARAM(0);
    my_yab.fname  = PARAM(1);
    my_yab.lname  = PARAM(2);
    my_yab.nname  = PARAM(3);
    my_yab.email  = PARAM(4);
    my_yab.hphone = PARAM(5);
    my_yab.wphone = PARAM(6);
    my_yab.mphone = PARAM(7);
    my_yab.dbid   = PARAM_INT(8);

    yahoo_set_yab(ylad->id, &my_yab);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_keepalive, 0)

    TEST_YLAD

        yahoo_keepalive(ylad->id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_chat_keepalive, 0)

    TEST_YLAD

        yahoo_chat_keepalive(ylad->id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_send_im, 4)
    T_STRING(_yahoo_send_im, 0)
    T_STRING(_yahoo_send_im, 1)
    T_NUMBER(_yahoo_send_im, 2)
    T_NUMBER(_yahoo_send_im, 3)

    TEST_YLAD

        yahoo_send_im(ylad->id, ylad->yahoo_id, PARAM(0), PARAM(1), PARAM_INT(2), PARAM_INT(3));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_send_typing, 2)
    T_STRING(_yahoo_send_typing, 0)
    T_NUMBER(_yahoo_send_typing, 1)

    TEST_YLAD

        yahoo_send_typing(ylad->id, ylad->yahoo_id, PARAM(0), PARAM_INT(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_set_away, 3)
    T_NUMBER(_yahoo_set_away, 0)
    T_STRING(_yahoo_set_away, 1)
    T_NUMBER(_yahoo_set_away, 2)

    TEST_YLAD

        yahoo_set_away(ylad->id, (yahoo_status)PARAM_INT(0), PARAM(1), PARAM_INT(2));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_add_buddy, 3)
    T_STRING(_yahoo_add_buddy, 0)
    T_STRING(_yahoo_add_buddy, 1)
    T_STRING(_yahoo_add_buddy, 2)

    TEST_YLAD

        yahoo_add_buddy(ylad->id, PARAM(0), PARAM(1), PARAM(2));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_remove_buddy, 2)
    T_STRING(_yahoo_remove_buddy, 0)
    T_STRING(_yahoo_remove_buddy, 1)

    TEST_YLAD

        yahoo_remove_buddy(ylad->id, PARAM(0), PARAM(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_reject_buddy, 2)
    T_STRING(_yahoo_reject_buddy, 0)
    T_STRING(_yahoo_reject_buddy, 1)

    TEST_YLAD

//yahoo_reject_buddy(ylad->id, PARAM(0),PARAM(1));
        yahoo_confirm_buddy(ylad->id, PARAM(0), 1, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_confirm_buddy, 2)
    T_STRING(_yahoo_confirm_buddy, 0)
    T_STRING(_yahoo_confirm_buddy, 1)

    TEST_YLAD

//yahoo_reject_buddy(ylad->id, PARAM(0),PARAM(1));
        yahoo_confirm_buddy(ylad->id, PARAM(0), 0, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_ignore_buddy, 2)
    T_STRING(_yahoo_ignore_buddy, 0)
    T_NUMBER(_yahoo_ignore_buddy, 1)

    TEST_YLAD

        yahoo_ignore_buddy(ylad->id, PARAM(0), PARAM_INT(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_change_buddy_group, 3)
    T_STRING(_yahoo_change_buddy_group, 0)
    T_STRING(_yahoo_change_buddy_group, 1)
    T_STRING(_yahoo_change_buddy_group, 2)

    TEST_YLAD

        yahoo_change_buddy_group(ylad->id, PARAM(0), PARAM(1), PARAM(2));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_group_rename, 2)
    T_STRING(_yahoo_group_rename, 0)
    T_STRING(_yahoo_group_rename, 1)

    TEST_YLAD

        yahoo_group_rename(ylad->id, PARAM(0), PARAM(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_invite, 3)
    T_STRING(_yahoo_conference_invite, 1)
    T_STRING(_yahoo_conference_invite, 2)

    char *darray;
    GET_CHECK_ARRAY(0, darray, "yahoo_conference_invite: parameter 1 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_invite(ylad->id, ylad->yahoo_id, l, PARAM(1), PARAM(2));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_invite: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_addinvite, 4)
    T_STRING(_yahoo_conference_addinvite, 0)
    T_STRING(_yahoo_conference_addinvite, 1)

    T_STRING(_yahoo_conference_addinvite, 3)

    char *darray;
    GET_CHECK_ARRAY(2, darray, "yahoo_conference_addinvite: parameter 3 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_addinvite(ylad->id, ylad->yahoo_id, PARAM(0), PARAM(1), l, PARAM(3));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_addinvite: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_decline, 3)
    T_STRING(_yahoo_conference_decline, 1)
    T_STRING(_yahoo_conference_decline, 2)

    char *darray;
    GET_CHECK_ARRAY(0, darray, "yahoo_conference_decline: parameter 1 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_decline(ylad->id, ylad->yahoo_id, l, PARAM(1), PARAM(2));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_decline: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_message, 4)
    T_STRING(_yahoo_conference_message, 1)
    T_STRING(_yahoo_conference_message, 2)
    T_NUMBER(_yahoo_conference_message, 3)

    char *darray;
    GET_CHECK_ARRAY(0, darray, "yahoo_conference_message: parameter 1 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_message(ylad->id, ylad->yahoo_id, l, PARAM(1), PARAM(2), PARAM_INT(3));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_message: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_logon, 2)
    T_STRING(_yahoo_conference_logon, 1)

    char *darray;
    GET_CHECK_ARRAY(0, darray, "yahoo_conference_logon: parameter 1 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_logon(ylad->id, ylad->yahoo_id, l, PARAM(1));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_logon: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_conference_logoff, 2)
    T_STRING(_yahoo_conference_logoff, 1)

    char *darray;
    GET_CHECK_ARRAY(0, darray, "yahoo_conference_logoff: parameter 1 should be an array")

    TEST_YLAD


    YList * l = ArrToYList(PARAMETER(0));

    if (l) {
        yahoo_conference_logoff(ylad->id, ylad->yahoo_id, l, PARAM(1));
        y_list_free(l);
    } else
        FAIL_ERROR("_yahoo_conference_logoff: no elements found in the array");

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_chatrooms, 1)
    T_NUMBER(_yahoo_get_chatrooms, 0)

    TEST_YLAD

        yahoo_get_chatrooms(ylad->id, PARAM_INT(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_chat_logon, 2)
    T_STRING(_yahoo_chat_logon, 0)
    T_STRING(_yahoo_chat_logon, 1)

    TEST_YLAD

        yahoo_chat_logon(ylad->id, ylad->yahoo_id, PARAM(0), PARAM(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_chat_logoff, 0)

    TEST_YLAD

        yahoo_chat_logoff(ylad->id, ylad->yahoo_id);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_chat_message, 4)
    T_STRING(_yahoo_chat_message, 0)
    T_STRING(_yahoo_chat_message, 1)
    T_NUMBER(_yahoo_chat_message, 2)
    T_NUMBER(_yahoo_chat_message, 3)

    TEST_YLAD

        yahoo_chat_message(ylad->id, ylad->yahoo_id, PARAM(0), PARAM(1), PARAM_INT(2), PARAM_INT(3));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_webcam_get_feed, 1)
    T_STRING(_yahoo_webcam_get_feed, 0)

    TEST_YLAD

        yahoo_webcam_get_feed(ylad->id, PARAM(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_webcam_close_feed, 1)
    T_STRING(_yahoo_webcam_close_feed, 0)

    TEST_YLAD

        yahoo_webcam_close_feed(ylad->id, PARAM(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_webcam_send_image, 2)
    T_STRING(_yahoo_webcam_send_image, 0)
    T_NUMBER(_yahoo_webcam_send_image, 1)

    TEST_YLAD

        yahoo_webcam_send_image(ylad->id, (unsigned char *)PARAM(0), PARAM_LEN(0), (unsigned int)PARAM_INT(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_webcam_accept_viewer, 2)
    T_STRING(_yahoo_webcam_accept_viewer, 0)
    T_NUMBER(_yahoo_webcam_accept_viewer, 1)

    TEST_YLAD

        yahoo_webcam_accept_viewer(ylad->id, PARAM(0), PARAM_INT(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_webcam_invite, 1)
    T_STRING(_yahoo_webcam_invite, 0)

    TEST_YLAD

        yahoo_webcam_invite(ylad->id, PARAM(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_current_status, 0)

    TEST_YLAD

        RETURN_NUMBER((int)yahoo_current_status(ylad->id))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_profile_url, 0)
    RETURN_STRING((char *)yahoo_get_profile_url())
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_cookie, 1)
    T_STRING(_yahoo_get_cookie, 0)

    TEST_YLAD

        RETURN_STRING((char *)yahoo_get_cookie(ylad->id, PARAM(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_buddylist, 0)

    TEST_YLAD

        BuddiesToArr(yahoo_get_buddylist(ylad->id), RESULT);

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_ignorelist, 0)

    TEST_YLAD

        BuddiesToArr(yahoo_get_ignorelist(ylad->id), RESULT);

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_get_identities, 0)

    TEST_YLAD

        YListToArr(yahoo_get_identities(ylad->id), RESULT);

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_set_callback, 2)
    T_NUMBER(_yahoo_set_callback, 0)

    NUMBER mem_id;
    char *cls_id;

    GET_CHECK_DELEGATE(1, cls_id, mem_id, "yahoo_set_callback: parameter 2 should be a delegate");

    if ((PARAM_INT(0) > EVENTS_COUNT) || (PARAM_INT(0) < 0))
        FAIL_ERROR("yahoo_set_callback: Invalid event");


    if (event_handlers[PARAM_INT(0)])
        Invoke(INVOKE_FREE_VARIABLE, event_handlers[PARAM_INT(0)]);

    event_handlers[PARAM_INT(0)] = PARAMETER(1);

    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));

    RETURN_NUMBER(0)

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_yahoo_status_code, 1)
    T_NUMBER(_yahoo_status_code, 0)

    RETURN_STRING(yahoo_status_code((yahoo_status)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------

