//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <purple.h>

GMainLoop   *loop     = 0;
INVOKE_CALL InvokePtr = 0;

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

#define SAFE_DELEGATE(var)                                            \
    {                                                                 \
        INTEGER type;                                                 \
        char    *sDummy;                                              \
        NUMBER  nDummy;                                               \
        InvokePtr(INVOKE_GET_VARIABLE, var, &type, &sDummy, &nDummy); \
        if (type != VARIABLE_DELEGATE)                                \
            var = 0;                                                  \
        else                                                          \
            InvokePtr(INVOKE_LOCK_VARIABLE, var);                     \
    }
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    DEFINE_ECONSTANT(PURPLE_MESSAGE_SEND)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_RECV)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_SYSTEM)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_AUTO_RESP)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_ACTIVE_ONLY)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_NICK)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_NO_LOG)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_WHISPER)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_ERROR)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_DELAYED)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_RAW)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_IMAGES)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_NOTIFY)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_NO_LINKIFY)
    DEFINE_ECONSTANT(PURPLE_MESSAGE_INVISIBLE)

    DEFINE_ECONSTANT(PURPLE_CBFLAGS_NONE)
    DEFINE_ECONSTANT(PURPLE_CBFLAGS_VOICE)
    DEFINE_ECONSTANT(PURPLE_CBFLAGS_HALFOP)
    DEFINE_ECONSTANT(PURPLE_CBFLAGS_OP)
    DEFINE_ECONSTANT(PURPLE_CBFLAGS_FOUNDER)
    DEFINE_ECONSTANT(PURPLE_CBFLAGS_TYPING)

    DEFINE_ECONSTANT(PURPLE_XFER_UNKNOWN)
    DEFINE_ECONSTANT(PURPLE_XFER_SEND)
    DEFINE_ECONSTANT(PURPLE_XFER_RECEIVE)

    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_UNKNOWN)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_NOT_STARTED)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_ACCEPTED)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_STARTED)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_DONE)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_CANCEL_LOCAL)
    DEFINE_ECONSTANT(PURPLE_XFER_STATUS_CANCEL_REMOTE)

    DEFINE_ECONSTANT(PURPLE_PRESENCE_CONTEXT_UNSET)
    DEFINE_ECONSTANT(PURPLE_PRESENCE_CONTEXT_ACCOUNT)
    DEFINE_ECONSTANT(PURPLE_PRESENCE_CONTEXT_CONV)
    DEFINE_ECONSTANT(PURPLE_PRESENCE_CONTEXT_BUDDY)

    DEFINE_ECONSTANT(PURPLE_STATUS_UNSET)
    DEFINE_ECONSTANT(PURPLE_STATUS_OFFLINE)
    DEFINE_ECONSTANT(PURPLE_STATUS_AVAILABLE)
    DEFINE_ECONSTANT(PURPLE_STATUS_UNAVAILABLE)
    DEFINE_ECONSTANT(PURPLE_STATUS_INVISIBLE)
    DEFINE_ECONSTANT(PURPLE_STATUS_AWAY)
    DEFINE_ECONSTANT(PURPLE_STATUS_EXTENDED_AWAY)
    DEFINE_ECONSTANT(PURPLE_STATUS_MOBILE)
    DEFINE_ECONSTANT(PURPLE_STATUS_TUNE)
    DEFINE_ECONSTANT(PURPLE_STATUS_NUM_PRIMITIVES)

    DEFINE_ECONSTANT(PURPLE_TUNE_ARTIST)
    DEFINE_ECONSTANT(PURPLE_TUNE_TITLE)
    DEFINE_ECONSTANT(PURPLE_TUNE_ALBUM)
    DEFINE_ECONSTANT(PURPLE_TUNE_GENRE)
    DEFINE_ECONSTANT(PURPLE_TUNE_COMMENT)
    DEFINE_ECONSTANT(PURPLE_TUNE_TRACK)
    DEFINE_ECONSTANT(PURPLE_TUNE_TIME)
    DEFINE_ECONSTANT(PURPLE_TUNE_YEAR)
    DEFINE_ECONSTANT(PURPLE_TUNE_URL)
    DEFINE_ECONSTANT(PURPLE_TUNE_FULL)

    DEFINE_ECONSTANT(PURPLE_CONV_TYPE_UNKNOWN)
    DEFINE_ECONSTANT(PURPLE_CONV_TYPE_IM)
    DEFINE_ECONSTANT(PURPLE_CONV_TYPE_CHAT)
    DEFINE_ECONSTANT(PURPLE_CONV_TYPE_MISC)
    DEFINE_ECONSTANT(PURPLE_CONV_TYPE_ANY)
    return 0;
}
//---------------------------------------------------------------------------
static void *PurpleCallback_F(void *gc, gpointer delegate) {
    void *f_result = 0;

    if (delegate) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)gc
                  );
        GET_EVENT_RESULT;
        f_result = (void *)(long)result;
    }
    return f_result;
}

//---------------------------------------------------------------------------
static void *PurpleCallback_F2(void *gc, void *p1, gpointer delegate) {
    void *f_result = 0;

    if (delegate) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)gc,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p1
                  );
        GET_EVENT_RESULT;
        f_result = (void *)(long)result;
    }
    return f_result;
}

//---------------------------------------------------------------------------
static void *PurpleCallback_F3(void *gc, void *p1, void *p2, gpointer delegate) {
    void *f_result = 0;

    if (delegate) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)gc,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p2
                  );
        GET_EVENT_RESULT;
        f_result = (void *)(long)result;
    }
    return f_result;
}

//---------------------------------------------------------------------------
static void *PurpleCallback_F4(void *gc, void *p1, void *p2, void *p3, gpointer delegate) {
    void *f_result = 0;

    if (delegate) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)gc,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)p3
                  );
        GET_EVENT_RESULT;
        f_result = (void *)(long)result;
    }
    return f_result;
}

//---------------------------------------------------------------------------

void *ui_prefs_init_DELEGATE = 0;
void ui_prefs_init(void) {
    if (ui_prefs_init_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, ui_prefs_init_DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
        GET_EVENT_RESULT;
    }
}

void *debug_ui_init_DELEGATE = 0;
void debug_ui_init(void) {
    if (debug_ui_init_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, debug_ui_init_DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
        GET_EVENT_RESULT;
    }
}

void *ui_init_DELEGATE = 0;
void ui_init(void) {
    if (ui_init_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, ui_init_DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
        GET_EVENT_RESULT;
    }
}

void *ui_quit_DELEGATE = 0;
void ui_quit(void) {
    if (ui_quit_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, ui_quit_DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
        GET_EVENT_RESULT;
    }
}

void *get_ui_info_DELEGATE = 0;
GHashTable *get_ui_info(void) {
    if (ui_quit_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, ui_quit_DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
        GET_EVENT_RESULT;
        return (GHashTable *)result;
    }
    return 0;
}

//---------------------------------------------------------------------------
void ListToArr(GList *lst, void *var) {
    InvokePtr(INVOKE_CREATE_ARRAY, var);
    if (!lst)
        return;
    GList   *b;
    INTEGER index = 0;
    for (b = lst; b; b = b->next) {
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, var, index, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)b->data);
        index++;
    }
}

//------------------------------------------------------------------------
void SListToArr(GSList *lst, void *var) {
    InvokePtr(INVOKE_CREATE_ARRAY, var);
    if (!lst)
        return;
    GSList  *b;
    INTEGER index = 0;
    for (b = lst; b; b = b->next) {
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, var, index, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)b->data);
        index++;
    }
}

//------------------------------------------------------------------------
void *create_conversation_DELEGATE = 0;
void create_conversation(PurpleConversation *conv) {
    if (create_conversation_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, create_conversation_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv
                  );
        GET_EVENT_RESULT;
    }
}

void *destroy_conversation_DELEGATE = 0;
void destroy_conversation(PurpleConversation *conv) {
    if (destroy_conversation_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, destroy_conversation_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv
                  );
        GET_EVENT_RESULT;
    }
}

void *write_chat_DELEGATE = 0;
void write_chat(PurpleConversation *conv, const char *who, const char *message, PurpleMessageFlags flags, time_t mtime) {
    if (write_chat_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, write_chat_DELEGATE, &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)message, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)flags,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)mtime
                  );
        GET_EVENT_RESULT;
    }
}

void *write_im_DELEGATE = 0;
void write_im(PurpleConversation *conv, const char *who, const char *message, PurpleMessageFlags flags, time_t mtime) {
    if (write_im_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, write_im_DELEGATE, &RES, &EXCEPTION, (INTEGER)5,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)who, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)message, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)flags,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)mtime
                  );
        GET_EVENT_RESULT;
    }
}

static void *write_conv_DELEGATE = 0;
static void write_conv(PurpleConversation *conv, const char *name, const char *alias, const char *message, PurpleMessageFlags flags, time_t mtime) {
    if (write_conv_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, write_conv_DELEGATE, &RES, &EXCEPTION, (INTEGER)6,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)name ? name : (char *)"", (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)alias ? alias : (char *)"", (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)message ? message : (char *)"", (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)flags,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)mtime
                  );
        GET_EVENT_RESULT;
    }
}

void *chat_add_users_DELEGATE = 0;
void chat_add_users(PurpleConversation *conv, GList *cbuddies, gboolean new_arrivals) {
    if (chat_add_users_DELEGATE) {
        INIT_EVENT;
        void *var = 0;
        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        ListToArr(cbuddies, var);
        InvokePtr(INVOKE_CALL_DELEGATE, chat_add_users_DELEGATE, &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_UNDEFINED, var,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)new_arrivals
                  );
        InvokePtr(INVOKE_FREE_VARIABLE, var);
        GET_EVENT_RESULT;
    }
}

void *chat_rename_user_DELEGATE = 0;
void chat_rename_user(PurpleConversation *conv, const char *old_name, const char *new_name, const char *new_alias) {
    if (chat_rename_user_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, chat_rename_user_DELEGATE, &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)old_name, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)new_name, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)new_alias, (double)0
                  );
        GET_EVENT_RESULT;
    }
}

void *chat_remove_users_DELEGATE = 0;
void chat_remove_users(PurpleConversation *conv, GList *users) {
    if (chat_remove_users_DELEGATE) {
        INIT_EVENT;
        void *var = 0;
        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        ListToArr(users, var);
        InvokePtr(INVOKE_CALL_DELEGATE, chat_remove_users_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );
        InvokePtr(INVOKE_FREE_VARIABLE, var);
        GET_EVENT_RESULT;
    }
}

void *chat_update_user_DELEGATE = 0;
void chat_update_user(PurpleConversation *conv, const char *user) {
    if (chat_rename_user_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, chat_update_user_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)user, (double)0
                  );
        GET_EVENT_RESULT;
    }
}

void *present_DELEGATE = 0;
void present(PurpleConversation *conv) {
    if (present_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, present_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv
                  );
        GET_EVENT_RESULT;
    }
}

void *has_focus_DELEGATE = 0;
gboolean has_focus(PurpleConversation *conv) {
    if (present_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, has_focus_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv
                  );
        GET_EVENT_RESULT;
        return result;
    }
    return FALSE;
}

void *custom_smiley_add_DELEGATE = 0;
gboolean custom_smiley_add(PurpleConversation *conv, const char *smile, gboolean remote) {
    if (custom_smiley_add_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, custom_smiley_add_DELEGATE, &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)smile, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)remote
                  );
        GET_EVENT_RESULT;
        return result;
    }
    return FALSE;
}

void *custom_smiley_write_DELEGATE = 0;
void custom_smiley_write(PurpleConversation *conv, const char *smile, const guchar *data, gsize size) {
    if (custom_smiley_write_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, custom_smiley_write_DELEGATE, &RES, &EXCEPTION, (INTEGER)3,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)smile, (double)0,
                  (INTEGER)VARIABLE_STRING, (char *)data, (double)size
                  );
        GET_EVENT_RESULT;
    }
}

void *custom_smiley_close_DELEGATE = 0;
void custom_smiley_close(PurpleConversation *conv, const char *smile) {
    if (custom_smiley_close_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, custom_smiley_close_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)smile, (double)0
                  );
        GET_EVENT_RESULT;
    }
}

void *send_confirm_DELEGATE = 0;
void send_confirm(PurpleConversation *conv, const char *message) {
    if (send_confirm_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, send_confirm_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)conv,
                  (INTEGER)VARIABLE_STRING, (char *)message, (double)0
                  );
        GET_EVENT_RESULT;
    }
}

static void *new_xfer_DELEGATE = 0;
static void new_xfer(PurpleXfer *xfer) {
    if (new_xfer_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, new_xfer_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

static void *destroy_DELEGATE = 0;
static void destroy(PurpleXfer *xfer) {
    if (destroy_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, destroy_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

static void *add_xfer_DELEGATE = 0;
static void add_xfer(PurpleXfer *xfer) {
    if (add_xfer_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, add_xfer_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

static void *update_progress_DELEGATE = 0;
static void update_progress(PurpleXfer *xfer, double percent) {
    if (update_progress_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, update_progress_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)percent
                  );
        GET_EVENT_RESULT;
    }
}

static void *cancel_local_DELEGATE = 0;
static void cancel_local(PurpleXfer *xfer) {
    if (cancel_local_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, cancel_local_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

static void *cancel_remote_DELEGATE = 0;
static void cancel_remote(PurpleXfer *xfer) {
    if (cancel_remote_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, cancel_remote_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

static void *ui_write_DELEGATE = 0;
static gssize ui_write(PurpleXfer *xfer, const guchar *buffer, gssize size) {
    if (ui_write_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, ui_write_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer,
                  (INTEGER)VARIABLE_STRING, (char *)buffer, (double)size
                  );
        GET_EVENT_RESULT;
        return result;
    }
    return size;
}

static void *ui_read_DELEGATE = 0;
static gssize ui_read(PurpleXfer *xfer, guchar **buffer, gssize size) {
    if (ui_read_DELEGATE) {
        /*INIT_EVENT;
           InvokePtr(INVOKE_CALL_DELEGATE,ui_read_DELEGATE,&RES,&EXCEPTION, (INTEGER)2,
            (INTEGER)VARIABLE_NUMBER,(char *)"",(double)(long)xfer,
            (INTEGER)VARIABLE_STRING,(char *)*buffer, (double)size
           );
           GET_EVENT_RESULT;
           return result;*/

        //TO DO !
    }
    return 0;
}

static void *data_not_sent_DELEGATE = 0;
static void data_not_sent(PurpleXfer *xfer, const guchar *buffer, gsize size) {
    if (data_not_sent_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, data_not_sent_DELEGATE, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer,
                  (INTEGER)VARIABLE_STRING, (char *)buffer, (double)size
                  );
        GET_EVENT_RESULT;
    }
}

PurpleConversationUiOps *InitConversationOps(void *arr_var) {
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "create_conversation", &create_conversation_DELEGATE);
    SAFE_DELEGATE(create_conversation_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "destroy_conversation", &destroy_conversation_DELEGATE);
    SAFE_DELEGATE(destroy_conversation_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "write_chat", &write_chat_DELEGATE);
    SAFE_DELEGATE(write_chat_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "write_im", &write_im_DELEGATE);
    SAFE_DELEGATE(write_im_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "write_conv", &write_conv_DELEGATE);
    SAFE_DELEGATE(write_conv_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "chat_add_users", &chat_add_users_DELEGATE);
    SAFE_DELEGATE(chat_add_users_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "chat_rename_user", &chat_rename_user_DELEGATE);
    SAFE_DELEGATE(chat_rename_user_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "chat_remove_users", &chat_remove_users_DELEGATE);
    SAFE_DELEGATE(chat_remove_users_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "chat_update_user", &chat_update_user_DELEGATE);
    SAFE_DELEGATE(chat_update_user_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "present", &present_DELEGATE);
    SAFE_DELEGATE(present_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "has_focus", &has_focus_DELEGATE);
    SAFE_DELEGATE(has_focus_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "custom_smiley_add", &custom_smiley_add_DELEGATE);
    SAFE_DELEGATE(custom_smiley_add_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "custom_smiley_write", &custom_smiley_write_DELEGATE);
    SAFE_DELEGATE(custom_smiley_write_DELEGATE);

    PurpleConversationUiOps *ops = new PurpleConversationUiOps;
    memset(ops, 0, sizeof(ops));

    ops->create_conversation  = create_conversation_DELEGATE ? create_conversation : 0;
    ops->destroy_conversation = destroy_conversation_DELEGATE ? destroy_conversation : 0;
    ops->write_chat           = write_chat_DELEGATE ? write_chat : 0;
    ops->write_im             = write_im_DELEGATE ? write_im : 0;
    ops->write_conv           = write_conv_DELEGATE ? write_conv : 0;
    ops->chat_add_users       = chat_add_users_DELEGATE ? chat_add_users : 0;
    ops->chat_rename_user     = chat_rename_user_DELEGATE ? chat_rename_user : 0;
    ops->chat_remove_users    = chat_remove_users_DELEGATE ? chat_remove_users : 0;
    ops->chat_update_user     = chat_update_user_DELEGATE ? chat_update_user : 0;
    ops->present             = present_DELEGATE ? present : 0;
    ops->has_focus           = has_focus_DELEGATE ? has_focus : 0;
    ops->custom_smiley_add   = custom_smiley_add_DELEGATE ? custom_smiley_add : 0;
    ops->custom_smiley_write = custom_smiley_write_DELEGATE ? custom_smiley_write : 0;

    return ops;
}

PurpleCoreUiOps *InitCoreOps(void *arr_var) {
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "ui_prefs_init", &ui_prefs_init_DELEGATE);
    SAFE_DELEGATE(ui_prefs_init_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "debug_ui_init", &debug_ui_init_DELEGATE);
    SAFE_DELEGATE(debug_ui_init_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "ui_init", &ui_init_DELEGATE);
    SAFE_DELEGATE(ui_init_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "quit", &ui_quit_DELEGATE);
    SAFE_DELEGATE(ui_quit_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "get_ui_info", &get_ui_info_DELEGATE);
    SAFE_DELEGATE(get_ui_info_DELEGATE);

    PurpleCoreUiOps *ops = new PurpleCoreUiOps;
    memset(ops, 0, sizeof(ops));

    ops->ui_prefs_init = ui_prefs_init_DELEGATE ? ui_prefs_init : 0;
    ops->debug_ui_init = debug_ui_init_DELEGATE ? debug_ui_init : 0;
    ops->ui_init       = ui_init_DELEGATE ? ui_init : 0;
    ops->quit          = ui_quit_DELEGATE ? ui_quit : 0;
    ops->get_ui_info   = get_ui_info_DELEGATE ? get_ui_info : 0;

    return ops;
}

PurpleXferUiOps *InitXferOps(void *arr_var) {
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "new_xfer", &new_xfer_DELEGATE);
    SAFE_DELEGATE(new_xfer_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "destroy", &destroy_DELEGATE);
    SAFE_DELEGATE(destroy_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "add_xfer", &add_xfer_DELEGATE);
    SAFE_DELEGATE(add_xfer_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "update_progress", &update_progress_DELEGATE);
    SAFE_DELEGATE(update_progress_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "cancel_local", &cancel_local_DELEGATE);
    SAFE_DELEGATE(cancel_local_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "cancel_remote", &cancel_remote_DELEGATE);
    SAFE_DELEGATE(cancel_remote_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "ui_write", &ui_write_DELEGATE);
    SAFE_DELEGATE(ui_write_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "ui_read", &ui_read_DELEGATE);
    SAFE_DELEGATE(ui_read_DELEGATE);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, arr_var, "data_not_sent", &data_not_sent_DELEGATE);
    SAFE_DELEGATE(data_not_sent_DELEGATE);

    PurpleXferUiOps *ops = new PurpleXferUiOps;
    memset(ops, 0, sizeof(ops));

    ops->new_xfer        = new_xfer_DELEGATE ? new_xfer : 0;
    ops->destroy         = destroy_DELEGATE ? destroy : 0;
    ops->add_xfer        = add_xfer_DELEGATE ? add_xfer : 0;
    ops->update_progress = update_progress_DELEGATE ? update_progress : 0;
    ops->cancel_local    = cancel_local_DELEGATE ? cancel_local : 0;
    ops->cancel_remote   = cancel_remote_DELEGATE ? cancel_remote : 0;
    ops->ui_write        = ui_write_DELEGATE ? ui_write : 0;
    ops->ui_read         = ui_read_DELEGATE ? ui_read : 0;
    ops->data_not_sent   = data_not_sent_DELEGATE ? data_not_sent : 0;

    return ops;
}

void PurpleAccountRegistration_F(PurpleAccount *, gboolean flag, void *delegate) {
}

void PurpleAccountRequestAuthorization_F(void *delegate) {
}

static void cb_req_pass_ok(void *delegate, PurpleRequestFields *f) {
}

static void cb_req_pass_cancel(void *delegate, PurpleRequestFields *f) {
}

static gboolean add_seconds_F(void *delegate) {
    return TRUE;
}

static void generic_delegate(void *delegate) {
}

void PurpleCertificateVerifiedCallback_F(PurpleCertificateVerificationStatus status, void *delegate) {
}

PurpleCmdRet PurpleCmdRet_F(PurpleConversation *pc, const gchar *c1, gchar **c2, gchar **c3, void *delegate) {
}

void PurpleInput_F(void *delegate, gint i, PurpleInputCondition ic) {
}

void PurpleNetworkListenCallback_F(int i, void *delegate) {
}

static void *PurpleNotifySearchResultsCallback_delegate = 0;
void PurpleNotifySearchResultsCallback_F(PurpleConnection *pc, GList *lst, void *delegate) {
}

void PurplePrefCallback_F(const char *s, PurplePrefType pt, const void *cv, void *delegate) {
}

void PurpleProxyConnectFunction_F(void *delegate, gint i, const gchar *s) {
}

//------------------------------------------------------------------------
#define PURPLE_GLIB_READ_COND     (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_GLIB_WRITE_COND    (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)

typedef struct _PurpleGLibIOClosure {
    PurpleInputFunction function;
    guint               result;
    gpointer            data;
} PurpleGLibIOClosure;

static void purple_glib_io_destroy(gpointer data) {
    g_free(data);
}

static gboolean purple_glib_io_invoke(GIOChannel *source, GIOCondition condition, gpointer data) {
    PurpleGLibIOClosure *closure = (PurpleGLibIOClosure *)data;
    int purple_cond = 0;

    if (condition & PURPLE_GLIB_READ_COND)
        purple_cond |= PURPLE_INPUT_READ;
    if (condition & PURPLE_GLIB_WRITE_COND)
        purple_cond |= PURPLE_INPUT_WRITE;

    closure->function(closure->data, g_io_channel_unix_get_fd(source),
                      (PurpleInputCondition)purple_cond);

    return TRUE;
}

static guint glib_input_add(gint fd, PurpleInputCondition condition, PurpleInputFunction function,
                            gpointer data) {
    PurpleGLibIOClosure *closure = g_new0(PurpleGLibIOClosure, 1);
    GIOChannel          *channel;
    int cond = 0;

    closure->function = function;
    closure->data     = data;

    if (condition & PURPLE_INPUT_READ)
        cond |= PURPLE_GLIB_READ_COND;
    if (condition & PURPLE_INPUT_WRITE)
        cond |= PURPLE_GLIB_WRITE_COND;

    channel         = g_io_channel_unix_new(fd);
    closure->result = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, (GIOCondition)cond,
                                          purple_glib_io_invoke, closure, purple_glib_io_destroy);

    g_io_channel_unref(channel);
    return closure->result;
}

static PurpleEventLoopUiOps glib_eventloops =
{
    g_timeout_add,
    g_source_remove,
    glib_input_add,
    g_source_remove,
    NULL,
#if GLIB_CHECK_VERSION(2,  14, 0)
    g_timeout_add_seconds,
#else
    NULL,
#endif

    /* padding */
    NULL,
    NULL,
    NULL
};

//------------------------------------------------------------------------
static void *request_denied_DELEGATE = 0;
static void request_denied_fnc(PurpleXfer *xfer) {
    if (request_denied_DELEGATE) {
        INIT_EVENT;
        InvokePtr(INVOKE_CALL_DELEGATE, request_denied_DELEGATE, &RES, &EXCEPTION, (INTEGER)1,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)xfer
                  );
        GET_EVENT_RESULT;
    }
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PURPLE_STRING, 1)
    T_NUMBER(PURPLE_STRING, 0)

    RETURN_STRING((char *)PARAM_INT(0))
END_IMPL
//------------------------------------------------------------------------

CONCEPT_FUNCTION_IMPL(purple_xfer_set_request_denied_fnc, 2)
    T_NUMBER(purple_xfer_set_request_denied_fnc, 0)     // PurpleXfer*
    T_DELEGATE(purple_xfer_set_request_denied_fnc, 1)   // void*

    request_denied_DELEGATE = PARAMETER(0);

    purple_xfer_set_request_denied_fnc((PurpleXfer *)(long)PARAM(0), request_denied_fnc);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PURPLE_CONV_IM, 1)
    T_HANDLE(PURPLE_CONV_IM, 0)

    RETURN_NUMBER((long )PURPLE_CONV_IM((PurpleConversation *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_debug_set_enabled, 1)
    T_NUMBER(purple_debug_set_enabled, 0)
    purple_debug_set_enabled((gboolean)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_loop, 0)
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_quit, 0)
    if (loop) {
        g_main_loop_quit(loop);
        loop = 0;
    }

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_list_to_array, 0)
    T_HANDLE(purple_list_to_array, 0)
    ListToArr((GList *)PARAM_INT(0), RESULT);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_new, 2)
    T_STRING(purple_account_new, 0)     // char*
    T_STRING(purple_account_new, 1)     // char*

    RETURN_NUMBER((long)purple_account_new(PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_destroy, 1)
    T_NUMBER(purple_account_destroy, 0)     // PurpleAccount*

    purple_account_destroy((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_connect, 1)
    T_NUMBER(purple_account_connect, 0)     // PurpleAccount*

    purple_account_connect((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_register_callback, 2)
    T_NUMBER(purple_account_set_register_callback, 0)     // PurpleAccount*
    T_DELEGATE(purple_account_set_register_callback, 1)   // PurpleAccountRegistrationCb

    purple_account_set_register_callback((PurpleAccount *)(long)PARAM(0), PurpleAccountRegistration_F, (void *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_register, 1)
    T_NUMBER(purple_account_register, 0)     // PurpleAccount*

    purple_account_register((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_unregister, 2)
    T_NUMBER(purple_account_unregister, 0)     // PurpleAccount*
    T_DELEGATE(purple_account_unregister, 1)   // PurpleAccountUnregistrationCb

    purple_account_unregister((PurpleAccount *)(long)PARAM(0), PurpleAccountRegistration_F, (void *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_disconnect, 1)
    T_NUMBER(purple_account_disconnect, 0)     // PurpleAccount*

    purple_account_disconnect((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_notify_added, 5)
    T_NUMBER(purple_account_notify_added, 0)     // PurpleAccount*
    T_STRING(purple_account_notify_added, 1)     // char*
    T_STRING(purple_account_notify_added, 2)     // char*
    T_STRING(purple_account_notify_added, 3)     // char*
    T_STRING(purple_account_notify_added, 4)     // char*

    purple_account_notify_added((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3), PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_add, 5)
    T_NUMBER(purple_account_request_add, 0)     // PurpleAccount*
    T_STRING(purple_account_request_add, 1)     // char*
    T_STRING(purple_account_request_add, 2)     // char*
    T_STRING(purple_account_request_add, 3)     // char*
    T_STRING(purple_account_request_add, 4)     // char*

    purple_account_request_add((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3), PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_authorization, 8)
    T_NUMBER(purple_account_request_authorization, 0)     // PurpleAccount*
    T_STRING(purple_account_request_authorization, 1)     // char*
    T_STRING(purple_account_request_authorization, 2)     // char*
    T_STRING(purple_account_request_authorization, 3)     // char*
    T_STRING(purple_account_request_authorization, 4)     // char*
    T_NUMBER(purple_account_request_authorization, 5)     // int
    T_DELEGATE(purple_account_request_authorization, 6)   // PurpleAccountRequestAuthorizationCb
    T_DELEGATE(purple_account_request_authorization, 7)   // PurpleAccountRequestAuthorizationCb
//T_NUMBER(8) // void*

    RETURN_NUMBER((long)purple_account_request_authorization((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3), PARAM(4), (int)PARAM(5), PurpleAccountRequestAuthorization_F, PurpleAccountRequestAuthorization_F, (void *)PARAM(6)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_close_with_account, 1)
    T_NUMBER(purple_account_request_close_with_account, 0)     // PurpleAccount*

    purple_account_request_close_with_account((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_close, 1)
    T_NUMBER(purple_account_request_close, 0)     // void*

    purple_account_request_close((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_password, 2)
    T_NUMBER(purple_account_request_password, 0)     // PurpleAccount*
    T_DELEGATE(purple_account_request_password, 1)   // void*

    purple_account_request_password((PurpleAccount *)(long)PARAM(0), G_CALLBACK(cb_req_pass_ok), G_CALLBACK(cb_req_pass_cancel), (void *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_change_password, 1)
    T_NUMBER(purple_account_request_change_password, 0)     // PurpleAccount*

    purple_account_request_change_password((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_request_change_user_info, 1)
    T_NUMBER(purple_account_request_change_user_info, 0)     // PurpleAccount*

    purple_account_request_change_user_info((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_password, 2)
    T_NUMBER(purple_account_set_password, 0)     // PurpleAccount*
    T_STRING(purple_account_set_password, 1)     // char*

    purple_account_set_password((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_alias, 2)
    T_NUMBER(purple_account_set_alias, 0)     // PurpleAccount*
    T_STRING(purple_account_set_alias, 1)     // char*

    purple_account_set_alias((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_user_info, 2)
    T_NUMBER(purple_account_set_user_info, 0)     // PurpleAccount*
    T_STRING(purple_account_set_user_info, 1)     // char*

    purple_account_set_user_info((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_buddy_icon_path, 2)
    T_NUMBER(purple_account_set_buddy_icon_path, 0)     // PurpleAccount*
    T_STRING(purple_account_set_buddy_icon_path, 1)     // char*

    purple_account_set_buddy_icon_path((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_protocol_id, 2)
    T_NUMBER(purple_account_set_protocol_id, 0)     // PurpleAccount*
    T_STRING(purple_account_set_protocol_id, 1)     // char*

    purple_account_set_protocol_id((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_connection, 2)
    T_NUMBER(purple_account_set_connection, 0)     // PurpleAccount*
    T_NUMBER(purple_account_set_connection, 1)     // PurpleConnection*

    purple_account_set_connection((PurpleAccount *)(long)PARAM(0), (PurpleConnection *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_remember_password, 2)
    T_NUMBER(purple_account_set_remember_password, 0)     // PurpleAccount*
    T_NUMBER(purple_account_set_remember_password, 1)     // int

    purple_account_set_remember_password((PurpleAccount *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_check_mail, 2)
    T_NUMBER(purple_account_set_check_mail, 0)     // PurpleAccount*
    T_NUMBER(purple_account_set_check_mail, 1)     // int

    purple_account_set_check_mail((PurpleAccount *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_enabled, 3)
    T_NUMBER(purple_account_set_enabled, 0)     // PurpleAccount*
    T_STRING(purple_account_set_enabled, 1)     // char*
    T_NUMBER(purple_account_set_enabled, 2)     // int

    purple_account_set_enabled((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_proxy_info, 2)
    T_NUMBER(purple_account_set_proxy_info, 0)     // PurpleAccount*
    T_NUMBER(purple_account_set_proxy_info, 1)     // PurpleProxyInfo*

    purple_account_set_proxy_info((PurpleAccount *)(long)PARAM(0), (PurpleProxyInfo *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_status_types, 2)
    T_NUMBER(purple_account_set_status_types, 0)     // PurpleAccount*
    T_NUMBER(purple_account_set_status_types, 1)     // GList*

    purple_account_set_status_types((PurpleAccount *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_status, 3)
    T_NUMBER(purple_account_set_status, 0)     // PurpleAccount*
    T_STRING(purple_account_set_status, 1)     // char*
    T_NUMBER(purple_account_set_status, 2)     // int

    purple_account_set_status((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_status_list, 4)
    T_NUMBER(purple_account_set_status_list, 0)     // PurpleAccount*
    T_STRING(purple_account_set_status_list, 1)     // char*
    T_NUMBER(purple_account_set_status_list, 2)     // int
    T_NUMBER(purple_account_set_status_list, 3)     // GList*

    purple_account_set_status_list((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2), (GList *)(long)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_clear_settings, 1)
    T_NUMBER(purple_account_clear_settings, 0)     // PurpleAccount*

    purple_account_clear_settings((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_remove_setting, 2)
    T_NUMBER(purple_account_remove_setting, 0)     // PurpleAccount*
    T_STRING(purple_account_remove_setting, 1)     // char*

    purple_account_remove_setting((PurpleAccount *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_int, 3)
    T_NUMBER(purple_account_set_int, 0)     // PurpleAccount*
    T_STRING(purple_account_set_int, 1)     // char*
    T_NUMBER(purple_account_set_int, 2)     // int

    purple_account_set_int((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_string, 3)
    T_NUMBER(purple_account_set_string, 0)     // PurpleAccount*
    T_STRING(purple_account_set_string, 1)     // char*
    T_STRING(purple_account_set_string, 2)     // char*

    purple_account_set_string((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_bool, 3)
    T_NUMBER(purple_account_set_bool, 0)     // PurpleAccount*
    T_STRING(purple_account_set_bool, 1)     // char*
    T_NUMBER(purple_account_set_bool, 2)     // int

    purple_account_set_bool((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_ui_int, 4)
    T_NUMBER(purple_account_set_ui_int, 0)     // PurpleAccount*
    T_STRING(purple_account_set_ui_int, 1)     // char*
    T_STRING(purple_account_set_ui_int, 2)     // char*
    T_NUMBER(purple_account_set_ui_int, 3)     // int

    purple_account_set_ui_int((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_ui_string, 4)
    T_NUMBER(purple_account_set_ui_string, 0)     // PurpleAccount*
    T_STRING(purple_account_set_ui_string, 1)     // char*
    T_STRING(purple_account_set_ui_string, 2)     // char*
    T_STRING(purple_account_set_ui_string, 3)     // char*

    purple_account_set_ui_string((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_set_ui_bool, 4)
    T_NUMBER(purple_account_set_ui_bool, 0)     // PurpleAccount*
    T_STRING(purple_account_set_ui_bool, 1)     // char*
    T_STRING(purple_account_set_ui_bool, 2)     // char*
    T_NUMBER(purple_account_set_ui_bool, 3)     // int

    purple_account_set_ui_bool((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_is_connected, 1)
    T_NUMBER(purple_account_is_connected, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_account_is_connected((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_is_connecting, 1)
    T_NUMBER(purple_account_is_connecting, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_account_is_connecting((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_is_disconnected, 1)
    T_NUMBER(purple_account_is_disconnected, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_account_is_disconnected((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_username, 1)
    T_NUMBER(purple_account_get_username, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_username((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_password, 1)
    T_NUMBER(purple_account_get_password, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_password((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_alias, 1)
    T_NUMBER(purple_account_get_alias, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_alias((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_user_info, 1)
    T_NUMBER(purple_account_get_user_info, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_user_info((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_buddy_icon_path, 1)
    T_NUMBER(purple_account_get_buddy_icon_path, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_buddy_icon_path((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_protocol_id, 1)
    T_NUMBER(purple_account_get_protocol_id, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_protocol_id((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_protocol_name, 1)
    T_NUMBER(purple_account_get_protocol_name, 0)     // PurpleAccount*

    RETURN_STRING((char *)purple_account_get_protocol_name((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_connection, 1)
    T_NUMBER(purple_account_get_connection, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_connection((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_remember_password, 1)
    T_NUMBER(purple_account_get_remember_password, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_account_get_remember_password((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_check_mail, 1)
    T_NUMBER(purple_account_get_check_mail, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_account_get_check_mail((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_enabled, 2)
    T_NUMBER(purple_account_get_enabled, 0)     // PurpleAccount*
    T_STRING(purple_account_get_enabled, 1)     // char*

    RETURN_NUMBER(purple_account_get_enabled((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_proxy_info, 1)
    T_NUMBER(purple_account_get_proxy_info, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_proxy_info((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_active_status, 1)
    T_NUMBER(purple_account_get_active_status, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_active_status((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_status, 2)
    T_NUMBER(purple_account_get_status, 0)     // PurpleAccount*
    T_STRING(purple_account_get_status, 1)     // char*

    RETURN_NUMBER((long)purple_account_get_status((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_status_type, 2)
    T_NUMBER(purple_account_get_status_type, 0)     // PurpleAccount*
    T_STRING(purple_account_get_status_type, 1)     // char*

    RETURN_NUMBER((long)purple_account_get_status_type((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_status_type_with_primitive, 2)
    T_NUMBER(purple_account_get_status_type_with_primitive, 0)     // PurpleAccount*
    T_NUMBER(purple_account_get_status_type_with_primitive, 1)     // PurpleStatusPrimitive

    RETURN_NUMBER((long)purple_account_get_status_type_with_primitive((PurpleAccount *)(long)PARAM(0), (PurpleStatusPrimitive)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_presence, 1)
    T_NUMBER(purple_account_get_presence, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_presence((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_is_status_active, 2)
    T_NUMBER(purple_account_is_status_active, 0)     // PurpleAccount*
    T_STRING(purple_account_is_status_active, 1)     // char*

    RETURN_NUMBER(purple_account_is_status_active((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_status_types, 1)
    T_NUMBER(purple_account_get_status_types, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_status_types((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_int, 3)
    T_NUMBER(purple_account_get_int, 0)     // PurpleAccount*
    T_STRING(purple_account_get_int, 1)     // char*
    T_NUMBER(purple_account_get_int, 2)     // int

    RETURN_NUMBER(purple_account_get_int((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_string, 3)
    T_NUMBER(purple_account_get_string, 0)     // PurpleAccount*
    T_STRING(purple_account_get_string, 1)     // char*
    T_STRING(purple_account_get_string, 2)     // char*

    RETURN_STRING((char *)purple_account_get_string((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_bool, 3)
    T_NUMBER(purple_account_get_bool, 0)     // PurpleAccount*
    T_STRING(purple_account_get_bool, 1)     // char*
    T_NUMBER(purple_account_get_bool, 2)     // int

    RETURN_NUMBER(purple_account_get_bool((PurpleAccount *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_ui_int, 4)
    T_NUMBER(purple_account_get_ui_int, 0)     // PurpleAccount*
    T_STRING(purple_account_get_ui_int, 1)     // char*
    T_STRING(purple_account_get_ui_int, 2)     // char*
    T_NUMBER(purple_account_get_ui_int, 3)     // int

    RETURN_NUMBER(purple_account_get_ui_int((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_ui_string, 4)
    T_NUMBER(purple_account_get_ui_string, 0)     // PurpleAccount*
    T_STRING(purple_account_get_ui_string, 1)     // char*
    T_STRING(purple_account_get_ui_string, 2)     // char*
    T_STRING(purple_account_get_ui_string, 3)     // char*

    RETURN_STRING((char *)purple_account_get_ui_string((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_ui_bool, 4)
    T_NUMBER(purple_account_get_ui_bool, 0)     // PurpleAccount*
    T_STRING(purple_account_get_ui_bool, 1)     // char*
    T_STRING(purple_account_get_ui_bool, 2)     // char*
    T_NUMBER(purple_account_get_ui_bool, 3)     // int

    RETURN_NUMBER(purple_account_get_ui_bool((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_log, 2)
    T_NUMBER(purple_account_get_log, 0)     // PurpleAccount*
    T_NUMBER(purple_account_get_log, 1)     // int

    RETURN_NUMBER((long)purple_account_get_log((PurpleAccount *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_destroy_log, 1)
    T_NUMBER(purple_account_destroy_log, 0)     // PurpleAccount*

    purple_account_destroy_log((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_add_buddy, 2)
    T_NUMBER(purple_account_add_buddy, 0)     // PurpleAccount*
    T_NUMBER(purple_account_add_buddy, 1)     // PurpleBuddy*

    purple_account_add_buddy((PurpleAccount *)(long)PARAM(0), (PurpleBuddy *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_add_buddies, 2)
    T_NUMBER(purple_account_add_buddies, 0)     // PurpleAccount*
    T_NUMBER(purple_account_add_buddies, 1)     // GList*

    purple_account_add_buddies((PurpleAccount *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_remove_buddy, 3)
    T_NUMBER(purple_account_remove_buddy, 0)     // PurpleAccount*
    T_NUMBER(purple_account_remove_buddy, 1)     // PurpleBuddy*
    T_NUMBER(purple_account_remove_buddy, 2)     // PurpleGroup*

    purple_account_remove_buddy((PurpleAccount *)(long)PARAM(0), (PurpleBuddy *)(long)PARAM(1), (PurpleGroup *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_remove_buddies, 3)
    T_NUMBER(purple_account_remove_buddies, 0)     // PurpleAccount*
    T_NUMBER(purple_account_remove_buddies, 1)     // GList*
    T_NUMBER(purple_account_remove_buddies, 2)     // GList*

    purple_account_remove_buddies((PurpleAccount *)(long)PARAM(0), (GList *)(long)PARAM(1), (GList *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_remove_group, 2)
    T_NUMBER(purple_account_remove_group, 0)     // PurpleAccount*
    T_NUMBER(purple_account_remove_group, 1)     // PurpleGroup*

    purple_account_remove_group((PurpleAccount *)(long)PARAM(0), (PurpleGroup *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_change_password, 3)
    T_NUMBER(purple_account_change_password, 0)     // PurpleAccount*
    T_STRING(purple_account_change_password, 1)     // char*
    T_STRING(purple_account_change_password, 2)     // char*

    purple_account_change_password((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_supports_offline_message, 2)
    T_NUMBER(purple_account_supports_offline_message, 0)     // PurpleAccount*
    T_NUMBER(purple_account_supports_offline_message, 1)     // PurpleBuddy*

    RETURN_NUMBER(purple_account_supports_offline_message((PurpleAccount *)(long)PARAM(0), (PurpleBuddy *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_get_current_error, 1)
    T_NUMBER(purple_account_get_current_error, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_account_get_current_error((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_clear_current_error, 1)
    T_NUMBER(purple_account_clear_current_error, 0)     // PurpleAccount*

    purple_account_clear_current_error((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_add, 1)
    T_NUMBER(purple_accounts_add, 0)     // PurpleAccount*

    purple_accounts_add((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_remove, 1)
    T_NUMBER(purple_accounts_remove, 0)     // PurpleAccount*

    purple_accounts_remove((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_delete, 1)
    T_NUMBER(purple_accounts_delete, 0)     // PurpleAccount*

    purple_accounts_delete((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_reorder, 2)
    T_NUMBER(purple_accounts_reorder, 0)     // PurpleAccount*
    T_NUMBER(purple_accounts_reorder, 1)     // int

    purple_accounts_reorder((PurpleAccount *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_get_all, 0)

    RETURN_NUMBER((long)purple_accounts_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_get_all_active, 0)

    RETURN_NUMBER((long)purple_accounts_get_all_active())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_find, 2)
    T_STRING(purple_accounts_find, 0)     // char*
    T_STRING(purple_accounts_find, 1)     // char*

    RETURN_NUMBER((long)purple_accounts_find(PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_restore_current_statuses, 0)

    purple_accounts_restore_current_statuses();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_set_ui_ops, 1)
    T_NUMBER(purple_accounts_set_ui_ops, 0)     // PurpleAccountUiOps*

    purple_accounts_set_ui_ops((PurpleAccountUiOps *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_accounts_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_get_handle, 0)

    RETURN_NUMBER((long)purple_accounts_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_init, 0)

    purple_accounts_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_accounts_uninit, 0)

    purple_accounts_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_destroy, 1)
    T_NUMBER(purple_account_option_destroy, 0)     // PurpleAccountOption*

    purple_account_option_destroy((PurpleAccountOption *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_set_default_bool, 2)
    T_NUMBER(purple_account_option_set_default_bool, 0)     // PurpleAccountOption*
    T_NUMBER(purple_account_option_set_default_bool, 1)     // int

    purple_account_option_set_default_bool((PurpleAccountOption *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_set_default_int, 2)
    T_NUMBER(purple_account_option_set_default_int, 0)     // PurpleAccountOption*
    T_NUMBER(purple_account_option_set_default_int, 1)     // int

    purple_account_option_set_default_int((PurpleAccountOption *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_set_default_string, 2)
    T_NUMBER(purple_account_option_set_default_string, 0)     // PurpleAccountOption*
    T_STRING(purple_account_option_set_default_string, 1)     // char*

    purple_account_option_set_default_string((PurpleAccountOption *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_set_masked, 2)
    T_NUMBER(purple_account_option_set_masked, 0)     // PurpleAccountOption*
    T_NUMBER(purple_account_option_set_masked, 1)     // int

    purple_account_option_set_masked((PurpleAccountOption *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_set_list, 2)
    T_NUMBER(purple_account_option_set_list, 0)     // PurpleAccountOption*
    T_NUMBER(purple_account_option_set_list, 1)     // GList*

    purple_account_option_set_list((PurpleAccountOption *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_add_list_item, 3)
    T_NUMBER(purple_account_option_add_list_item, 0)     // PurpleAccountOption*
    T_STRING(purple_account_option_add_list_item, 1)     // char*
    T_STRING(purple_account_option_add_list_item, 2)     // char*

    purple_account_option_add_list_item((PurpleAccountOption *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_type, 1)
    T_NUMBER(purple_account_option_get_type, 0)     // PurpleAccountOption*

    RETURN_NUMBER(purple_account_option_get_type((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_text, 1)
    T_NUMBER(purple_account_option_get_text, 0)     // PurpleAccountOption*

    RETURN_STRING((char *)purple_account_option_get_text((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_setting, 1)
    T_NUMBER(purple_account_option_get_setting, 0)     // PurpleAccountOption*

    RETURN_STRING((char *)purple_account_option_get_setting((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_default_bool, 1)
    T_NUMBER(purple_account_option_get_default_bool, 0)     // PurpleAccountOption*

    RETURN_NUMBER(purple_account_option_get_default_bool((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_default_int, 1)
    T_NUMBER(purple_account_option_get_default_int, 0)     // PurpleAccountOption*

    RETURN_NUMBER(purple_account_option_get_default_int((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_default_string, 1)
    T_NUMBER(purple_account_option_get_default_string, 0)     // PurpleAccountOption*

    RETURN_STRING((char *)purple_account_option_get_default_string((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_default_list_value, 1)
    T_NUMBER(purple_account_option_get_default_list_value, 0)     // PurpleAccountOption*

    RETURN_STRING((char *)purple_account_option_get_default_list_value((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_masked, 1)
    T_NUMBER(purple_account_option_get_masked, 0)     // PurpleAccountOption*

    RETURN_NUMBER(purple_account_option_get_masked((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_option_get_list, 1)
    T_NUMBER(purple_account_option_get_list, 0)     // PurpleAccountOption*

    RETURN_NUMBER((long)purple_account_option_get_list((PurpleAccountOption *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_new, 3)
    T_STRING(purple_account_user_split_new, 0)     // char*
    T_STRING(purple_account_user_split_new, 1)     // char*
    T_NUMBER(purple_account_user_split_new, 2)     // char

    RETURN_NUMBER((long)purple_account_user_split_new(PARAM(0), PARAM(1), (char)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_destroy, 1)
    T_NUMBER(purple_account_user_split_destroy, 0)     // PurpleAccountUserSplit*

    purple_account_user_split_destroy((PurpleAccountUserSplit *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_get_text, 1)
    T_NUMBER(purple_account_user_split_get_text, 0)     // PurpleAccountUserSplit*

    RETURN_STRING((char *)purple_account_user_split_get_text((PurpleAccountUserSplit *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_get_default_value, 1)
    T_NUMBER(purple_account_user_split_get_default_value, 0)     // PurpleAccountUserSplit*

    RETURN_STRING((char *)purple_account_user_split_get_default_value((PurpleAccountUserSplit *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_get_separator, 1)
    T_NUMBER(purple_account_user_split_get_separator, 0)     // PurpleAccountUserSplit*

    RETURN_NUMBER(purple_account_user_split_get_separator((PurpleAccountUserSplit *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_get_reverse, 1)
    T_NUMBER(purple_account_user_split_get_reverse, 0)     // PurpleAccountUserSplit*

    RETURN_NUMBER(purple_account_user_split_get_reverse((PurpleAccountUserSplit *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_account_user_split_set_reverse, 2)
    T_NUMBER(purple_account_user_split_set_reverse, 0)     // PurpleAccountUserSplit*
    T_NUMBER(purple_account_user_split_set_reverse, 1)     // int

    purple_account_user_split_set_reverse((PurpleAccountUserSplit *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_new, 0)

    RETURN_NUMBER((long)purple_blist_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_set_blist, 1)
    T_HANDLE(purple_set_blist, 0)
    purple_set_blist((PurpleBuddyList *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_blist, 0)

    RETURN_NUMBER((long)purple_get_blist())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_root, 0)

    RETURN_NUMBER((long)purple_blist_get_root())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_buddies, 0)
    SListToArr(purple_blist_get_buddies(), RESULT);
//RETURN_NUMBER((long)purple_blist_get_buddies())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_ui_data, 0)

    RETURN_NUMBER((long)purple_blist_get_ui_data())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_set_ui_data, 1)
    T_NUMBER(purple_blist_set_ui_data, 0)     // gpointer

    purple_blist_set_ui_data((gpointer)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_next, 2)
    T_NUMBER(purple_blist_node_next, 0)     // PurpleBlistNode*
    T_NUMBER(purple_blist_node_next, 1)     // int

    RETURN_NUMBER((long)purple_blist_node_next((PurpleBlistNode *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_parent, 1)
    T_NUMBER(purple_blist_node_get_parent, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_parent((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_first_child, 1)
    T_NUMBER(purple_blist_node_get_first_child, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_first_child((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_sibling_next, 1)
    T_NUMBER(purple_blist_node_get_sibling_next, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_sibling_next((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_sibling_prev, 1)
    T_NUMBER(purple_blist_node_get_sibling_prev, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_sibling_prev((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_ui_data, 1)
    T_NUMBER(purple_blist_node_get_ui_data, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_ui_data((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_set_ui_data, 2)
    T_NUMBER(purple_blist_node_set_ui_data, 0)     // PurpleBlistNode*
    T_NUMBER(purple_blist_node_set_ui_data, 1)     // gpointer

    purple_blist_node_set_ui_data((PurpleBlistNode *)(long)PARAM(0), (gpointer)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_show, 0)

    purple_blist_show();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_destroy, 0)

    purple_blist_destroy();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_set_visible, 1)
    T_NUMBER(purple_blist_set_visible, 0)     // int

    purple_blist_set_visible((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_update_buddy_status, 2)
    T_NUMBER(purple_blist_update_buddy_status, 0)     // PurpleBuddy*
    T_NUMBER(purple_blist_update_buddy_status, 1)     // PurpleStatus*

    purple_blist_update_buddy_status((PurpleBuddy *)(long)PARAM(0), (PurpleStatus *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_update_node_icon, 1)
    T_NUMBER(purple_blist_update_node_icon, 0)     // PurpleBlistNode*

    purple_blist_update_node_icon((PurpleBlistNode *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_rename_buddy, 2)
    T_NUMBER(purple_blist_rename_buddy, 0)     // PurpleBuddy*
    T_STRING(purple_blist_rename_buddy, 1)     // char*

    purple_blist_rename_buddy((PurpleBuddy *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_alias_contact, 2)
    T_NUMBER(purple_blist_alias_contact, 0)     // PurpleContact*
    T_STRING(purple_blist_alias_contact, 1)     // char*

    purple_blist_alias_contact((PurpleContact *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_alias_buddy, 2)
    T_NUMBER(purple_blist_alias_buddy, 0)     // PurpleBuddy*
    T_STRING(purple_blist_alias_buddy, 1)     // char*

    purple_blist_alias_buddy((PurpleBuddy *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_server_alias_buddy, 2)
    T_NUMBER(purple_blist_server_alias_buddy, 0)     // PurpleBuddy*
    T_STRING(purple_blist_server_alias_buddy, 1)     // char*

    purple_blist_server_alias_buddy((PurpleBuddy *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_alias_chat, 2)
    T_NUMBER(purple_blist_alias_chat, 0)     // PurpleChat*
    T_STRING(purple_blist_alias_chat, 1)     // char*

    purple_blist_alias_chat((PurpleChat *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_rename_group, 2)
    T_NUMBER(purple_blist_rename_group, 0)     // PurpleGroup*
    T_STRING(purple_blist_rename_group, 1)     // char*

    purple_blist_rename_group((PurpleGroup *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_new, 3)
    T_NUMBER(purple_chat_new, 0)     // PurpleAccount*
    T_STRING(purple_chat_new, 1)     // char*
    T_NUMBER(purple_chat_new, 2)     // GHashTable*

    RETURN_NUMBER((long)purple_chat_new((PurpleAccount *)(long)PARAM(0), PARAM(1), (GHashTable *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_destroy, 1)
    T_NUMBER(purple_chat_destroy, 0)     // PurpleChat*

    purple_chat_destroy((PurpleChat *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_add_chat, 3)
    T_NUMBER(purple_blist_add_chat, 0)     // PurpleChat*
    T_NUMBER(purple_blist_add_chat, 1)     // PurpleGroup*
    T_NUMBER(purple_blist_add_chat, 2)     // PurpleBlistNode*

    purple_blist_add_chat((PurpleChat *)(long)PARAM(0), (PurpleGroup *)(long)PARAM(1), (PurpleBlistNode *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_new, 3)
    T_NUMBER(purple_buddy_new, 0)     // PurpleAccount*
    T_STRING(purple_buddy_new, 1)     // char*
    T_STRING(purple_buddy_new, 2)     // char*

    RETURN_NUMBER((long)purple_buddy_new((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_destroy, 1)
    T_NUMBER(purple_buddy_destroy, 0)     // PurpleBuddy*

    purple_buddy_destroy((PurpleBuddy *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_set_icon, 2)
    T_NUMBER(purple_buddy_set_icon, 0)     // PurpleBuddy*
    T_NUMBER(purple_buddy_set_icon, 1)     // PurpleBuddyIcon*

    purple_buddy_set_icon((PurpleBuddy *)(long)PARAM(0), (PurpleBuddyIcon *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_account, 1)
    T_NUMBER(purple_buddy_get_account, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_account((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_name, 1)
    T_NUMBER(purple_buddy_get_name, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_name((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_icon, 1)
    T_NUMBER(purple_buddy_get_icon, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_icon((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_protocol_data, 1)
    T_NUMBER(purple_buddy_get_protocol_data, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_protocol_data((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_set_protocol_data, 2)
    T_NUMBER(purple_buddy_set_protocol_data, 0)     // PurpleBuddy*
    T_NUMBER(purple_buddy_set_protocol_data, 1)     // gpointer

    purple_buddy_set_protocol_data((PurpleBuddy *)(long)PARAM(0), (gpointer)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_contact, 1)
    T_NUMBER(purple_buddy_get_contact, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_contact((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_presence, 1)
    T_NUMBER(purple_buddy_get_presence, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_presence((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_add_buddy, 4)
    T_NUMBER(purple_blist_add_buddy, 0)     // PurpleBuddy*
    T_NUMBER(purple_blist_add_buddy, 1)     // PurpleContact*
    T_NUMBER(purple_blist_add_buddy, 2)     // PurpleGroup*
    T_NUMBER(purple_blist_add_buddy, 3)     // PurpleBlistNode*

    purple_blist_add_buddy((PurpleBuddy *)(long)PARAM(0), (PurpleContact *)(long)PARAM(1), (PurpleGroup *)(long)PARAM(2), (PurpleBlistNode *)(long)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_group_new, 1)
    T_STRING(purple_group_new, 0)     // char*

    RETURN_NUMBER((long)purple_group_new(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_group_destroy, 1)
    T_NUMBER(purple_group_destroy, 0)     // PurpleGroup*

    purple_group_destroy((PurpleGroup *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_add_group, 2)
    T_NUMBER(purple_blist_add_group, 0)     // PurpleGroup*
    T_NUMBER(purple_blist_add_group, 1)     // PurpleBlistNode*

    purple_blist_add_group((PurpleGroup *)(long)PARAM(0), (PurpleBlistNode *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_new, 0)

    RETURN_NUMBER((long)purple_contact_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_destroy, 1)
    T_NUMBER(purple_contact_destroy, 0)     // PurpleContact*

    purple_contact_destroy((PurpleContact *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_add_contact, 3)
    T_NUMBER(purple_blist_add_contact, 0)     // PurpleContact*
    T_NUMBER(purple_blist_add_contact, 1)     // PurpleGroup*
    T_NUMBER(purple_blist_add_contact, 2)     // PurpleBlistNode*

    purple_blist_add_contact((PurpleContact *)(long)PARAM(0), (PurpleGroup *)(long)PARAM(1), (PurpleBlistNode *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_merge_contact, 2)
    T_NUMBER(purple_blist_merge_contact, 0)     // PurpleContact*
    T_NUMBER(purple_blist_merge_contact, 1)     // PurpleBlistNode*

    purple_blist_merge_contact((PurpleContact *)(long)PARAM(0), (PurpleBlistNode *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_get_priority_buddy, 1)
    T_NUMBER(purple_contact_get_priority_buddy, 0)     // PurpleContact*

    RETURN_NUMBER((long)purple_contact_get_priority_buddy((PurpleContact *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_get_alias, 1)
    T_NUMBER(purple_contact_get_alias, 0)     // PurpleContact*

    RETURN_STRING((char *)purple_contact_get_alias((PurpleContact *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_on_account, 2)
    T_NUMBER(purple_contact_on_account, 0)     // PurpleContact*
    T_NUMBER(purple_contact_on_account, 1)     // PurpleAccount*

    RETURN_NUMBER(purple_contact_on_account((PurpleContact *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_contact_invalidate_priority_buddy, 1)
    T_NUMBER(purple_contact_invalidate_priority_buddy, 0)     // PurpleContact*

    purple_contact_invalidate_priority_buddy((PurpleContact *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_remove_buddy, 1)
    T_NUMBER(purple_blist_remove_buddy, 0)     // PurpleBuddy*

    purple_blist_remove_buddy((PurpleBuddy *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_remove_contact, 1)
    T_NUMBER(purple_blist_remove_contact, 0)     // PurpleContact*

    purple_blist_remove_contact((PurpleContact *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_remove_chat, 1)
    T_NUMBER(purple_blist_remove_chat, 0)     // PurpleChat*

    purple_blist_remove_chat((PurpleChat *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_remove_group, 1)
    T_NUMBER(purple_blist_remove_group, 0)     // PurpleGroup*

    purple_blist_remove_group((PurpleGroup *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_alias_only, 1)
    T_NUMBER(purple_buddy_get_alias_only, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_alias_only((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_server_alias, 1)
    T_NUMBER(purple_buddy_get_server_alias, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_server_alias((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_contact_alias, 1)
    T_NUMBER(purple_buddy_get_contact_alias, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_contact_alias((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_alias, 1)
    T_NUMBER(purple_buddy_get_alias, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_alias((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_local_buddy_alias, 1)
    T_NUMBER(purple_buddy_get_local_buddy_alias, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_get_local_buddy_alias((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_get_name, 1)
    T_NUMBER(purple_chat_get_name, 0)     // PurpleChat*

    RETURN_STRING((char *)purple_chat_get_name((PurpleChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_buddy, 2)
    T_NUMBER(purple_find_buddy, 0)     // PurpleAccount*
    T_STRING(purple_find_buddy, 1)     // char*

    RETURN_NUMBER((long)purple_find_buddy((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_buddy_in_group, 3)
    T_NUMBER(purple_find_buddy_in_group, 0)     // PurpleAccount*
    T_STRING(purple_find_buddy_in_group, 1)     // char*
    T_NUMBER(purple_find_buddy_in_group, 2)     // PurpleGroup*

    RETURN_NUMBER((long)purple_find_buddy_in_group((PurpleAccount *)(long)PARAM(0), PARAM(1), (PurpleGroup *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_buddies, 2)
    T_NUMBER(purple_find_buddies, 0)     // PurpleAccount*
    T_STRING(purple_find_buddies, 1)     // char*

    RETURN_NUMBER((long)purple_find_buddies((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_group, 1)
    T_STRING(purple_find_group, 0)     // char*

    RETURN_NUMBER((long)purple_find_group(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_find_chat, 2)
    T_NUMBER(purple_blist_find_chat, 0)     // PurpleAccount*
    T_STRING(purple_blist_find_chat, 1)     // char*

    RETURN_NUMBER((long)purple_blist_find_chat((PurpleAccount *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_get_group, 1)
    T_NUMBER(purple_chat_get_group, 0)     // PurpleChat*

    RETURN_NUMBER((long)purple_chat_get_group((PurpleChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_get_account, 1)
    T_NUMBER(purple_chat_get_account, 0)     // PurpleChat*

    RETURN_NUMBER((long)purple_chat_get_account((PurpleChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_chat_get_components, 1)
    T_NUMBER(purple_chat_get_components, 0)     // PurpleChat*

    RETURN_NUMBER((long)purple_chat_get_components((PurpleChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_get_group, 1)
    T_NUMBER(purple_buddy_get_group, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_buddy_get_group((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_group_get_accounts, 1)
    T_NUMBER(purple_group_get_accounts, 0)     // PurpleGroup*

    RETURN_NUMBER((long)purple_group_get_accounts((PurpleGroup *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_group_on_account, 2)
    T_NUMBER(purple_group_on_account, 0)     // PurpleGroup*
    T_NUMBER(purple_group_on_account, 1)     // PurpleAccount*

    RETURN_NUMBER(purple_group_on_account((PurpleGroup *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_group_get_name, 1)
    T_NUMBER(purple_group_get_name, 0)     // PurpleGroup*

    RETURN_STRING((char *)purple_group_get_name((PurpleGroup *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_add_account, 1)
    T_NUMBER(purple_blist_add_account, 0)     // PurpleAccount*

    purple_blist_add_account((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_remove_account, 1)
    T_NUMBER(purple_blist_remove_account, 0)     // PurpleAccount*

    purple_blist_remove_account((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_group_size, 2)
    T_NUMBER(purple_blist_get_group_size, 0)     // PurpleGroup*
    T_NUMBER(purple_blist_get_group_size, 1)     // int

    RETURN_NUMBER(purple_blist_get_group_size((PurpleGroup *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_group_online_count, 1)
    T_NUMBER(purple_blist_get_group_online_count, 0)     // PurpleGroup*

    RETURN_NUMBER(purple_blist_get_group_online_count((PurpleGroup *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_load, 0)

    purple_blist_load();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_schedule_save, 0)

    purple_blist_schedule_save();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_request_add_buddy, 4)
    T_NUMBER(purple_blist_request_add_buddy, 0)     // PurpleAccount*
    T_STRING(purple_blist_request_add_buddy, 1)     // char*
    T_STRING(purple_blist_request_add_buddy, 2)     // char*
    T_STRING(purple_blist_request_add_buddy, 3)     // char*

    purple_blist_request_add_buddy((PurpleAccount *)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_request_add_chat, 4)
    T_NUMBER(purple_blist_request_add_chat, 0)     // PurpleAccount*
    T_NUMBER(purple_blist_request_add_chat, 1)     // PurpleGroup*
    T_STRING(purple_blist_request_add_chat, 2)     // char*
    T_STRING(purple_blist_request_add_chat, 3)     // char*

    purple_blist_request_add_chat((PurpleAccount *)(long)PARAM(0), (PurpleGroup *)(long)PARAM(1), PARAM(2), PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_request_add_group, 0)

    purple_blist_request_add_group();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_set_bool, 3)
    T_NUMBER(purple_blist_node_set_bool, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_set_bool, 1)     // char*
    T_NUMBER(purple_blist_node_set_bool, 2)     // int

    purple_blist_node_set_bool((PurpleBlistNode *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_bool, 2)
    T_NUMBER(purple_blist_node_get_bool, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_get_bool, 1)     // char*

    RETURN_NUMBER(purple_blist_node_get_bool((PurpleBlistNode *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_set_int, 3)
    T_NUMBER(purple_blist_node_set_int, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_set_int, 1)     // char*
    T_NUMBER(purple_blist_node_set_int, 2)     // int

    purple_blist_node_set_int((PurpleBlistNode *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_int, 2)
    T_NUMBER(purple_blist_node_get_int, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_get_int, 1)     // char*

    RETURN_NUMBER(purple_blist_node_get_int((PurpleBlistNode *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_set_string, 3)
    T_NUMBER(purple_blist_node_set_string, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_set_string, 1)     // char*
    T_STRING(purple_blist_node_set_string, 2)     // char*

    purple_blist_node_set_string((PurpleBlistNode *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_string, 2)
    T_NUMBER(purple_blist_node_get_string, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_get_string, 1)     // char*

    RETURN_STRING((char *)purple_blist_node_get_string((PurpleBlistNode *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_remove_setting, 2)
    T_NUMBER(purple_blist_node_remove_setting, 0)     // PurpleBlistNode*
    T_STRING(purple_blist_node_remove_setting, 1)     // char*

    purple_blist_node_remove_setting((PurpleBlistNode *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_set_flags, 2)
    T_NUMBER(purple_blist_node_set_flags, 0)     // PurpleBlistNode*
    T_NUMBER(purple_blist_node_set_flags, 1)     // PurpleBlistNodeFlags

    purple_blist_node_set_flags((PurpleBlistNode *)(long)PARAM(0), (PurpleBlistNodeFlags)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_flags, 1)
    T_NUMBER(purple_blist_node_get_flags, 0)     // PurpleBlistNode*

    RETURN_NUMBER(purple_blist_node_get_flags((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_type, 1)
    T_NUMBER(purple_blist_node_get_type, 0)     // PurpleBlistNode*

    RETURN_NUMBER(purple_blist_node_get_type((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_node_get_extended_menu, 1)
    T_NUMBER(purple_blist_node_get_extended_menu, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_blist_node_get_extended_menu((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_set_ui_ops, 1)
    T_NUMBER(purple_blist_set_ui_ops, 0)     // PurpleBlistUiOps*

    purple_blist_set_ui_ops((PurpleBlistUiOps *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_blist_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_get_handle, 0)

    RETURN_NUMBER((long)purple_blist_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_init, 0)

    purple_blist_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_blist_uninit, 0)

    purple_blist_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_new, 5)
    T_NUMBER(purple_buddy_icon_new, 0)     // PurpleAccount*
    T_STRING(purple_buddy_icon_new, 1)     // char*
    T_NUMBER(purple_buddy_icon_new, 2)     // void*
    T_NUMBER(purple_buddy_icon_new, 3)     // size_t
    T_STRING(purple_buddy_icon_new, 4)     // char*

    RETURN_NUMBER((long)purple_buddy_icon_new((PurpleAccount *)(long)PARAM(0), PARAM(1), (void *)(long)PARAM(2), (size_t)PARAM(3), PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_ref, 1)
    T_NUMBER(purple_buddy_icon_ref, 0)     // PurpleBuddyIcon*

    RETURN_NUMBER((long)purple_buddy_icon_ref((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_unref, 1)
    T_NUMBER(purple_buddy_icon_unref, 0)     // PurpleBuddyIcon*

    RETURN_NUMBER((long)purple_buddy_icon_unref((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_update, 1)
    T_NUMBER(purple_buddy_icon_update, 0)     // PurpleBuddyIcon*

    purple_buddy_icon_update((PurpleBuddyIcon *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_set_data, 4)
    T_NUMBER(purple_buddy_icon_set_data, 0)     // PurpleBuddyIcon*
    T_STRING(purple_buddy_icon_set_data, 1)     // unsigned char*
    T_NUMBER(purple_buddy_icon_set_data, 2)     // size_t
    T_STRING(purple_buddy_icon_set_data, 3)     // char*

    purple_buddy_icon_set_data((PurpleBuddyIcon *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2), (char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_account, 1)
    T_NUMBER(purple_buddy_icon_get_account, 0)     // PurpleBuddyIcon*

    RETURN_NUMBER((long)purple_buddy_icon_get_account((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_username, 1)
    T_NUMBER(purple_buddy_icon_get_username, 0)     // PurpleBuddyIcon*

    RETURN_STRING((char *)purple_buddy_icon_get_username((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_checksum, 1)
    T_NUMBER(purple_buddy_icon_get_checksum, 0)     // PurpleBuddyIcon*

    RETURN_STRING((char *)purple_buddy_icon_get_checksum((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_data, 2)
    T_NUMBER(purple_buddy_icon_get_data, 0)     // PurpleBuddyIcon*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    RETURN_NUMBER((long)purple_buddy_icon_get_data((PurpleBuddyIcon *)PARAM_INT(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_extension, 1)
    T_NUMBER(purple_buddy_icon_get_extension, 0)     // PurpleBuddyIcon*

    RETURN_STRING((char *)purple_buddy_icon_get_extension((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_full_path, 1)
    T_NUMBER(purple_buddy_icon_get_full_path, 0)     // PurpleBuddyIcon*

    RETURN_STRING((char *)purple_buddy_icon_get_full_path((PurpleBuddyIcon *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_set_for_user, 5)
    T_NUMBER(purple_buddy_icons_set_for_user, 0)     // PurpleAccount*
    T_STRING(purple_buddy_icons_set_for_user, 1)     // char*
    T_NUMBER(purple_buddy_icons_set_for_user, 2)     // void*
    T_NUMBER(purple_buddy_icons_set_for_user, 3)     // size_t
    T_STRING(purple_buddy_icons_set_for_user, 4)     // char*

    purple_buddy_icons_set_for_user((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (void *)(long)PARAM(2), (size_t)PARAM(3), (char *)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_get_checksum_for_user, 1)
    T_NUMBER(purple_buddy_icons_get_checksum_for_user, 0)     // PurpleBuddy*

    RETURN_STRING((char *)purple_buddy_icons_get_checksum_for_user((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_find, 2)
    T_NUMBER(purple_buddy_icons_find, 0)     // PurpleAccount*
    T_STRING(purple_buddy_icons_find, 1)     // char*

    RETURN_NUMBER((long)purple_buddy_icons_find((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_find_account_icon, 1)
    T_NUMBER(purple_buddy_icons_find_account_icon, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_buddy_icons_find_account_icon((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_set_account_icon, 3)
    T_NUMBER(purple_buddy_icons_set_account_icon, 0)     // PurpleAccount*
    T_STRING(purple_buddy_icons_set_account_icon, 1)     // unsigned char*
    T_NUMBER(purple_buddy_icons_set_account_icon, 2)     // size_t

    RETURN_NUMBER((long)purple_buddy_icons_set_account_icon((PurpleAccount *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_get_account_icon_timestamp, 1)
    T_NUMBER(purple_buddy_icons_get_account_icon_timestamp, 0)     // PurpleAccount*

    RETURN_NUMBER(purple_buddy_icons_get_account_icon_timestamp((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_node_has_custom_icon, 1)
    T_NUMBER(purple_buddy_icons_node_has_custom_icon, 0)     // PurpleBlistNode*

    RETURN_NUMBER(purple_buddy_icons_node_has_custom_icon((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_node_find_custom_icon, 1)
    T_NUMBER(purple_buddy_icons_node_find_custom_icon, 0)     // PurpleBlistNode*

    RETURN_NUMBER((long)purple_buddy_icons_node_find_custom_icon((PurpleBlistNode *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_node_set_custom_icon, 3)
    T_NUMBER(purple_buddy_icons_node_set_custom_icon, 0)     // PurpleBlistNode*
    T_STRING(purple_buddy_icons_node_set_custom_icon, 1)     // unsigned char*
    T_NUMBER(purple_buddy_icons_node_set_custom_icon, 2)     // size_t

    RETURN_NUMBER((long)purple_buddy_icons_node_set_custom_icon((PurpleBlistNode *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_node_set_custom_icon_from_file, 2)
    T_NUMBER(purple_buddy_icons_node_set_custom_icon_from_file, 0)     // PurpleBlistNode*
    T_STRING(purple_buddy_icons_node_set_custom_icon_from_file, 1)     // char*

    RETURN_NUMBER((long)purple_buddy_icons_node_set_custom_icon_from_file((PurpleBlistNode *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_set_caching, 1)
    T_NUMBER(purple_buddy_icons_set_caching, 0)     // int

    purple_buddy_icons_set_caching((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_is_caching, 0)

    RETURN_NUMBER(purple_buddy_icons_is_caching())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_set_cache_dir, 1)
    T_STRING(purple_buddy_icons_set_cache_dir, 0)     // char*

    purple_buddy_icons_set_cache_dir((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_get_cache_dir, 0)

    RETURN_STRING((char *)purple_buddy_icons_get_cache_dir())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_get_handle, 0)

    RETURN_NUMBER((long)purple_buddy_icons_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_init, 0)

    purple_buddy_icons_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icons_uninit, 0)

    purple_buddy_icons_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_buddy_icon_get_scale_size, 3)
    T_NUMBER(purple_buddy_icon_get_scale_size, 0)     // PurpleBuddyIconSpec*

// ... parameter 1 is by reference (int*)
    int local_parameter_1;
// ... parameter 2 is by reference (int*)
    int local_parameter_2;

    purple_buddy_icon_get_scale_size((PurpleBuddyIconSpec *)(long)PARAM(0), &local_parameter_1, &local_parameter_2);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_verify, 4)
    T_NUMBER(purple_certificate_verify, 0)     // PurpleCertificateVerifier*
    T_STRING(purple_certificate_verify, 1)     // char*
    T_NUMBER(purple_certificate_verify, 2)     // GList*
    T_DELEGATE(purple_certificate_verify, 3)
//T_NUMBER(3) // PurpleCertificateVerifiedCallback
//T_NUMBER(4) // gpointer

    purple_certificate_verify((PurpleCertificateVerifier *)(long)PARAM(0), (char *)PARAM(1), (GList *)(long)PARAM(2), PurpleCertificateVerifiedCallback_F, (gpointer)PARAM_INT(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_verify_complete, 2)
    T_NUMBER(purple_certificate_verify_complete, 0)
    T_NUMBER(purple_certificate_verify_complete, 1)     // PurpleCertificateVerificationStatus

// ... parameter 0 is by reference (PurpleCertificateVerificationRequest*)
//PurpleCertificateVerificationRequest local_parameter_0;

    purple_certificate_verify_complete((PurpleCertificateVerificationRequest *)PARAM_INT(0), (PurpleCertificateVerificationStatus)PARAM(1));
//SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_copy, 1)
    T_NUMBER(purple_certificate_copy, 0)     // PurpleCertificate*

    RETURN_NUMBER((long)purple_certificate_copy((PurpleCertificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_copy_list, 1)
    T_NUMBER(purple_certificate_copy_list, 0)     // GList*

    RETURN_NUMBER((long)purple_certificate_copy_list((GList *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_destroy, 1)
    T_NUMBER(purple_certificate_destroy, 0)     // PurpleCertificate*

    purple_certificate_destroy((PurpleCertificate *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_destroy_list, 1)
    T_NUMBER(purple_certificate_destroy_list, 0)     // GList*

    purple_certificate_destroy_list((GList *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_signed_by, 2)
    T_NUMBER(purple_certificate_signed_by, 0)     // PurpleCertificate*
    T_NUMBER(purple_certificate_signed_by, 1)     // PurpleCertificate*

    RETURN_NUMBER(purple_certificate_signed_by((PurpleCertificate *)(long)PARAM(0), (PurpleCertificate *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_check_signature_chain_with_failing, 2)
    T_NUMBER(purple_certificate_check_signature_chain_with_failing, 0)     // GList*

// ... parameter 1 is by reference (PurpleCertificate**)
    PurpleCertificate * local_parameter_1;

    RETURN_NUMBER(purple_certificate_check_signature_chain_with_failing((GList *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_check_signature_chain, 1)
    T_NUMBER(purple_certificate_check_signature_chain, 0)     // GList*

    RETURN_NUMBER(purple_certificate_check_signature_chain((GList *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_import, 2)
    T_NUMBER(purple_certificate_import, 0)     // PurpleCertificateScheme*
    T_STRING(purple_certificate_import, 1)     // char*

    RETURN_NUMBER((long)purple_certificate_import((PurpleCertificateScheme *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_export, 2)
    T_STRING(purple_certificate_export, 0)     // char*
    T_NUMBER(purple_certificate_export, 1)     // PurpleCertificate*

    RETURN_NUMBER(purple_certificate_export((char *)PARAM(0), (PurpleCertificate *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_fingerprint_sha1, 1)
    T_NUMBER(purple_certificate_get_fingerprint_sha1, 0)     // PurpleCertificate*

    RETURN_NUMBER((long)purple_certificate_get_fingerprint_sha1((PurpleCertificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_unique_id, 1)
    T_NUMBER(purple_certificate_get_unique_id, 0)     // PurpleCertificate*

    RETURN_STRING((char *)purple_certificate_get_unique_id((PurpleCertificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_issuer_unique_id, 1)
    T_NUMBER(purple_certificate_get_issuer_unique_id, 0)     // PurpleCertificate*

    RETURN_STRING((char *)purple_certificate_get_issuer_unique_id((PurpleCertificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_subject_name, 1)
    T_NUMBER(purple_certificate_get_subject_name, 0)     // PurpleCertificate*

    RETURN_STRING((char *)purple_certificate_get_subject_name((PurpleCertificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_check_subject_name, 2)
    T_NUMBER(purple_certificate_check_subject_name, 0)     // PurpleCertificate*
    T_STRING(purple_certificate_check_subject_name, 1)     // char*

    RETURN_NUMBER(purple_certificate_check_subject_name((PurpleCertificate *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_times, 3)
    T_NUMBER(purple_certificate_get_times, 0)     // PurpleCertificate*

// ... parameter 1 is by reference (time_t*)
    time_t local_parameter_1;
// ... parameter 2 is by reference (time_t*)
    time_t local_parameter_2;

    RETURN_NUMBER(purple_certificate_get_times((PurpleCertificate *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_mkpath, 2)
    T_NUMBER(purple_certificate_pool_mkpath, 0)     // PurpleCertificatePool*
    T_STRING(purple_certificate_pool_mkpath, 1)     // char*

    RETURN_STRING((char *)purple_certificate_pool_mkpath((PurpleCertificatePool *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_usable, 1)
    T_NUMBER(purple_certificate_pool_usable, 0)     // PurpleCertificatePool*

    RETURN_NUMBER(purple_certificate_pool_usable((PurpleCertificatePool *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_get_scheme, 1)
    T_NUMBER(purple_certificate_pool_get_scheme, 0)     // PurpleCertificatePool*

    RETURN_NUMBER((long)purple_certificate_pool_get_scheme((PurpleCertificatePool *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_contains, 2)
    T_NUMBER(purple_certificate_pool_contains, 0)     // PurpleCertificatePool*
    T_STRING(purple_certificate_pool_contains, 1)     // char*

    RETURN_NUMBER(purple_certificate_pool_contains((PurpleCertificatePool *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_retrieve, 2)
    T_NUMBER(purple_certificate_pool_retrieve, 0)     // PurpleCertificatePool*
    T_STRING(purple_certificate_pool_retrieve, 1)     // char*

    RETURN_NUMBER((long)purple_certificate_pool_retrieve((PurpleCertificatePool *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_store, 3)
    T_NUMBER(purple_certificate_pool_store, 0)     // PurpleCertificatePool*
    T_STRING(purple_certificate_pool_store, 1)     // char*
    T_NUMBER(purple_certificate_pool_store, 2)     // PurpleCertificate*

    RETURN_NUMBER(purple_certificate_pool_store((PurpleCertificatePool *)(long)PARAM(0), (char *)PARAM(1), (PurpleCertificate *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_delete, 2)
    T_NUMBER(purple_certificate_pool_delete, 0)     // PurpleCertificatePool*
    T_STRING(purple_certificate_pool_delete, 1)     // char*

    RETURN_NUMBER(purple_certificate_pool_delete((PurpleCertificatePool *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_get_idlist, 1)
    T_NUMBER(purple_certificate_pool_get_idlist, 0)     // PurpleCertificatePool*

    RETURN_NUMBER((long)purple_certificate_pool_get_idlist((PurpleCertificatePool *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_pool_destroy_idlist, 1)
    T_NUMBER(purple_certificate_pool_destroy_idlist, 0)     // GList*

    purple_certificate_pool_destroy_idlist((GList *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_init, 0)

    purple_certificate_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_uninit, 0)

    purple_certificate_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_handle, 0)

    RETURN_NUMBER((long)purple_certificate_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_find_scheme, 1)
    T_STRING(purple_certificate_find_scheme, 0)     // char*

    RETURN_NUMBER((long)purple_certificate_find_scheme((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_schemes, 0)

    RETURN_NUMBER((long)purple_certificate_get_schemes())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_register_scheme, 1)
    T_NUMBER(purple_certificate_register_scheme, 0)     // PurpleCertificateScheme*

    RETURN_NUMBER(purple_certificate_register_scheme((PurpleCertificateScheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_unregister_scheme, 1)
    T_NUMBER(purple_certificate_unregister_scheme, 0)     // PurpleCertificateScheme*

    RETURN_NUMBER(purple_certificate_unregister_scheme((PurpleCertificateScheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_find_verifier, 2)
    T_STRING(purple_certificate_find_verifier, 0)     // char*
    T_STRING(purple_certificate_find_verifier, 1)     // char*

    RETURN_NUMBER((long)purple_certificate_find_verifier((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_verifiers, 0)

    RETURN_NUMBER((long)purple_certificate_get_verifiers())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_register_verifier, 1)
    T_NUMBER(purple_certificate_register_verifier, 0)     // PurpleCertificateVerifier*

    RETURN_NUMBER(purple_certificate_register_verifier((PurpleCertificateVerifier *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_unregister_verifier, 1)
    T_NUMBER(purple_certificate_unregister_verifier, 0)     // PurpleCertificateVerifier*

    RETURN_NUMBER(purple_certificate_unregister_verifier((PurpleCertificateVerifier *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_find_pool, 2)
    T_STRING(purple_certificate_find_pool, 0)     // char*
    T_STRING(purple_certificate_find_pool, 1)     // char*

    RETURN_NUMBER((long)purple_certificate_find_pool((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_get_pools, 0)

    RETURN_NUMBER((long)purple_certificate_get_pools())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_register_pool, 1)
    T_NUMBER(purple_certificate_register_pool, 0)     // PurpleCertificatePool*

    RETURN_NUMBER(purple_certificate_register_pool((PurpleCertificatePool *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_unregister_pool, 1)
    T_NUMBER(purple_certificate_unregister_pool, 0)     // PurpleCertificatePool*

    RETURN_NUMBER(purple_certificate_unregister_pool((PurpleCertificatePool *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_display_x509, 1)
    T_NUMBER(purple_certificate_display_x509, 0)     // PurpleCertificate*

    purple_certificate_display_x509((PurpleCertificate *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_certificate_add_ca_search_path, 1)
    T_STRING(purple_certificate_add_ca_search_path, 0)     // char*

    purple_certificate_add_ca_search_path((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_get_name, 1)
    T_NUMBER(purple_cipher_get_name, 0)     // PurpleCipher*

    RETURN_STRING((char *)purple_cipher_get_name((PurpleCipher *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_get_capabilities, 1)
    T_NUMBER(purple_cipher_get_capabilities, 0)     // PurpleCipher*

    RETURN_NUMBER(purple_cipher_get_capabilities((PurpleCipher *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_digest_region, 6)
    T_STRING(purple_cipher_digest_region, 0)     // char*
    T_STRING(purple_cipher_digest_region, 1)     // unsigned char*
    T_NUMBER(purple_cipher_digest_region, 2)     // size_t
    T_NUMBER(purple_cipher_digest_region, 3)     // size_t
    T_STRING(purple_cipher_digest_region, 4)     // unsigned char

// ... parameter 5 is by reference (size_t*)
    size_t local_parameter_5;

    RETURN_NUMBER((long)purple_cipher_digest_region((char *)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2), (size_t)PARAM(3), (unsigned char *)PARAM(4), &local_parameter_5))
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_find_cipher, 1)
    T_STRING(purple_ciphers_find_cipher, 0)     // char*

    RETURN_NUMBER((long)purple_ciphers_find_cipher((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_register_cipher, 2)
    T_STRING(purple_ciphers_register_cipher, 0)     // char*
    T_NUMBER(purple_ciphers_register_cipher, 1)

    RETURN_NUMBER((long)purple_ciphers_register_cipher((char *)PARAM(0), (PurpleCipherOps *)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_unregister_cipher, 1)
    T_NUMBER(purple_ciphers_unregister_cipher, 0)     // PurpleCipher*

    RETURN_NUMBER(purple_ciphers_unregister_cipher((PurpleCipher *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_get_ciphers, 0)

    RETURN_NUMBER((long)purple_ciphers_get_ciphers())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_get_handle, 0)

    RETURN_NUMBER((long)purple_ciphers_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_init, 0)

    purple_ciphers_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ciphers_uninit, 0)

    purple_ciphers_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_option, 3)
    T_NUMBER(purple_cipher_context_set_option, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_set_option, 1)     // char*
    T_NUMBER(purple_cipher_context_set_option, 2)     // gpointer

    purple_cipher_context_set_option((PurpleCipherContext *)(long)PARAM(0), (char *)PARAM(1), (gpointer)PARAM_INT(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_option, 2)
    T_NUMBER(purple_cipher_context_get_option, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_get_option, 1)     // char*

    RETURN_NUMBER((long)purple_cipher_context_get_option((PurpleCipherContext *)PARAM_INT(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_new, 2)
    T_NUMBER(purple_cipher_context_new, 0)     // PurpleCipher*
    T_NUMBER(purple_cipher_context_new, 1)     // void*

    RETURN_NUMBER((long)purple_cipher_context_new((PurpleCipher *)(long)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_new_by_name, 2)
    T_STRING(purple_cipher_context_new_by_name, 0)     // char*
    T_NUMBER(purple_cipher_context_new_by_name, 1)     // void*

    RETURN_NUMBER((long)purple_cipher_context_new_by_name((char *)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_reset, 2)
    T_NUMBER(purple_cipher_context_reset, 0)     // PurpleCipherContext*
    T_NUMBER(purple_cipher_context_reset, 1)     // gpointer

    purple_cipher_context_reset((PurpleCipherContext *)(long)PARAM(0), (gpointer)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_destroy, 1)
    T_NUMBER(purple_cipher_context_destroy, 0)     // PurpleCipherContext*

    purple_cipher_context_destroy((PurpleCipherContext *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_iv, 3)
    T_NUMBER(purple_cipher_context_set_iv, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_set_iv, 1)     // unsigned char*
    T_NUMBER(purple_cipher_context_set_iv, 2)     // size_t

    purple_cipher_context_set_iv((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_append, 3)
    T_NUMBER(purple_cipher_context_append, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_append, 1)     // unsigned char*
    T_NUMBER(purple_cipher_context_append, 2)     // size_t

    purple_cipher_context_append((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_digest, 4)
    T_NUMBER(purple_cipher_context_digest, 0)     // PurpleCipherContext*
    T_NUMBER(purple_cipher_context_digest, 1)     // size_t
    T_STRING(purple_cipher_context_digest, 2)     // unsigned char

// ... parameter 3 is by reference (size_t*)
    size_t local_parameter_3;

    RETURN_NUMBER(purple_cipher_context_digest((PurpleCipherContext *)(long)PARAM(0), (size_t)PARAM(1), (unsigned char *)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_digest_to_str, 4)
    T_NUMBER(purple_cipher_context_digest_to_str, 0)     // PurpleCipherContext*
    T_NUMBER(purple_cipher_context_digest_to_str, 1)     // size_t
    T_STRING(purple_cipher_context_digest_to_str, 2)     // char

// ... parameter 3 is by reference (size_t*)
    size_t local_parameter_3;

    RETURN_NUMBER(purple_cipher_context_digest_to_str((PurpleCipherContext *)(long)PARAM(0), (size_t)PARAM(1), (char *)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_encrypt, 5)
    T_NUMBER(purple_cipher_context_encrypt, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_encrypt, 1)     // unsigned char
    T_NUMBER(purple_cipher_context_encrypt, 2)     // size_t
    T_STRING(purple_cipher_context_encrypt, 3)     // unsigned char

// ... parameter 4 is by reference (size_t*)
    size_t local_parameter_4;

    RETURN_NUMBER((long)purple_cipher_context_encrypt((PurpleCipherContext *)PARAM_INT(0), (unsigned char *)PARAM(1), (size_t)PARAM(2), (unsigned char *)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_decrypt, 5)
    T_NUMBER(purple_cipher_context_decrypt, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_decrypt, 1)     // unsigned char
    T_NUMBER(purple_cipher_context_decrypt, 2)     // size_t
    T_STRING(purple_cipher_context_decrypt, 3)     // unsigned char

// ... parameter 4 is by reference (size_t*)
    size_t local_parameter_4;

    RETURN_NUMBER(purple_cipher_context_decrypt((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2), (unsigned char *)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_salt, 2)
    T_NUMBER(purple_cipher_context_set_salt, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_set_salt, 1)     // unsigned char*

    purple_cipher_context_set_salt((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_salt_size, 1)
    T_NUMBER(purple_cipher_context_get_salt_size, 0)     // PurpleCipherContext*

    RETURN_NUMBER(purple_cipher_context_get_salt_size((PurpleCipherContext *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_key, 2)
    T_NUMBER(purple_cipher_context_set_key, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_set_key, 1)     // unsigned char*

    purple_cipher_context_set_key((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_key_size, 1)
    T_NUMBER(purple_cipher_context_get_key_size, 0)     // PurpleCipherContext*

    RETURN_NUMBER(purple_cipher_context_get_key_size((PurpleCipherContext *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_batch_mode, 2)
    T_NUMBER(purple_cipher_context_set_batch_mode, 0)     // PurpleCipherContext*
    T_NUMBER(purple_cipher_context_set_batch_mode, 1)     // PurpleCipherBatchMode

    purple_cipher_context_set_batch_mode((PurpleCipherContext *)(long)PARAM(0), (PurpleCipherBatchMode)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_batch_mode, 1)
    T_NUMBER(purple_cipher_context_get_batch_mode, 0)     // PurpleCipherContext*

    RETURN_NUMBER(purple_cipher_context_get_batch_mode((PurpleCipherContext *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_block_size, 1)
    T_NUMBER(purple_cipher_context_get_block_size, 0)     // PurpleCipherContext*

    RETURN_NUMBER(purple_cipher_context_get_block_size((PurpleCipherContext *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_key_with_len, 3)
    T_NUMBER(purple_cipher_context_set_key_with_len, 0)     // PurpleCipherContext*
    T_STRING(purple_cipher_context_set_key_with_len, 1)     // unsigned char*
    T_NUMBER(purple_cipher_context_set_key_with_len, 2)     // size_t

    purple_cipher_context_set_key_with_len((PurpleCipherContext *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_set_data, 2)
    T_NUMBER(purple_cipher_context_set_data, 0)     // PurpleCipherContext*
    T_NUMBER(purple_cipher_context_set_data, 1)     // gpointer

    purple_cipher_context_set_data((PurpleCipherContext *)(long)PARAM(0), (gpointer)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_context_get_data, 1)
    T_NUMBER(purple_cipher_context_get_data, 0)     // PurpleCipherContext*

    RETURN_NUMBER((long)purple_cipher_context_get_data((PurpleCipherContext *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_http_digest_calculate_session_key, 6)
    T_STRING(purple_cipher_http_digest_calculate_session_key, 0)     // char*
    T_STRING(purple_cipher_http_digest_calculate_session_key, 1)     // char*
    T_STRING(purple_cipher_http_digest_calculate_session_key, 2)     // char*
    T_STRING(purple_cipher_http_digest_calculate_session_key, 3)     // char*
    T_STRING(purple_cipher_http_digest_calculate_session_key, 4)     // char*
    T_STRING(purple_cipher_http_digest_calculate_session_key, 5)     // char*

    RETURN_STRING((char *)purple_cipher_http_digest_calculate_session_key((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (char *)PARAM(4), (char *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cipher_http_digest_calculate_response, 9)
    T_STRING(purple_cipher_http_digest_calculate_response, 0)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 1)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 2)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 3)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 4)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 5)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 6)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 7)     // char*
    T_STRING(purple_cipher_http_digest_calculate_response, 8)     // char*

    RETURN_STRING((char *)purple_cipher_http_digest_calculate_response((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (char *)PARAM(4), (char *)PARAM(5), (char *)PARAM(6), (char *)PARAM(7), (char *)PARAM(8)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmd_register, 7)
    T_STRING(purple_cmd_register, 0)     // char*
    T_STRING(purple_cmd_register, 1)     // char*
    T_NUMBER(purple_cmd_register, 2)     // PurpleCmdPriority
    T_NUMBER(purple_cmd_register, 3)     // PurpleCmdFlag
    T_STRING(purple_cmd_register, 4)     // char*
    T_DELEGATE(purple_cmd_register, 5)   // PurpleCmdFunc
    T_STRING(purple_cmd_register, 6)     // char*

    RETURN_NUMBER((long)purple_cmd_register((char *)PARAM(0), (char *)PARAM(1), (PurpleCmdPriority)PARAM(2), (PurpleCmdFlag)PARAM(3), (char *)PARAM(4), PurpleCmdRet_F, (char *)PARAM(6), (void *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmd_unregister, 1)
    T_NUMBER(purple_cmd_unregister, 0)     // PurpleCmdId

    purple_cmd_unregister((PurpleCmdId)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmd_do_command, 4)
    T_NUMBER(purple_cmd_do_command, 0)     // PurpleConversation*
    T_STRING(purple_cmd_do_command, 1)     // char*
    T_STRING(purple_cmd_do_command, 2)     // char*

// ... parameter 3 is by reference (char**)
    char *local_parameter_3;

    RETURN_NUMBER(purple_cmd_do_command((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmd_list, 1)
    T_NUMBER(purple_cmd_list, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_cmd_list((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmd_help, 2)
    T_NUMBER(purple_cmd_help, 0)     // PurpleConversation*
    T_STRING(purple_cmd_help, 1)     // char*

    RETURN_NUMBER((long)purple_cmd_help((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmds_get_handle, 0)

    RETURN_NUMBER((long)purple_cmds_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmds_init, 0)

    purple_cmds_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_cmds_uninit, 0)

    purple_cmds_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_set_state, 2)
    T_NUMBER(purple_connection_set_state, 0)     // PurpleConnection*
    T_NUMBER(purple_connection_set_state, 1)     // PurpleConnectionState

    purple_connection_set_state((PurpleConnection *)(long)PARAM(0), (PurpleConnectionState)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_set_account, 2)
    T_NUMBER(purple_connection_set_account, 0)     // PurpleConnection*
    T_NUMBER(purple_connection_set_account, 1)     // PurpleAccount*

    purple_connection_set_account((PurpleConnection *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_set_display_name, 2)
    T_NUMBER(purple_connection_set_display_name, 0)     // PurpleConnection*
    T_STRING(purple_connection_set_display_name, 1)     // char*

    purple_connection_set_display_name((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_set_protocol_data, 2)
    T_NUMBER(purple_connection_set_protocol_data, 0)     // PurpleConnection*
    T_NUMBER(purple_connection_set_protocol_data, 1)     // void*

    purple_connection_set_protocol_data((PurpleConnection *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_state, 1)
    T_NUMBER(purple_connection_get_state, 0)     // PurpleConnection*

    RETURN_NUMBER(purple_connection_get_state((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_account, 1)
    T_NUMBER(purple_connection_get_account, 0)     // PurpleConnection*

    RETURN_NUMBER((long)purple_connection_get_account((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_prpl, 1)
    T_NUMBER(purple_connection_get_prpl, 0)     // PurpleConnection*

    RETURN_NUMBER((long)purple_connection_get_prpl((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_password, 1)
    T_NUMBER(purple_connection_get_password, 0)     // PurpleConnection*

    RETURN_STRING((char *)purple_connection_get_password((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_display_name, 1)
    T_NUMBER(purple_connection_get_display_name, 0)     // PurpleConnection*

    RETURN_STRING((char *)purple_connection_get_display_name((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_get_protocol_data, 1)
    T_NUMBER(purple_connection_get_protocol_data, 0)     // PurpleConnection*

    RETURN_NUMBER((long)purple_connection_get_protocol_data((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_update_progress, 4)
    T_NUMBER(purple_connection_update_progress, 0)     // PurpleConnection*
    T_STRING(purple_connection_update_progress, 1)     // char*
    T_NUMBER(purple_connection_update_progress, 2)     // size_t
    T_NUMBER(purple_connection_update_progress, 3)     // size_t

    purple_connection_update_progress((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (size_t)PARAM(2), (size_t)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_notice, 2)
    T_NUMBER(purple_connection_notice, 0)     // PurpleConnection*
    T_STRING(purple_connection_notice, 1)     // char*

    purple_connection_notice((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_error, 2)
    T_NUMBER(purple_connection_error, 0)     // PurpleConnection*
    T_STRING(purple_connection_error, 1)     // char*

    purple_connection_error((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_error_reason, 3)
    T_NUMBER(purple_connection_error_reason, 0)     // PurpleConnection*
    T_NUMBER(purple_connection_error_reason, 1)     // PurpleConnectionError
    T_STRING(purple_connection_error_reason, 2)     // char*

    purple_connection_error_reason((PurpleConnection *)(long)PARAM(0), (PurpleConnectionError)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_ssl_error, 2)
    T_NUMBER(purple_connection_ssl_error, 0)     // PurpleConnection*
    T_NUMBER(purple_connection_ssl_error, 1)     // PurpleSslErrorType

    purple_connection_ssl_error((PurpleConnection *)(long)PARAM(0), (PurpleSslErrorType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connection_error_is_fatal, 1)
    T_NUMBER(purple_connection_error_is_fatal, 0)     // PurpleConnectionError

    RETURN_NUMBER(purple_connection_error_is_fatal((PurpleConnectionError)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_disconnect_all, 0)

    purple_connections_disconnect_all();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_get_all, 0)

    RETURN_NUMBER((long)purple_connections_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_get_connecting, 0)

    RETURN_NUMBER((long)purple_connections_get_connecting())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_set_ui_ops, 1)
    T_NUMBER(purple_connections_set_ui_ops, 0)
    purple_connections_set_ui_ops((PurpleConnectionUiOps *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_connections_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_init, 0)

    purple_connections_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_uninit, 0)

    purple_connections_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_connections_get_handle, 0)

    RETURN_NUMBER((long)purple_connections_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_new, 3)
    T_NUMBER(purple_conversation_new, 0)     // PurpleConversationType
    T_NUMBER(purple_conversation_new, 1)     // PurpleAccount*
    T_STRING(purple_conversation_new, 2)     // char*

    RETURN_NUMBER((long)purple_conversation_new((PurpleConversationType)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_destroy, 1)
    T_NUMBER(purple_conversation_destroy, 0)     // PurpleConversation*

    purple_conversation_destroy((PurpleConversation *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_present, 1)
    T_NUMBER(purple_conversation_present, 0)     // PurpleConversation*

    purple_conversation_present((PurpleConversation *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_type, 1)
    T_NUMBER(purple_conversation_get_type, 0)     // PurpleConversation*

    RETURN_NUMBER(purple_conversation_get_type((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_ui_ops, 2)
    T_NUMBER(purple_conversation_set_ui_ops, 0)     // PurpleConversation*
    T_ARRAY(purple_conversation_set_ui_ops, 1)

    purple_conversation_set_ui_ops((PurpleConversation *)(long)PARAM(0), InitConversationOps(PARAMETER(1)));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversations_set_ui_ops, 1)
    T_ARRAY(purple_conversations_set_ui_ops, 0)

    purple_conversations_set_ui_ops(InitConversationOps(PARAMETER(0)));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_ui_ops, 1)
    T_NUMBER(purple_conversation_get_ui_ops, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_ui_ops((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_account, 2)
    T_NUMBER(purple_conversation_set_account, 0)     // PurpleConversation*
    T_NUMBER(purple_conversation_set_account, 1)     // PurpleAccount*

    purple_conversation_set_account((PurpleConversation *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_account, 1)
    T_NUMBER(purple_conversation_get_account, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_account((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_gc, 1)
    T_NUMBER(purple_conversation_get_gc, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_gc((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_title, 2)
    T_NUMBER(purple_conversation_set_title, 0)     // PurpleConversation*
    T_STRING(purple_conversation_set_title, 1)     // char*

    purple_conversation_set_title((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_title, 1)
    T_NUMBER(purple_conversation_get_title, 0)     // PurpleConversation*

    RETURN_STRING((char *)purple_conversation_get_title((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_autoset_title, 1)
    T_NUMBER(purple_conversation_autoset_title, 0)     // PurpleConversation*

    purple_conversation_autoset_title((PurpleConversation *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_name, 2)
    T_NUMBER(purple_conversation_set_name, 0)     // PurpleConversation*
    T_STRING(purple_conversation_set_name, 1)     // char*

    purple_conversation_set_name((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_name, 1)
    T_NUMBER(purple_conversation_get_name, 0)     // PurpleConversation*

    RETURN_STRING((char *)purple_conversation_get_name((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_logging, 2)
    T_NUMBER(purple_conversation_set_logging, 0)     // PurpleConversation*
    T_NUMBER(purple_conversation_set_logging, 1)     // int

    purple_conversation_set_logging((PurpleConversation *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_is_logging, 1)
    T_NUMBER(purple_conversation_is_logging, 0)     // PurpleConversation*

    RETURN_NUMBER(purple_conversation_is_logging((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_close_logs, 1)
    T_NUMBER(purple_conversation_close_logs, 0)     // PurpleConversation*

    purple_conversation_close_logs((PurpleConversation *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_im_data, 1)
    T_NUMBER(purple_conversation_get_im_data, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_im_data((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_chat_data, 1)
    T_NUMBER(purple_conversation_get_chat_data, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_chat_data((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_data, 3)
    T_NUMBER(purple_conversation_set_data, 0)     // PurpleConversation*
    T_STRING(purple_conversation_set_data, 1)     // char*
    T_NUMBER(purple_conversation_set_data, 2)     // gpointer

    purple_conversation_set_data((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (gpointer)PARAM_INT(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_data, 2)
    T_NUMBER(purple_conversation_get_data, 0)     // PurpleConversation*
    T_STRING(purple_conversation_get_data, 1)     // char*

    RETURN_NUMBER((long)purple_conversation_get_data((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_conversations, 0)

    RETURN_NUMBER((long)purple_get_conversations())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_ims, 0)

    RETURN_NUMBER((long)purple_get_ims())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_chats, 0)

    RETURN_NUMBER((long)purple_get_chats())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_conversation_with_account, 3)
    T_NUMBER(purple_find_conversation_with_account, 0)     // PurpleConversationType
    T_STRING(purple_find_conversation_with_account, 1)     // char*
    T_NUMBER(purple_find_conversation_with_account, 2)     // PurpleAccount*

    RETURN_NUMBER((long)purple_find_conversation_with_account((PurpleConversationType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_write, 5)
    T_NUMBER(purple_conversation_write, 0)     // PurpleConversation*
    T_STRING(purple_conversation_write, 1)     // char*
    T_STRING(purple_conversation_write, 2)     // char*
    T_NUMBER(purple_conversation_write, 3)     // PurpleMessageFlags
    T_NUMBER(purple_conversation_write, 4)     // time_t

    purple_conversation_write((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (PurpleMessageFlags)PARAM(3), (time_t)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_set_features, 2)
    T_NUMBER(purple_conversation_set_features, 0)     // PurpleConversation*
    T_NUMBER(purple_conversation_set_features, 1)     // PurpleConnectionFlags

    purple_conversation_set_features((PurpleConversation *)(long)PARAM(0), (PurpleConnectionFlags)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_features, 1)
    T_NUMBER(purple_conversation_get_features, 0)     // PurpleConversation*

    RETURN_NUMBER(purple_conversation_get_features((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_has_focus, 1)
    T_NUMBER(purple_conversation_has_focus, 0)     // PurpleConversation*

    RETURN_NUMBER(purple_conversation_has_focus((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_update, 2)
    T_NUMBER(purple_conversation_update, 0)     // PurpleConversation*
    T_NUMBER(purple_conversation_update, 1)     // PurpleConvUpdateType

    purple_conversation_update((PurpleConversation *)(long)PARAM(0), (PurpleConvUpdateType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_message_history, 1)
    T_NUMBER(purple_conversation_get_message_history, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_message_history((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_clear_message_history, 1)
    T_NUMBER(purple_conversation_clear_message_history, 0)     // PurpleConversation*

    purple_conversation_clear_message_history((PurpleConversation *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_message_get_sender, 1)
    T_NUMBER(purple_conversation_message_get_sender, 0)     // PurpleConvMessage*

    RETURN_STRING((char *)purple_conversation_message_get_sender((PurpleConvMessage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_message_get_message, 1)
    T_NUMBER(purple_conversation_message_get_message, 0)     // PurpleConvMessage*

    RETURN_STRING((char *)purple_conversation_message_get_message((PurpleConvMessage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_message_get_flags, 1)
    T_NUMBER(purple_conversation_message_get_flags, 0)     // PurpleConvMessage*

    RETURN_NUMBER(purple_conversation_message_get_flags((PurpleConvMessage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_message_get_timestamp, 1)
    T_NUMBER(purple_conversation_message_get_timestamp, 0)     // PurpleConvMessage*

    RETURN_NUMBER(purple_conversation_message_get_timestamp((PurpleConvMessage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_conversation, 1)
    T_NUMBER(purple_conv_im_get_conversation, 0)     // PurpleConvIm*

    RETURN_NUMBER((long)purple_conv_im_get_conversation((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_set_icon, 2)
    T_NUMBER(purple_conv_im_set_icon, 0)     // PurpleConvIm*
    T_NUMBER(purple_conv_im_set_icon, 1)     // PurpleBuddyIcon*

    purple_conv_im_set_icon((PurpleConvIm *)(long)PARAM(0), (PurpleBuddyIcon *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_icon, 1)
    T_NUMBER(purple_conv_im_get_icon, 0)     // PurpleConvIm*

    RETURN_NUMBER((long)purple_conv_im_get_icon((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_set_typing_state, 2)
    T_NUMBER(purple_conv_im_set_typing_state, 0)     // PurpleConvIm*
    T_NUMBER(purple_conv_im_set_typing_state, 1)     // PurpleTypingState

    purple_conv_im_set_typing_state((PurpleConvIm *)(long)PARAM(0), (PurpleTypingState)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_typing_state, 1)
    T_NUMBER(purple_conv_im_get_typing_state, 0)     // PurpleConvIm*

    RETURN_NUMBER(purple_conv_im_get_typing_state((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_start_typing_timeout, 2)
    T_NUMBER(purple_conv_im_start_typing_timeout, 0)     // PurpleConvIm*
    T_NUMBER(purple_conv_im_start_typing_timeout, 1)     // int

    purple_conv_im_start_typing_timeout((PurpleConvIm *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_stop_typing_timeout, 1)
    T_NUMBER(purple_conv_im_stop_typing_timeout, 0)     // PurpleConvIm*

    purple_conv_im_stop_typing_timeout((PurpleConvIm *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_typing_timeout, 1)
    T_NUMBER(purple_conv_im_get_typing_timeout, 0)     // PurpleConvIm*

    RETURN_NUMBER(purple_conv_im_get_typing_timeout((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_set_type_again, 2)
    T_NUMBER(purple_conv_im_set_type_again, 0)     // PurpleConvIm*
    T_NUMBER(purple_conv_im_set_type_again, 1)     // unsigned int

    purple_conv_im_set_type_again((PurpleConvIm *)(long)PARAM(0), (unsigned int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_type_again, 1)
    T_NUMBER(purple_conv_im_get_type_again, 0)     // PurpleConvIm*

    RETURN_NUMBER(purple_conv_im_get_type_again((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_start_send_typed_timeout, 1)
    T_NUMBER(purple_conv_im_start_send_typed_timeout, 0)     // PurpleConvIm*

    purple_conv_im_start_send_typed_timeout((PurpleConvIm *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_stop_send_typed_timeout, 1)
    T_NUMBER(purple_conv_im_stop_send_typed_timeout, 0)     // PurpleConvIm*

    purple_conv_im_stop_send_typed_timeout((PurpleConvIm *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_get_send_typed_timeout, 1)
    T_NUMBER(purple_conv_im_get_send_typed_timeout, 0)     // PurpleConvIm*

    RETURN_NUMBER(purple_conv_im_get_send_typed_timeout((PurpleConvIm *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_update_typing, 1)
    T_NUMBER(purple_conv_im_update_typing, 0)     // PurpleConvIm*

    purple_conv_im_update_typing((PurpleConvIm *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_write, 5)
    T_NUMBER(purple_conv_im_write, 0)     // PurpleConvIm*
    T_STRING(purple_conv_im_write, 1)     // char*
    T_STRING(purple_conv_im_write, 2)     // char*
    T_NUMBER(purple_conv_im_write, 3)     // PurpleMessageFlags
    T_NUMBER(purple_conv_im_write, 4)     // time_t

    purple_conv_im_write((PurpleConvIm *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (PurpleMessageFlags)PARAM(3), (time_t)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_present_error, 3)
    T_STRING(purple_conv_present_error, 0)     // char*
    T_NUMBER(purple_conv_present_error, 1)     // PurpleAccount*
    T_STRING(purple_conv_present_error, 2)     // char*

    RETURN_NUMBER(purple_conv_present_error((char *)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_send, 2)
    T_NUMBER(purple_conv_im_send, 0)     // PurpleConvIm*
    T_STRING(purple_conv_im_send, 1)     // char*

    purple_conv_im_send((PurpleConvIm *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_send_confirm, 2)
    T_NUMBER(purple_conv_send_confirm, 0)     // PurpleConversation*
    T_STRING(purple_conv_send_confirm, 1)     // char*

    purple_conv_send_confirm((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_im_send_with_flags, 3)
    T_NUMBER(purple_conv_im_send_with_flags, 0)     // PurpleConvIm*
    T_STRING(purple_conv_im_send_with_flags, 1)     // char*
    T_NUMBER(purple_conv_im_send_with_flags, 2)     // PurpleMessageFlags

    purple_conv_im_send_with_flags((PurpleConvIm *)(long)PARAM(0), (char *)PARAM(1), (PurpleMessageFlags)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_custom_smiley_add, 5)
    T_NUMBER(purple_conv_custom_smiley_add, 0)     // PurpleConversation*
    T_STRING(purple_conv_custom_smiley_add, 1)     // char*
    T_STRING(purple_conv_custom_smiley_add, 2)     // char*
    T_STRING(purple_conv_custom_smiley_add, 3)     // char*
    T_NUMBER(purple_conv_custom_smiley_add, 4)     // int

    RETURN_NUMBER(purple_conv_custom_smiley_add((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_custom_smiley_write, 4)
    T_NUMBER(purple_conv_custom_smiley_write, 0)     // PurpleConversation*
    T_STRING(purple_conv_custom_smiley_write, 1)     // char*
    T_STRING(purple_conv_custom_smiley_write, 2)     // unsigned char*
    T_NUMBER(purple_conv_custom_smiley_write, 3)     // gsize

    purple_conv_custom_smiley_write((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (unsigned char *)PARAM(2), (gsize)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_custom_smiley_close, 2)
    T_NUMBER(purple_conv_custom_smiley_close, 0)     // PurpleConversation*
    T_STRING(purple_conv_custom_smiley_close, 1)     // char*

    purple_conv_custom_smiley_close((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_conversation, 1)
    T_NUMBER(purple_conv_chat_get_conversation, 0)     // PurpleConvChat*

    RETURN_NUMBER((long)purple_conv_chat_get_conversation((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_set_users, 2)
    T_NUMBER(purple_conv_chat_set_users, 0)     // PurpleConvChat*
    T_NUMBER(purple_conv_chat_set_users, 1)     // GList*

    RETURN_NUMBER((long)purple_conv_chat_set_users((PurpleConvChat *)(long)PARAM(0), (GList *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_users, 1)
    T_NUMBER(purple_conv_chat_get_users, 0)     // PurpleConvChat*

    RETURN_NUMBER((long)purple_conv_chat_get_users((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_ignore, 2)
    T_NUMBER(purple_conv_chat_ignore, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_ignore, 1)     // char*

    purple_conv_chat_ignore((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_unignore, 2)
    T_NUMBER(purple_conv_chat_unignore, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_unignore, 1)     // char*

    purple_conv_chat_unignore((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_set_ignored, 2)
    T_NUMBER(purple_conv_chat_set_ignored, 0)     // PurpleConvChat*
    T_NUMBER(purple_conv_chat_set_ignored, 1)     // GList*

    RETURN_NUMBER((long)purple_conv_chat_set_ignored((PurpleConvChat *)(long)PARAM(0), (GList *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_ignored, 1)
    T_NUMBER(purple_conv_chat_get_ignored, 0)     // PurpleConvChat*

    RETURN_NUMBER((long)purple_conv_chat_get_ignored((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_ignored_user, 2)
    T_NUMBER(purple_conv_chat_get_ignored_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_get_ignored_user, 1)     // char*

    RETURN_STRING((char *)purple_conv_chat_get_ignored_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_is_user_ignored, 2)
    T_NUMBER(purple_conv_chat_is_user_ignored, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_is_user_ignored, 1)     // char*

    RETURN_NUMBER(purple_conv_chat_is_user_ignored((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_set_topic, 3)
    T_NUMBER(purple_conv_chat_set_topic, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_set_topic, 1)     // char*
    T_STRING(purple_conv_chat_set_topic, 2)     // char*

    purple_conv_chat_set_topic((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_topic, 1)
    T_NUMBER(purple_conv_chat_get_topic, 0)     // PurpleConvChat*

    RETURN_STRING((char *)purple_conv_chat_get_topic((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_set_id, 2)
    T_NUMBER(purple_conv_chat_set_id, 0)     // PurpleConvChat*
    T_NUMBER(purple_conv_chat_set_id, 1)     // int

    purple_conv_chat_set_id((PurpleConvChat *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_id, 1)
    T_NUMBER(purple_conv_chat_get_id, 0)     // PurpleConvChat*

    RETURN_NUMBER(purple_conv_chat_get_id((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_write, 5)
    T_NUMBER(purple_conv_chat_write, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_write, 1)     // char*
    T_STRING(purple_conv_chat_write, 2)     // char*
    T_NUMBER(purple_conv_chat_write, 3)     // PurpleMessageFlags
    T_NUMBER(purple_conv_chat_write, 4)     // time_t

    purple_conv_chat_write((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (PurpleMessageFlags)PARAM(3), (time_t)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_send, 2)
    T_NUMBER(purple_conv_chat_send, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_send, 1)     // char*

    purple_conv_chat_send((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_send_with_flags, 3)
    T_NUMBER(purple_conv_chat_send_with_flags, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_send_with_flags, 1)     // char*
    T_NUMBER(purple_conv_chat_send_with_flags, 2)     // PurpleMessageFlags

    purple_conv_chat_send_with_flags((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (PurpleMessageFlags)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_add_user, 5)
    T_NUMBER(purple_conv_chat_add_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_add_user, 1)     // char*
    T_STRING(purple_conv_chat_add_user, 2)     // char*
    T_NUMBER(purple_conv_chat_add_user, 3)     // PurpleConvChatBuddyFlags
    T_NUMBER(purple_conv_chat_add_user, 4)     // int

    purple_conv_chat_add_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (PurpleConvChatBuddyFlags)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_add_users, 5)
    T_NUMBER(purple_conv_chat_add_users, 0)     // PurpleConvChat*
    T_NUMBER(purple_conv_chat_add_users, 1)     // GList*
    T_NUMBER(purple_conv_chat_add_users, 2)     // GList*
    T_NUMBER(purple_conv_chat_add_users, 3)     // GList*
    T_NUMBER(purple_conv_chat_add_users, 4)     // int

    purple_conv_chat_add_users((PurpleConvChat *)(long)PARAM(0), (GList *)(long)PARAM(1), (GList *)(long)PARAM(2), (GList *)(long)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_rename_user, 3)
    T_NUMBER(purple_conv_chat_rename_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_rename_user, 1)     // char*
    T_STRING(purple_conv_chat_rename_user, 2)     // char*

    purple_conv_chat_rename_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_remove_user, 3)
    T_NUMBER(purple_conv_chat_remove_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_remove_user, 1)     // char*
    T_STRING(purple_conv_chat_remove_user, 2)     // char*

    purple_conv_chat_remove_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_remove_users, 3)
    T_NUMBER(purple_conv_chat_remove_users, 0)     // PurpleConvChat*
    T_NUMBER(purple_conv_chat_remove_users, 1)     // GList*
    T_STRING(purple_conv_chat_remove_users, 2)     // char*

    purple_conv_chat_remove_users((PurpleConvChat *)(long)PARAM(0), (GList *)(long)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_find_user, 2)
    T_NUMBER(purple_conv_chat_find_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_find_user, 1)     // char*

    RETURN_NUMBER(purple_conv_chat_find_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_user_set_flags, 3)
    T_NUMBER(purple_conv_chat_user_set_flags, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_user_set_flags, 1)     // char*
    T_NUMBER(purple_conv_chat_user_set_flags, 2)     // PurpleConvChatBuddyFlags

    purple_conv_chat_user_set_flags((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (PurpleConvChatBuddyFlags)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_user_get_flags, 2)
    T_NUMBER(purple_conv_chat_user_get_flags, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_user_get_flags, 1)     // char*

    RETURN_NUMBER(purple_conv_chat_user_get_flags((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_clear_users, 1)
    T_NUMBER(purple_conv_chat_clear_users, 0)     // PurpleConvChat*

    purple_conv_chat_clear_users((PurpleConvChat *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_set_nick, 2)
    T_NUMBER(purple_conv_chat_set_nick, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_set_nick, 1)     // char*

    purple_conv_chat_set_nick((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_get_nick, 1)
    T_NUMBER(purple_conv_chat_get_nick, 0)     // PurpleConvChat*

    RETURN_STRING((char *)purple_conv_chat_get_nick((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_chat, 2)
    T_NUMBER(purple_find_chat, 0)     // PurpleConnection*
    T_NUMBER(purple_find_chat, 1)     // int

    RETURN_NUMBER((long)purple_find_chat((PurpleConnection *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_left, 1)
    T_NUMBER(purple_conv_chat_left, 0)     // PurpleConvChat*

    purple_conv_chat_left((PurpleConvChat *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_invite_user, 4)
    T_NUMBER(purple_conv_chat_invite_user, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_invite_user, 1)     // char*
    T_STRING(purple_conv_chat_invite_user, 2)     // char*
    T_NUMBER(purple_conv_chat_invite_user, 3)     // int

    purple_conv_chat_invite_user((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_has_left, 1)
    T_NUMBER(purple_conv_chat_has_left, 0)     // PurpleConvChat*

    RETURN_NUMBER(purple_conv_chat_has_left((PurpleConvChat *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_cb_new, 3)
    T_STRING(purple_conv_chat_cb_new, 0)     // char*
    T_STRING(purple_conv_chat_cb_new, 1)     // char*
    T_NUMBER(purple_conv_chat_cb_new, 2)     // PurpleConvChatBuddyFlags

    RETURN_NUMBER((long)purple_conv_chat_cb_new((char *)PARAM(0), (char *)PARAM(1), (PurpleConvChatBuddyFlags)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_cb_find, 2)
    T_NUMBER(purple_conv_chat_cb_find, 0)     // PurpleConvChat*
    T_STRING(purple_conv_chat_cb_find, 1)     // char*

    RETURN_NUMBER((long)purple_conv_chat_cb_find((PurpleConvChat *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_cb_get_name, 1)
    T_HANDLE(purple_conv_chat_cb_get_name, 0)
    RETURN_STRING((char *)purple_conv_chat_cb_get_name((PurpleConvChatBuddy *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conv_chat_cb_destroy, 1)
    T_HANDLE(purple_conv_chat_cb_destroy, 0)

    purple_conv_chat_cb_destroy((PurpleConvChatBuddy *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_get_extended_menu, 1)
    T_NUMBER(purple_conversation_get_extended_menu, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_conversation_get_extended_menu((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversation_do_command, 4)
    T_NUMBER(purple_conversation_do_command, 0)     // PurpleConversation*
    T_STRING(purple_conversation_do_command, 1)     // char*
    T_STRING(purple_conversation_do_command, 2)     // char*

// ... parameter 3 is by reference (char**)
    char *local_parameter_3;

    RETURN_NUMBER(purple_conversation_do_command((PurpleConversation *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversations_get_handle, 0)

    RETURN_NUMBER((long)purple_conversations_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversations_init, 0)

    purple_conversations_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_conversations_uninit, 0)

    purple_conversations_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_init, 1)
    T_STRING(purple_core_init, 0)     // char*

    purple_eventloop_set_ui_ops(&glib_eventloops);
    RETURN_NUMBER(purple_core_init((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_quit, 0)

    purple_core_quit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_quit_cb, 1)
    T_NUMBER(purple_core_quit_cb, 0)     // gpointer

    RETURN_NUMBER(purple_core_quit_cb((gpointer)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_get_version, 0)

    RETURN_STRING((char *)purple_core_get_version())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_get_ui, 0)

    RETURN_STRING((char *)purple_core_get_ui())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_core, 0)

    RETURN_NUMBER((long)purple_get_core())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_set_ui_ops, 1)
    T_ARRAY(purple_core_set_ui_ops, 0)     // PurpleCoreUiOps*

    purple_core_set_ui_ops(InitCoreOps(PARAMETER(0)));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_core_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_migrate, 0)

    RETURN_NUMBER(purple_core_migrate())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_ensure_single_instance, 0)

    RETURN_NUMBER(purple_core_ensure_single_instance())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_core_get_ui_info, 0)

    RETURN_NUMBER((long)purple_core_get_ui_info())
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_desktop_item_new_from_file,1)
        T_STRING(purple_core_get_ui_info, 0) // char*

        RETURN_NUMBER((long)purple_desktop_item_new_from_file((char*)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_desktop_item_get_entry_type,1)
        T_NUMBER(purple_desktop_item_get_entry_type, 0) // PurpleDesktopItem*

        RETURN_NUMBER(purple_desktop_item_get_entry_type((PurpleDesktopItem*)(long)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_desktop_item_get_string,2)
        T_NUMBER(purple_desktop_item_get_string, 0) // PurpleDesktopItem*
        T_STRING(purple_desktop_item_get_string, 1) // char*

        RETURN_STRING((char*)purple_desktop_item_get_string((PurpleDesktopItem*)(long)PARAM(0), (char*)PARAM(1)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_desktop_item_copy,1)
        T_NUMBER(purple_desktop_item_copy, 0) // PurpleDesktopItem*

        RETURN_NUMBER((long)purple_desktop_item_copy((PurpleDesktopItem*)(long)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_desktop_item_unref,1)
        T_NUMBER(purple_desktop_item_unref, 0) // PurpleDesktopItem*

        purple_desktop_item_unref((PurpleDesktopItem*)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_timeout_add, 3)
    T_NUMBER(purple_timeout_add, 0)     // guint
    T_NUMBER(purple_timeout_add, 1)     // GSourceFunc
    T_NUMBER(purple_timeout_add, 2)     // gpointer

    RETURN_NUMBER((long)purple_timeout_add((guint)PARAM(0), (GSourceFunc)add_seconds_F, (gpointer)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_timeout_add_seconds, 3)
    T_NUMBER(purple_timeout_add_seconds, 0)     // guint
    T_DELEGATE(purple_timeout_add_seconds, 1)   // GSourceFunc
//T_NUMBER(2) // gpointer

    RETURN_NUMBER(purple_timeout_add_seconds((guint)PARAM(0), add_seconds_F, (gpointer)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_timeout_remove, 1)
    T_NUMBER(purple_timeout_remove, 0)     // guint

    RETURN_NUMBER(purple_timeout_remove((guint)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_input_add, 3)
    T_NUMBER(purple_input_add, 0)     // int
    T_NUMBER(purple_input_add, 1)     // PurpleInputCondition
//T_NUMBER(2) // PurpleInputFunction
    T_DELEGATE(purple_input_add, 2)   // gpointer

    RETURN_NUMBER(purple_input_add((int)PARAM(0), (PurpleInputCondition)PARAM(1), PurpleInput_F, (gpointer)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_input_remove, 1)
    T_NUMBER(purple_input_remove, 0)     // guint

    RETURN_NUMBER(purple_input_remove((guint)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_input_get_error, 2)
    T_NUMBER(purple_input_get_error, 0)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER(purple_input_get_error((int)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_eventloop_set_ui_ops, 1)
    T_NUMBER(purple_eventloop_set_ui_ops, 0)     // PurpleEventLoopUiOps*

    purple_eventloop_set_ui_ops((PurpleEventLoopUiOps *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_eventloop_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_eventloop_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_new, 3)
    T_NUMBER(purple_xfer_new, 0)     // PurpleAccount*
    T_NUMBER(purple_xfer_new, 1)     // PurpleXferType
    T_STRING(purple_xfer_new, 2)     // char*

    RETURN_NUMBER((long)purple_xfer_new((PurpleAccount *)(long)PARAM(0), (PurpleXferType)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_get_all, 0)
    ListToArr(purple_xfers_get_all(), RESULT);
//RETURN_NUMBER((long)purple_xfers_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_ref, 1)
    T_NUMBER(purple_xfer_ref, 0)     // PurpleXfer*

    purple_xfer_ref((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_unref, 1)
    T_NUMBER(purple_xfer_unref, 0)     // PurpleXfer*

    purple_xfer_unref((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_request, 1)
    T_NUMBER(purple_xfer_request, 0)     // PurpleXfer*

    purple_xfer_request((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------;
CONCEPT_FUNCTION_IMPL(purple_xfer_request_accepted, 2)
    T_NUMBER(purple_xfer_request_accepted, 0)     // PurpleXfer*
    T_STRING(purple_xfer_request_accepted, 1)     // char*

    char *fname = PARAM(1);
    if ((fname) && (!fname[0]))
        fname = 0;

    purple_xfer_request_accepted((PurpleXfer *)(long)PARAM(0), (char *)fname);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_request_denied, 1)
    T_NUMBER(purple_xfer_request_denied, 0)     // PurpleXfer*

    purple_xfer_request_denied((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_type, 1)
    T_NUMBER(purple_xfer_get_type, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_type((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_account, 1)
    T_NUMBER(purple_xfer_get_account, 0)     // PurpleXfer*

    RETURN_NUMBER((long)purple_xfer_get_account((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_remote_user, 1)
    T_NUMBER(purple_xfer_get_remote_user, 0)     // PurpleXfer*

    RETURN_STRING((char *)purple_xfer_get_remote_user((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_status, 1)
    T_NUMBER(purple_xfer_get_status, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_status((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_is_canceled, 1)
    T_NUMBER(purple_xfer_is_canceled, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_is_canceled((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_is_completed, 1)
    T_NUMBER(purple_xfer_is_completed, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_is_completed((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_filename, 1)
    T_NUMBER(purple_xfer_get_filename, 0)     // PurpleXfer*

    RETURN_STRING((char *)purple_xfer_get_filename((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_local_filename, 1)
    T_NUMBER(purple_xfer_get_local_filename, 0)     // PurpleXfer*

    RETURN_STRING((char *)purple_xfer_get_local_filename((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_bytes_sent, 1)
    T_NUMBER(purple_xfer_get_bytes_sent, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_bytes_sent((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_bytes_remaining, 1)
    T_NUMBER(purple_xfer_get_bytes_remaining, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_bytes_remaining((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_size, 1)
    T_NUMBER(purple_xfer_get_size, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_size((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_progress, 1)
    T_NUMBER(purple_xfer_get_progress, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_progress((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_local_port, 1)
    T_NUMBER(purple_xfer_get_local_port, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_local_port((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_remote_ip, 1)
    T_NUMBER(purple_xfer_get_remote_ip, 0)     // PurpleXfer*

    RETURN_STRING((char *)purple_xfer_get_remote_ip((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_remote_port, 1)
    T_NUMBER(purple_xfer_get_remote_port, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_remote_port((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_start_time, 1)
    T_NUMBER(purple_xfer_get_start_time, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_start_time((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_end_time, 1)
    T_NUMBER(purple_xfer_get_end_time, 0)     // PurpleXfer*

    RETURN_NUMBER(purple_xfer_get_end_time((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_completed, 2)
    T_NUMBER(purple_xfer_set_completed, 0)     // PurpleXfer*
    T_NUMBER(purple_xfer_set_completed, 1)     // int

    purple_xfer_set_completed((PurpleXfer *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_message, 2)
    T_NUMBER(purple_xfer_set_message, 0)     // PurpleXfer*
    T_STRING(purple_xfer_set_message, 1)     // char*

    purple_xfer_set_message((PurpleXfer *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_filename, 2)
    T_NUMBER(purple_xfer_set_filename, 0)     // PurpleXfer*
    T_STRING(purple_xfer_set_filename, 1)     // char*

    purple_xfer_set_filename((PurpleXfer *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_local_filename, 2)
    T_NUMBER(purple_xfer_set_local_filename, 0)     // PurpleXfer*
    T_STRING(purple_xfer_set_local_filename, 1)     // char*

    purple_xfer_set_local_filename((PurpleXfer *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_size, 2)
    T_NUMBER(purple_xfer_set_size, 0)     // PurpleXfer*
    T_NUMBER(purple_xfer_set_size, 1)     // size_t

    purple_xfer_set_size((PurpleXfer *)(long)PARAM(0), (size_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_set_bytes_sent, 2)
    T_NUMBER(purple_xfer_set_bytes_sent, 0)     // PurpleXfer*
    T_NUMBER(purple_xfer_set_bytes_sent, 1)     // size_t

    purple_xfer_set_bytes_sent((PurpleXfer *)(long)PARAM(0), (size_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_get_ui_ops, 1)
    T_NUMBER(purple_xfer_get_ui_ops, 0)     // PurpleXfer*

    RETURN_NUMBER((long)purple_xfer_get_ui_ops((PurpleXfer *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_read, 2)
    T_NUMBER(purple_xfer_read, 0)     // PurpleXfer*

// ... parameter 1 is by reference (unsigned char**)
    unsigned char *local_parameter_1;

    RETURN_NUMBER(purple_xfer_read((PurpleXfer *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_write, 3)
    T_NUMBER(purple_xfer_write, 0)     // PurpleXfer*
    T_STRING(purple_xfer_write, 1)     // unsigned char*
    T_NUMBER(purple_xfer_write, 2)     // gsize

    RETURN_NUMBER(purple_xfer_write((PurpleXfer *)(long)PARAM(0), (unsigned char *)PARAM(1), (gsize)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_start, 4)
    T_NUMBER(purple_xfer_start, 0)     // PurpleXfer*
    T_NUMBER(purple_xfer_start, 1)     // int
    T_STRING(purple_xfer_start, 2)     // char*
    T_NUMBER(purple_xfer_start, 3)     // unsigned int

    purple_xfer_start((PurpleXfer *)(long)PARAM(0), (int)PARAM(1), (char *)PARAM(2), (unsigned int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_end, 1)
    T_NUMBER(purple_xfer_end, 0)     // PurpleXfer*

    purple_xfer_end((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_add, 1)
    T_NUMBER(purple_xfer_add, 0)     // PurpleXfer*

    purple_xfer_add((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_cancel_local, 1)
    T_NUMBER(purple_xfer_cancel_local, 0)     // PurpleXfer*

    purple_xfer_cancel_local((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_cancel_remote, 1)
    T_NUMBER(purple_xfer_cancel_remote, 0)     // PurpleXfer*

    purple_xfer_cancel_remote((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_error, 4)
    T_NUMBER(purple_xfer_error, 0)     // PurpleXferType
    T_NUMBER(purple_xfer_error, 1)     // PurpleAccount*
    T_STRING(purple_xfer_error, 2)     // char*
    T_STRING(purple_xfer_error, 3)     // char*

    purple_xfer_error((PurpleXferType)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_update_progress, 1)
    T_NUMBER(purple_xfer_update_progress, 0)     // PurpleXfer*

    purple_xfer_update_progress((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_conversation_write, 3)
    T_NUMBER(purple_xfer_conversation_write, 0)     // PurpleXfer*
    T_STRING(purple_xfer_conversation_write, 1)     // char*
    T_NUMBER(purple_xfer_conversation_write, 2)     // int

    purple_xfer_conversation_write((PurpleXfer *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_ui_ready, 1)
    T_NUMBER(purple_xfer_ui_ready, 0)     // PurpleXfer*

    purple_xfer_ui_ready((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfer_prpl_ready, 1)
    T_NUMBER(purple_xfer_prpl_ready, 0)     // PurpleXfer*

    purple_xfer_prpl_ready((PurpleXfer *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_get_handle, 0)

    RETURN_NUMBER((long)purple_xfers_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_init, 0)

    purple_xfers_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_uninit, 0)

    purple_xfers_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_set_ui_ops, 1)
    T_ARRAY(purple_xfers_set_ui_ops, 0)

    purple_xfers_set_ui_ops(InitXferOps(PARAMETER(0)));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_xfers_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_xfers_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_touch, 0)

    purple_idle_touch();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_set, 1)
    T_NUMBER(purple_idle_set, 0)     // time_t

    purple_idle_set((time_t)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_set_ui_ops, 1)
    T_NUMBER(purple_idle_set_ui_ops, 0)     // PurpleIdleUiOps*

    purple_idle_set_ui_ops((PurpleIdleUiOps *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_idle_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_init, 0)

    purple_idle_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_idle_uninit, 0)

    purple_idle_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_add, 3)
    T_NUMBER(purple_imgstore_add, 0)     // gpointer
    T_NUMBER(purple_imgstore_add, 1)     // size_t
    T_STRING(purple_imgstore_add, 2)     // char*

    RETURN_NUMBER((long)purple_imgstore_add((gpointer)PARAM_INT(0), (size_t)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_new_from_file, 1)
    T_STRING(purple_imgstore_new_from_file, 0)     // char*

    RETURN_NUMBER((long)purple_imgstore_new_from_file((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_add_with_id, 3)
    T_NUMBER(purple_imgstore_add_with_id, 0)     // gpointer
    T_NUMBER(purple_imgstore_add_with_id, 1)     // size_t
    T_STRING(purple_imgstore_add_with_id, 2)     // char*

    RETURN_NUMBER(purple_imgstore_add_with_id((gpointer)PARAM_INT(0), (size_t)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_find_by_id, 1)
    T_NUMBER(purple_imgstore_find_by_id, 0)     // int

    RETURN_NUMBER((long)purple_imgstore_find_by_id((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_get_data, 1)
    T_NUMBER(purple_imgstore_get_data, 0)     // PurpleStoredImage*

    RETURN_NUMBER((long)purple_imgstore_get_data((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_get_size, 1)
    T_NUMBER(purple_imgstore_get_size, 0)     // PurpleStoredImage*

    RETURN_NUMBER(purple_imgstore_get_size((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_get_filename, 1)
    T_NUMBER(purple_imgstore_get_filename, 0)     // PurpleStoredImage*

    RETURN_STRING((char *)purple_imgstore_get_filename((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_get_extension, 1)
    T_NUMBER(purple_imgstore_get_extension, 0)     // PurpleStoredImage*

    RETURN_STRING((char *)purple_imgstore_get_extension((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_ref, 1)
    T_NUMBER(purple_imgstore_ref, 0)     // PurpleStoredImage*

    RETURN_NUMBER((long)purple_imgstore_ref((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_unref, 1)
    T_NUMBER(purple_imgstore_unref, 0)     // PurpleStoredImage*

    RETURN_NUMBER((long)purple_imgstore_unref((PurpleStoredImage *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_ref_by_id, 1)
    T_NUMBER(purple_imgstore_ref_by_id, 0)     // int

    purple_imgstore_ref_by_id((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_unref_by_id, 1)
    T_NUMBER(purple_imgstore_unref_by_id, 0)     // int

    purple_imgstore_unref_by_id((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_get_handle, 0)

    RETURN_NUMBER((long)purple_imgstore_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_init, 0)

    purple_imgstore_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_imgstore_uninit, 0)

    purple_imgstore_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_new, 5)
    T_NUMBER(purple_log_new, 0)     // PurpleLogType
    T_STRING(purple_log_new, 1)     // char*
    T_NUMBER(purple_log_new, 2)     // PurpleAccount*
    T_NUMBER(purple_log_new, 3)     // PurpleConversation*
    T_NUMBER(purple_log_new, 4)     // time_t

// ... parameter 5 is by reference (tm*)
    tm local_parameter_5;

    RETURN_NUMBER((long)purple_log_new((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2), (PurpleConversation *)(long)PARAM(3), (time_t)PARAM(4), &local_parameter_5))
//SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_free, 1)
    T_NUMBER(purple_log_free, 0)     // PurpleLog*

    purple_log_free((PurpleLog *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_write, 5)
    T_NUMBER(purple_log_write, 0)     // PurpleLog*
    T_NUMBER(purple_log_write, 1)     // PurpleMessageFlags
    T_STRING(purple_log_write, 2)     // char*
    T_NUMBER(purple_log_write, 3)     // time_t
    T_STRING(purple_log_write, 4)     // char*

    purple_log_write((PurpleLog *)(long)PARAM(0), (PurpleMessageFlags)PARAM(1), (char *)PARAM(2), (time_t)PARAM(3), (char *)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_read, 2)
    T_NUMBER(purple_log_read, 0)     // PurpleLog*

// ... parameter 1 is by reference (PurpleLogReadFlags*)
    PurpleLogReadFlags local_parameter_1;

    RETURN_STRING((char *)purple_log_read((PurpleLog *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_logs, 3)
    T_NUMBER(purple_log_get_logs, 0)     // PurpleLogType
    T_STRING(purple_log_get_logs, 1)     // char*
    T_NUMBER(purple_log_get_logs, 2)     // PurpleAccount*

    RETURN_NUMBER((long)purple_log_get_logs((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_log_sets, 0)

    RETURN_NUMBER((long)purple_log_get_log_sets())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_system_logs, 1)
    T_NUMBER(purple_log_get_system_logs, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_log_get_system_logs((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_size, 1)
    T_NUMBER(purple_log_get_size, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_get_size((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_total_size, 3)
    T_NUMBER(purple_log_get_total_size, 0)     // PurpleLogType
    T_STRING(purple_log_get_total_size, 1)     // char*
    T_NUMBER(purple_log_get_total_size, 2)     // PurpleAccount*

    RETURN_NUMBER(purple_log_get_total_size((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_activity_score, 3)
    T_NUMBER(purple_log_get_activity_score, 0)     // PurpleLogType
    T_STRING(purple_log_get_activity_score, 1)     // char*
    T_NUMBER(purple_log_get_activity_score, 2)     // PurpleAccount*

    RETURN_NUMBER(purple_log_get_activity_score((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_is_deletable, 1)
    T_NUMBER(purple_log_is_deletable, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_is_deletable((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_delete, 1)
    T_NUMBER(purple_log_delete, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_delete((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_log_dir, 3)
    T_NUMBER(purple_log_get_log_dir, 0)     // PurpleLogType
    T_STRING(purple_log_get_log_dir, 1)     // char*
    T_NUMBER(purple_log_get_log_dir, 2)     // PurpleAccount*

    RETURN_STRING((char *)purple_log_get_log_dir((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_compare, 2)
    T_NUMBER(purple_log_compare, 0)     // void*
    T_NUMBER(purple_log_compare, 1)     // void*

    RETURN_NUMBER(purple_log_compare((void *)(long)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_set_compare, 2)
    T_NUMBER(purple_log_set_compare, 0)     // void*
    T_NUMBER(purple_log_set_compare, 1)     // void*

    RETURN_NUMBER(purple_log_set_compare((void *)(long)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_set_free, 1)
    T_NUMBER(purple_log_set_free, 0)

    purple_log_set_free((PurpleLogSet *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_writer, 2)
    T_NUMBER(purple_log_common_writer, 0)     // PurpleLog*
    T_STRING(purple_log_common_writer, 1)     // char*

    purple_log_common_writer((PurpleLog *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_lister, 5)
    T_NUMBER(purple_log_common_lister, 0)     // PurpleLogType
    T_STRING(purple_log_common_lister, 1)     // char*
    T_NUMBER(purple_log_common_lister, 2)     // PurpleAccount*
    T_STRING(purple_log_common_lister, 3)     // char*
    T_NUMBER(purple_log_common_lister, 4)     // PurpleLogLogger*

    RETURN_NUMBER((long)purple_log_common_lister((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2), (char *)PARAM(3), (PurpleLogLogger *)(long)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_total_sizer, 4)
    T_NUMBER(purple_log_common_total_sizer, 0)     // PurpleLogType
    T_STRING(purple_log_common_total_sizer, 1)     // char*
    T_NUMBER(purple_log_common_total_sizer, 2)     // PurpleAccount*
    T_STRING(purple_log_common_total_sizer, 3)     // char*

    RETURN_NUMBER(purple_log_common_total_sizer((PurpleLogType)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2), (char *)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_sizer, 1)
    T_NUMBER(purple_log_common_sizer, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_common_sizer((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_deleter, 1)
    T_NUMBER(purple_log_common_deleter, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_common_deleter((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_common_is_deletable, 1)
    T_NUMBER(purple_log_common_is_deletable, 0)     // PurpleLog*

    RETURN_NUMBER(purple_log_common_is_deletable((PurpleLog *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_new, 3)
    T_STRING(purple_log_logger_new, 0)     // char*
    T_STRING(purple_log_logger_new, 1)     // char*
    T_NUMBER(purple_log_logger_new, 2)     // int

    RETURN_NUMBER((long)purple_log_logger_new((char *)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_free, 1)
    T_NUMBER(purple_log_logger_free, 0)     // PurpleLogLogger*

    purple_log_logger_free((PurpleLogLogger *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_add, 1)
    T_NUMBER(purple_log_logger_add, 0)     // PurpleLogLogger*

    purple_log_logger_add((PurpleLogLogger *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_remove, 1)
    T_NUMBER(purple_log_logger_remove, 0)     // PurpleLogLogger*

    purple_log_logger_remove((PurpleLogLogger *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_set, 1)
    T_NUMBER(purple_log_logger_set, 0)     // PurpleLogLogger*

    purple_log_logger_set((PurpleLogLogger *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_get, 0)

    RETURN_NUMBER((long)purple_log_logger_get())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_logger_get_options, 0)

    RETURN_NUMBER((long)purple_log_logger_get_options())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_init, 0)

    purple_log_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_get_handle, 0)

    RETURN_NUMBER((long)purple_log_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_log_uninit, 0)

    purple_log_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_session_type_get_type, 0)

    RETURN_NUMBER(purple_media_session_type_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_type_get_type, 0)

    RETURN_NUMBER(purple_media_candidate_type_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_network_protocol_get_type, 0)

    RETURN_NUMBER(purple_media_network_protocol_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_type, 0)

    RETURN_NUMBER(purple_media_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_state_changed_get_type, 0)

    RETURN_NUMBER(purple_media_state_changed_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_info_type_get_type, 0)

    RETURN_NUMBER(purple_media_info_type_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_type, 0)

    RETURN_NUMBER(purple_media_candidate_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_new, 6)
    T_STRING(purple_media_candidate_new, 0)     // char*
    T_NUMBER(purple_media_candidate_new, 1)     // guint
    T_NUMBER(purple_media_candidate_new, 2)     // PurpleMediaCandidateType
    T_NUMBER(purple_media_candidate_new, 3)     // PurpleMediaNetworkProtocol
    T_STRING(purple_media_candidate_new, 4)     // char*
    T_NUMBER(purple_media_candidate_new, 5)     // guint

    RETURN_NUMBER((long)purple_media_candidate_new((char *)PARAM(0), (guint)PARAM(1), (PurpleMediaCandidateType)PARAM(2), (PurpleMediaNetworkProtocol)PARAM(3), (char *)PARAM(4), (guint)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_list_copy, 1)
    T_NUMBER(purple_media_candidate_list_copy, 0)     // GList*

    RETURN_NUMBER((long)purple_media_candidate_list_copy((GList *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_list_free, 1)
    T_NUMBER(purple_media_candidate_list_free, 0)     // GList*

    purple_media_candidate_list_free((GList *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_foundation, 1)
    T_NUMBER(purple_media_candidate_get_foundation, 0)     // PurpleMediaCandidate*

    RETURN_STRING((char *)purple_media_candidate_get_foundation((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_component_id, 1)
    T_NUMBER(purple_media_candidate_get_component_id, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_component_id((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_ip, 1)
    T_NUMBER(purple_media_candidate_get_ip, 0)     // PurpleMediaCandidate*

    RETURN_STRING((char *)purple_media_candidate_get_ip((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_port, 1)
    T_NUMBER(purple_media_candidate_get_port, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_port((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_base_ip, 1)
    T_NUMBER(purple_media_candidate_get_base_ip, 0)     // PurpleMediaCandidate*

    RETURN_STRING((char *)purple_media_candidate_get_base_ip((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_base_port, 1)
    T_NUMBER(purple_media_candidate_get_base_port, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_base_port((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_protocol, 1)
    T_NUMBER(purple_media_candidate_get_protocol, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_protocol((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_priority, 1)
    T_NUMBER(purple_media_candidate_get_priority, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_priority((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_candidate_type, 1)
    T_NUMBER(purple_media_candidate_get_candidate_type, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_candidate_type((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_username, 1)
    T_NUMBER(purple_media_candidate_get_username, 0)     // PurpleMediaCandidate*

    RETURN_STRING((char *)purple_media_candidate_get_username((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_password, 1)
    T_NUMBER(purple_media_candidate_get_password, 0)     // PurpleMediaCandidate*

    RETURN_STRING((char *)purple_media_candidate_get_password((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidate_get_ttl, 1)
    T_NUMBER(purple_media_candidate_get_ttl, 0)     // PurpleMediaCandidate*

    RETURN_NUMBER(purple_media_candidate_get_ttl((PurpleMediaCandidate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_type, 0)

    RETURN_NUMBER(purple_media_codec_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_new, 4)
    T_NUMBER(purple_media_codec_new, 0)     // int
    T_STRING(purple_media_codec_new, 1)     // char*
    T_NUMBER(purple_media_codec_new, 2)     // PurpleMediaSessionType
    T_NUMBER(purple_media_codec_new, 3)     // guint

    RETURN_NUMBER((long)purple_media_codec_new((int)PARAM(0), (char *)PARAM(1), (PurpleMediaSessionType)PARAM(2), (guint)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_id, 1)
    T_NUMBER(purple_media_codec_get_id, 0)     // PurpleMediaCodec*

    RETURN_NUMBER(purple_media_codec_get_id((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_encoding_name, 1)
    T_NUMBER(purple_media_codec_get_encoding_name, 0)     // PurpleMediaCodec*

    RETURN_STRING((char *)purple_media_codec_get_encoding_name((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_clock_rate, 1)
    T_NUMBER(purple_media_codec_get_clock_rate, 0)     // PurpleMediaCodec*

    RETURN_NUMBER(purple_media_codec_get_clock_rate((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_channels, 1)
    T_NUMBER(purple_media_codec_get_channels, 0)     // PurpleMediaCodec*

    RETURN_NUMBER(purple_media_codec_get_channels((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_optional_parameters, 1)
    T_NUMBER(purple_media_codec_get_optional_parameters, 0)     // PurpleMediaCodec*

    RETURN_NUMBER((long)purple_media_codec_get_optional_parameters((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_to_string, 1)
    T_NUMBER(purple_media_codec_to_string, 0)     // PurpleMediaCodec*

    RETURN_STRING((char *)purple_media_codec_to_string((PurpleMediaCodec *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_add_optional_parameter, 3)
    T_NUMBER(purple_media_codec_add_optional_parameter, 0)     // PurpleMediaCodec*
    T_STRING(purple_media_codec_add_optional_parameter, 1)     // char*
    T_STRING(purple_media_codec_add_optional_parameter, 2)     // char*

    purple_media_codec_add_optional_parameter((PurpleMediaCodec *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_remove_optional_parameter, 2)
    T_NUMBER(purple_media_codec_remove_optional_parameter, 0)     // PurpleMediaCodec*
    T_NUMBER(purple_media_codec_remove_optional_parameter, 1)

    purple_media_codec_remove_optional_parameter((PurpleMediaCodec *)(long)PARAM(0), (PurpleKeyValuePair *)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_get_optional_parameter, 3)
    T_NUMBER(purple_media_codec_get_optional_parameter, 0)     // PurpleMediaCodec*
    T_STRING(purple_media_codec_get_optional_parameter, 1)     // char*
    T_STRING(purple_media_codec_get_optional_parameter, 2)     // char*

    RETURN_NUMBER((long)purple_media_codec_get_optional_parameter((PurpleMediaCodec *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_list_copy, 1)
    T_NUMBER(purple_media_codec_list_copy, 0)     // GList*

    RETURN_NUMBER((long)purple_media_codec_list_copy((GList *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codec_list_free, 1)
    T_NUMBER(purple_media_codec_list_free, 0)     // GList*

    purple_media_codec_list_free((GList *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_session_ids, 1)
    T_NUMBER(purple_media_get_session_ids, 0)     // PurpleMedia*

    RETURN_NUMBER((long)purple_media_get_session_ids((PurpleMedia *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_account, 1)
    T_NUMBER(purple_media_get_account, 0)     // PurpleMedia*

    RETURN_NUMBER((long)purple_media_get_account((PurpleMedia *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_prpl_data, 1)
    T_NUMBER(purple_media_get_prpl_data, 0)     // PurpleMedia*

    RETURN_NUMBER((long)purple_media_get_prpl_data((PurpleMedia *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_prpl_data, 2)
    T_NUMBER(purple_media_set_prpl_data, 0)     // PurpleMedia*
    T_NUMBER(purple_media_set_prpl_data, 1)     // void*

    purple_media_set_prpl_data((PurpleMedia *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_error, 2)
    T_NUMBER(purple_media_error, 0)     // PurpleMedia*
    T_STRING(purple_media_error, 1)     // char*

    purple_media_error((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_end, 3)
    T_NUMBER(purple_media_end, 0)     // PurpleMedia*
    T_STRING(purple_media_end, 1)     // char*
    T_STRING(purple_media_end, 2)     // char*

    purple_media_end((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_stream_info, 5)
    T_NUMBER(purple_media_stream_info, 0)     // PurpleMedia*
    T_NUMBER(purple_media_stream_info, 1)     // PurpleMediaInfoType
    T_STRING(purple_media_stream_info, 2)     // char*
    T_STRING(purple_media_stream_info, 3)     // char*
    T_NUMBER(purple_media_stream_info, 4)     // int

    purple_media_stream_info((PurpleMedia *)(long)PARAM(0), (PurpleMediaInfoType)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_add_stream, 8)
    T_NUMBER(purple_media_add_stream, 0)     // PurpleMedia*
    T_STRING(purple_media_add_stream, 1)     // char*
    T_STRING(purple_media_add_stream, 2)     // char*
    T_NUMBER(purple_media_add_stream, 3)     // PurpleMediaSessionType
    T_NUMBER(purple_media_add_stream, 4)     // int
    T_STRING(purple_media_add_stream, 5)     // char*
    T_NUMBER(purple_media_add_stream, 6)     // guint
    T_NUMBER(purple_media_add_stream, 7)

    RETURN_NUMBER(purple_media_add_stream((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (PurpleMediaSessionType)PARAM(3), (int)PARAM(4), (char *)PARAM(5), (guint)PARAM(6), (GParameter *)PARAM_INT(7)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_session_type, 2)
    T_NUMBER(purple_media_get_session_type, 0)     // PurpleMedia*
    T_STRING(purple_media_get_session_type, 1)     // char*

    RETURN_NUMBER(purple_media_get_session_type((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_manager, 1)
    T_NUMBER(purple_media_get_manager, 0)     // PurpleMedia*

    RETURN_NUMBER((long)purple_media_get_manager((PurpleMedia *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_codecs, 2)
    T_NUMBER(purple_media_get_codecs, 0)     // PurpleMedia*
    T_STRING(purple_media_get_codecs, 1)     // char*

    RETURN_NUMBER((long)purple_media_get_codecs((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_add_remote_candidates, 4)
    T_NUMBER(purple_media_add_remote_candidates, 0)     // PurpleMedia*
    T_STRING(purple_media_add_remote_candidates, 1)     // char*
    T_STRING(purple_media_add_remote_candidates, 2)     // char*
    T_NUMBER(purple_media_add_remote_candidates, 3)     // GList*

    purple_media_add_remote_candidates((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (GList *)(long)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_get_local_candidates, 3)
    T_NUMBER(purple_media_get_local_candidates, 0)     // PurpleMedia*
    T_STRING(purple_media_get_local_candidates, 1)     // char*
    T_STRING(purple_media_get_local_candidates, 2)     // char*

    RETURN_NUMBER((long)purple_media_get_local_candidates((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_remote_codecs, 4)
    T_NUMBER(purple_media_set_remote_codecs, 0)     // PurpleMedia*
    T_STRING(purple_media_set_remote_codecs, 1)     // char*
    T_STRING(purple_media_set_remote_codecs, 2)     // char*
    T_NUMBER(purple_media_set_remote_codecs, 3)     // GList*

    RETURN_NUMBER(purple_media_set_remote_codecs((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (GList *)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_candidates_prepared, 3)
    T_NUMBER(purple_media_candidates_prepared, 0)     // PurpleMedia*
    T_STRING(purple_media_candidates_prepared, 1)     // char*
    T_STRING(purple_media_candidates_prepared, 2)     // char*

    RETURN_NUMBER(purple_media_candidates_prepared((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_send_codec, 3)
    T_NUMBER(purple_media_set_send_codec, 0)     // PurpleMedia*
    T_STRING(purple_media_set_send_codec, 1)     // char*
    T_NUMBER(purple_media_set_send_codec, 2)     // PurpleMediaCodec*

    RETURN_NUMBER(purple_media_set_send_codec((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (PurpleMediaCodec *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_codecs_ready, 2)
    T_NUMBER(purple_media_codecs_ready, 0)     // PurpleMedia*
    T_STRING(purple_media_codecs_ready, 1)     // char*

    RETURN_NUMBER(purple_media_codecs_ready((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_is_initiator, 3)
    T_NUMBER(purple_media_is_initiator, 0)     // PurpleMedia*
    T_STRING(purple_media_is_initiator, 1)     // char*
    T_STRING(purple_media_is_initiator, 2)     // char*

    RETURN_NUMBER(purple_media_is_initiator((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_accepted, 3)
    T_NUMBER(purple_media_accepted, 0)     // PurpleMedia*
    T_STRING(purple_media_accepted, 1)     // char*
    T_STRING(purple_media_accepted, 2)     // char*

    RETURN_NUMBER(purple_media_accepted((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_input_volume, 3)
    T_NUMBER(purple_media_set_input_volume, 0)     // PurpleMedia*
    T_STRING(purple_media_set_input_volume, 1)     // char*
    T_NUMBER(purple_media_set_input_volume, 2)     // double

    purple_media_set_input_volume((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_output_volume, 4)
    T_NUMBER(purple_media_set_output_volume, 0)     // PurpleMedia*
    T_STRING(purple_media_set_output_volume, 1)     // char*
    T_STRING(purple_media_set_output_volume, 2)     // char*
    T_NUMBER(purple_media_set_output_volume, 3)     // double

    purple_media_set_output_volume((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (double)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_set_output_window, 4)
    T_NUMBER(purple_media_set_output_window, 0)     // PurpleMedia*
    T_STRING(purple_media_set_output_window, 1)     // char*
    T_STRING(purple_media_set_output_window, 2)     // char*
    T_NUMBER(purple_media_set_output_window, 3)     // gulong

    RETURN_NUMBER(purple_media_set_output_window((PurpleMedia *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (gulong)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_remove_output_windows, 1)
    T_NUMBER(purple_media_remove_output_windows, 0)     // PurpleMedia*

    purple_media_remove_output_windows((PurpleMedia *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_get_type, 0)

    RETURN_NUMBER(purple_media_manager_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_get, 0)

    RETURN_NUMBER((long)purple_media_manager_get())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_create_media, 5)
    T_NUMBER(purple_media_manager_create_media, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_create_media, 1)     // PurpleAccount*
    T_STRING(purple_media_manager_create_media, 2)     // char*
    T_STRING(purple_media_manager_create_media, 3)     // char*
    T_NUMBER(purple_media_manager_create_media, 4)     // int

    RETURN_NUMBER((long)purple_media_manager_create_media((PurpleMediaManager *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_get_media, 1)
    T_NUMBER(purple_media_manager_get_media, 0)     // PurpleMediaManager*

    RETURN_NUMBER((long)purple_media_manager_get_media((PurpleMediaManager *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_get_media_by_account, 2)
    T_NUMBER(purple_media_manager_get_media_by_account, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_get_media_by_account, 1)     // PurpleAccount*

    RETURN_NUMBER((long)purple_media_manager_get_media_by_account((PurpleMediaManager *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_remove_media, 2)
    T_NUMBER(purple_media_manager_remove_media, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_remove_media, 1)     // PurpleMedia*

    purple_media_manager_remove_media((PurpleMediaManager *)(long)PARAM(0), (PurpleMedia *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_create_output_window, 4)
    T_NUMBER(purple_media_manager_create_output_window, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_create_output_window, 1)     // PurpleMedia*
    T_STRING(purple_media_manager_create_output_window, 2)     // char*
    T_STRING(purple_media_manager_create_output_window, 3)     // char*

    RETURN_NUMBER(purple_media_manager_create_output_window((PurpleMediaManager *)(long)PARAM(0), (PurpleMedia *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_set_output_window, 5)
    T_NUMBER(purple_media_manager_set_output_window, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_set_output_window, 1)     // PurpleMedia*
    T_STRING(purple_media_manager_set_output_window, 2)     // char*
    T_STRING(purple_media_manager_set_output_window, 3)     // char*
    T_NUMBER(purple_media_manager_set_output_window, 4)     // gulong

    RETURN_NUMBER(purple_media_manager_set_output_window((PurpleMediaManager *)(long)PARAM(0), (PurpleMedia *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (gulong)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_remove_output_window, 2)
    T_NUMBER(purple_media_manager_remove_output_window, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_remove_output_window, 1)     // gulong

    RETURN_NUMBER(purple_media_manager_remove_output_window((PurpleMediaManager *)(long)PARAM(0), (gulong)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_remove_output_windows, 4)
    T_NUMBER(purple_media_manager_remove_output_windows, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_remove_output_windows, 1)     // PurpleMedia*
    T_STRING(purple_media_manager_remove_output_windows, 2)     // char*
    T_STRING(purple_media_manager_remove_output_windows, 3)     // char*

    purple_media_manager_remove_output_windows((PurpleMediaManager *)(long)PARAM(0), (PurpleMedia *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_set_ui_caps, 2)
    T_NUMBER(purple_media_manager_set_ui_caps, 0)     // PurpleMediaManager*
    T_NUMBER(purple_media_manager_set_ui_caps, 1)     // PurpleMediaCaps

    purple_media_manager_set_ui_caps((PurpleMediaManager *)(long)PARAM(0), (PurpleMediaCaps)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_media_manager_get_ui_caps, 1)
    T_NUMBER(purple_media_manager_get_ui_caps, 0)     // PurpleMediaManager*

    RETURN_NUMBER(purple_media_manager_get_ui_caps((PurpleMediaManager *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_new, 0)

    RETURN_NUMBER((long)purple_mime_document_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_free, 1)
    T_NUMBER(purple_mime_document_free, 0)     // PurpleMimeDocument*

    purple_mime_document_free((PurpleMimeDocument *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_parse, 1)
    T_STRING(purple_mime_document_parse, 0)     // char*

    RETURN_NUMBER((long)purple_mime_document_parse((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_parsen, 2)
    T_STRING(purple_mime_document_parsen, 0)     // char*
    T_NUMBER(purple_mime_document_parsen, 1)     // gsize

    RETURN_NUMBER((long)purple_mime_document_parsen((char *)PARAM(0), (gsize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_write, 2)
    T_NUMBER(purple_mime_document_write, 0)     // PurpleMimeDocument*
    T_STRING(purple_mime_document_write, 1)

// ... parameter 1 is by reference (GString*)
    GString * local_parameter_1 = g_string_new_len(PARAM(1), PARAM_LEN(1));

    purple_mime_document_write((PurpleMimeDocument *)(long)PARAM(0), local_parameter_1);

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_get_fields, 1)
    T_NUMBER(purple_mime_document_get_fields, 0)     // PurpleMimeDocument*

    RETURN_NUMBER((long)purple_mime_document_get_fields((PurpleMimeDocument *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_get_field, 2)
    T_NUMBER(purple_mime_document_get_field, 0)     // PurpleMimeDocument*
    T_STRING(purple_mime_document_get_field, 1)     // char*

    RETURN_STRING((char *)purple_mime_document_get_field((PurpleMimeDocument *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_set_field, 3)
    T_NUMBER(purple_mime_document_set_field, 0)     // PurpleMimeDocument*
    T_STRING(purple_mime_document_set_field, 1)     // char*
    T_STRING(purple_mime_document_set_field, 2)     // char*

    purple_mime_document_set_field((PurpleMimeDocument *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_document_get_parts, 1)
    T_NUMBER(purple_mime_document_get_parts, 0)     // PurpleMimeDocument*

    RETURN_NUMBER((long)purple_mime_document_get_parts((PurpleMimeDocument *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_new, 1)
    T_NUMBER(purple_mime_part_new, 0)     // PurpleMimeDocument*

    RETURN_NUMBER((long)purple_mime_part_new((PurpleMimeDocument *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_fields, 1)
    T_NUMBER(purple_mime_part_get_fields, 0)     // PurpleMimePart*

    RETURN_NUMBER((long)purple_mime_part_get_fields((PurpleMimePart *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_field, 2)
    T_NUMBER(purple_mime_part_get_field, 0)     // PurpleMimePart*
    T_STRING(purple_mime_part_get_field, 1)     // char*

    RETURN_STRING((char *)purple_mime_part_get_field((PurpleMimePart *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_field_decoded, 2)
    T_NUMBER(purple_mime_part_get_field_decoded, 0)     // PurpleMimePart*
    T_STRING(purple_mime_part_get_field_decoded, 1)     // char*

    RETURN_STRING((char *)purple_mime_part_get_field_decoded((PurpleMimePart *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_set_field, 3)
    T_NUMBER(purple_mime_part_set_field, 0)     // PurpleMimePart*
    T_STRING(purple_mime_part_set_field, 1)     // char*
    T_STRING(purple_mime_part_set_field, 2)     // char*

    purple_mime_part_set_field((PurpleMimePart *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_data, 1)
    T_NUMBER(purple_mime_part_get_data, 0)     // PurpleMimePart*

    RETURN_STRING((char *)purple_mime_part_get_data((PurpleMimePart *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_data_decoded, 3)
    T_NUMBER(purple_mime_part_get_data_decoded, 0)     // PurpleMimePart*

// ... parameter 1 is by reference (unsigned char**)
    unsigned char *local_parameter_1;
// ... parameter 2 is by reference (gsize*)
    gsize local_parameter_2;

    purple_mime_part_get_data_decoded((PurpleMimePart *)(long)PARAM(0), &local_parameter_1, &local_parameter_2);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_get_length, 1)
    T_NUMBER(purple_mime_part_get_length, 0)     // PurpleMimePart*

    RETURN_NUMBER(purple_mime_part_get_length((PurpleMimePart *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_part_set_data, 2)
    T_NUMBER(purple_mime_part_set_data, 0)     // PurpleMimePart*
    T_STRING(purple_mime_part_set_data, 1)     // char*

    purple_mime_part_set_data((PurpleMimePart *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_pmp_init,0)

        purple_pmp_init();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_pmp_get_public_ip,0)

        RETURN_STRING((char*)purple_pmp_get_public_ip())
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_pmp_create_map,4)
        T_NUMBER(purple_pmp_create_map, 0) // PurplePmpType
        T_NUMBER(purple_pmp_create_map, 1) // unsigned short
        T_NUMBER(purple_pmp_create_map, 2) // unsigned short
        T_NUMBER(purple_pmp_create_map, 3) // int

        RETURN_NUMBER(purple_pmp_create_map((PurplePmpType)PARAM(0), (unsigned short)PARAM(1), (unsigned short)PARAM(2), (int)PARAM(3)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_pmp_destroy_map,2)
        T_NUMBER(purple_pmp_destroy_map, 0) // PurplePmpType
        T_NUMBER(purple_pmp_destroy_map, 1) // unsigned short

        RETURN_NUMBER(purple_pmp_destroy_map((PurplePmpType)PARAM(0), (unsigned short)PARAM(1)))
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_ip_atoi, 1)
    T_STRING(purple_network_ip_atoi, 0)     // char*

    RETURN_STRING((char *)purple_network_ip_atoi((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_set_public_ip, 1)
    T_STRING(purple_network_set_public_ip, 0)     // char*

    purple_network_set_public_ip((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_public_ip, 0)

    RETURN_STRING((char *)purple_network_get_public_ip())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_local_system_ip, 1)
    T_NUMBER(purple_network_get_local_system_ip, 0)     // int

    RETURN_STRING((char *)purple_network_get_local_system_ip((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_my_ip, 1)
    T_NUMBER(purple_network_get_my_ip, 0)     // int

    RETURN_STRING((char *)purple_network_get_my_ip((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_listen_map_external, 1)
    T_NUMBER(purple_network_listen_map_external, 0)     // int

    purple_network_listen_map_external((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_listen, 3)
    T_NUMBER(purple_network_listen, 0)     // unsigned short
    T_NUMBER(purple_network_listen, 1)     // int
    T_DELEGATE(purple_network_listen, 2)   // PurpleNetworkListenCallback

    RETURN_NUMBER((long)purple_network_listen((unsigned short)PARAM(0), (int)PARAM(1), PurpleNetworkListenCallback_F, (void *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_listen_range, 4)
    T_NUMBER(purple_network_listen_range, 0)     // unsigned short
    T_NUMBER(purple_network_listen_range, 1)     // unsigned short
    T_NUMBER(purple_network_listen_range, 2)     // int
    T_DELEGATE(purple_network_listen_range, 3)   // PurpleNetworkListenCallback

    RETURN_NUMBER((long)purple_network_listen_range((unsigned short)PARAM(0), (unsigned short)PARAM(1), (int)PARAM(2), PurpleNetworkListenCallback_F, (void *)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_listen_cancel, 1)
    T_NUMBER(purple_network_listen_cancel, 0)     // PurpleNetworkListenData*

    purple_network_listen_cancel((PurpleNetworkListenData *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_port_from_fd, 1)
    T_NUMBER(purple_network_get_port_from_fd, 0)     // int

    RETURN_NUMBER(purple_network_get_port_from_fd((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_is_available, 0)

    RETURN_NUMBER(purple_network_is_available())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_force_online, 0)

    purple_network_force_online();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_handle, 0)

    RETURN_NUMBER((long)purple_network_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_set_stun_server, 1)
    T_STRING(purple_network_set_stun_server, 0)     // char*

    purple_network_set_stun_server((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_stun_ip, 0)

    RETURN_STRING((char *)purple_network_get_stun_ip())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_set_turn_server, 1)
    T_STRING(purple_network_set_turn_server, 0)     // char*

    purple_network_set_turn_server((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_get_turn_ip, 0)

    RETURN_STRING((char *)purple_network_get_turn_ip())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_remove_port_mapping, 1)
    T_NUMBER(purple_network_remove_port_mapping, 0)     // int

    purple_network_remove_port_mapping((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_convert_idn_to_ascii, 2)
    T_STRING(purple_network_convert_idn_to_ascii, 0)     // char*

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(purple_network_convert_idn_to_ascii((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_init, 0)

    purple_network_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_network_uninit, 0)

    purple_network_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults, 6)
    T_NUMBER(purple_notify_searchresults, 0)     // PurpleConnection*
    T_STRING(purple_notify_searchresults, 1)     // char*
    T_STRING(purple_notify_searchresults, 2)     // char*
    T_STRING(purple_notify_searchresults, 3)     // char*
    T_NUMBER(purple_notify_searchresults, 4)     // PurpleNotifySearchResults*
    T_DELEGATE(purple_notify_searchresults, 5)   // PurpleNotifyCloseCallback
//T_NUMBER(6) // void*

    RETURN_NUMBER((long)purple_notify_searchresults((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (PurpleNotifySearchResults *)(long)PARAM(4), generic_delegate, (void *)(long)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_free, 1)
    T_NUMBER(purple_notify_searchresults_free, 0)     // PurpleNotifySearchResults*

    purple_notify_searchresults_free((PurpleNotifySearchResults *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_new_rows, 3)
    T_NUMBER(purple_notify_searchresults_new_rows, 0)     // PurpleConnection*
    T_NUMBER(purple_notify_searchresults_new_rows, 1)     // PurpleNotifySearchResults*
    T_NUMBER(purple_notify_searchresults_new_rows, 2)     // void*

    purple_notify_searchresults_new_rows((PurpleConnection *)(long)PARAM(0), (PurpleNotifySearchResults *)(long)PARAM(1), (void *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_button_add, 3)
    T_NUMBER(purple_notify_searchresults_button_add, 0)     // PurpleNotifySearchResults*
    T_NUMBER(purple_notify_searchresults_button_add, 1)     // PurpleNotifySearchButtonType
    T_DELEGATE(purple_notify_searchresults_button_add, 2)   // PurpleNotifySearchResultsCallback

    PurpleNotifySearchResultsCallback_delegate = PARAM(2);
    purple_notify_searchresults_button_add((PurpleNotifySearchResults *)(long)PARAM(0), (PurpleNotifySearchButtonType)PARAM(1), PurpleNotifySearchResultsCallback_F);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_button_add_labeled, 3)
    T_NUMBER(purple_notify_searchresults_button_add_labeled, 0)     // PurpleNotifySearchResults*
    T_STRING(purple_notify_searchresults_button_add_labeled, 1)     // char*
    T_DELEGATE(purple_notify_searchresults_button_add_labeled, 2)   // PurpleNotifySearchResultsCallback

    PurpleNotifySearchResultsCallback_delegate = PARAM(2);

    purple_notify_searchresults_button_add_labeled((PurpleNotifySearchResults *)(long)PARAM(0), (char *)PARAM(1), PurpleNotifySearchResultsCallback_F);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_new, 0)

    RETURN_NUMBER((long)purple_notify_searchresults_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_column_new, 1)
    T_STRING(purple_notify_searchresults_column_new, 0)     // char*

    RETURN_NUMBER((long)purple_notify_searchresults_column_new((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_column_add, 2)
    T_NUMBER(purple_notify_searchresults_column_add, 0)     // PurpleNotifySearchResults*
    T_NUMBER(purple_notify_searchresults_column_add, 1)     // PurpleNotifySearchColumn*

    purple_notify_searchresults_column_add((PurpleNotifySearchResults *)(long)PARAM(0), (PurpleNotifySearchColumn *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_searchresults_row_add, 2)
    T_NUMBER(purple_notify_searchresults_row_add, 0)     // PurpleNotifySearchResults*
    T_NUMBER(purple_notify_searchresults_row_add, 1)     // GList*

    purple_notify_searchresults_row_add((PurpleNotifySearchResults *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_message, 6)
    T_NUMBER(purple_notify_message, 0)     // void*
    T_NUMBER(purple_notify_message, 1)     // PurpleNotifyMsgType
    T_STRING(purple_notify_message, 2)     // char*
    T_STRING(purple_notify_message, 3)     // char*
    T_STRING(purple_notify_message, 4)     // char*
    T_DELEGATE(purple_notify_message, 5)   // PurpleNotifyCloseCallback

    RETURN_NUMBER((long)purple_notify_message((void *)(long)PARAM(0), (PurpleNotifyMsgType)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (char *)PARAM(4), generic_delegate, (void *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_email, 6)
    T_NUMBER(purple_notify_email, 0)     // void*
    T_STRING(purple_notify_email, 1)     // char*
    T_STRING(purple_notify_email, 2)     // char*
    T_STRING(purple_notify_email, 3)     // char*
    T_STRING(purple_notify_email, 4)     // char*
    T_DELEGATE(purple_notify_email, 5)   // PurpleNotifyCloseCallback
//T_NUMBER(6) // void*

    RETURN_NUMBER((long)purple_notify_email((void *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (char *)PARAM(4), generic_delegate, (void *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_emails, 8)
    T_NUMBER(purple_notify_emails, 0)     // void*
    T_NUMBER(purple_notify_emails, 1)     // size_t
    T_NUMBER(purple_notify_emails, 2)     // int
    T_DELEGATE(purple_notify_emails, 7)   // PurpleNotifyCloseCallback
//T_NUMBER(8) // void*

// ... parameter 3 is by reference (char**)
    const char *local_parameter_3 = 0;
// ... parameter 4 is by reference (char**)
    const char *local_parameter_4 = 0;
// ... parameter 5 is by reference (char**)
    const char *local_parameter_5 = 0;
// ... parameter 6 is by reference (char**)
    const char *local_parameter_6 = 0;

    RETURN_NUMBER((long)purple_notify_emails((void *)(long)PARAM(0), (size_t)PARAM(1), (int)PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6, generic_delegate, (void *)PARAM(7)))
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_formatted, 6)
    T_NUMBER(purple_notify_formatted, 0)     // void*
    T_STRING(purple_notify_formatted, 1)     // char*
    T_STRING(purple_notify_formatted, 2)     // char*
    T_STRING(purple_notify_formatted, 3)     // char*
    T_STRING(purple_notify_formatted, 4)     // char*
    T_DELEGATE(purple_notify_formatted, 5)   // PurpleNotifyCloseCallback
//T_NUMBER(6) // void*

    RETURN_NUMBER((long)purple_notify_formatted((void *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (char *)PARAM(4), generic_delegate, (void *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_userinfo, 4)
    T_NUMBER(purple_notify_userinfo, 0)     // PurpleConnection*
    T_STRING(purple_notify_userinfo, 1)     // char*
    T_NUMBER(purple_notify_userinfo, 2)     // PurpleNotifyUserInfo*
    T_DELEGATE(purple_notify_userinfo, 3)   // PurpleNotifyCloseCallback
//T_NUMBER(4) // void*

    RETURN_NUMBER((long)purple_notify_userinfo((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (PurpleNotifyUserInfo *)(long)PARAM(2), generic_delegate, (void *)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_new, 0)

    RETURN_NUMBER((long)purple_notify_user_info_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_destroy, 1)
    T_NUMBER(purple_notify_user_info_destroy, 0)     // PurpleNotifyUserInfo*

    purple_notify_user_info_destroy((PurpleNotifyUserInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_get_entries, 1)
    T_NUMBER(purple_notify_user_info_get_entries, 0)     // PurpleNotifyUserInfo*

    RETURN_NUMBER((long)purple_notify_user_info_get_entries((PurpleNotifyUserInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_get_text_with_newline, 2)
    T_NUMBER(purple_notify_user_info_get_text_with_newline, 0)     // PurpleNotifyUserInfo*
    T_STRING(purple_notify_user_info_get_text_with_newline, 1)     // char*

    RETURN_STRING((char *)purple_notify_user_info_get_text_with_newline((PurpleNotifyUserInfo *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_add_pair, 3)
    T_NUMBER(purple_notify_user_info_add_pair, 0)     // PurpleNotifyUserInfo*
    T_STRING(purple_notify_user_info_add_pair, 1)     // char*
    T_STRING(purple_notify_user_info_add_pair, 2)     // char*

    purple_notify_user_info_add_pair((PurpleNotifyUserInfo *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_prepend_pair, 3)
    T_NUMBER(purple_notify_user_info_prepend_pair, 0)     // PurpleNotifyUserInfo*
    T_STRING(purple_notify_user_info_prepend_pair, 1)     // char*
    T_STRING(purple_notify_user_info_prepend_pair, 2)     // char*

    purple_notify_user_info_prepend_pair((PurpleNotifyUserInfo *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_new, 2)
    T_STRING(purple_notify_user_info_entry_new, 0)     // char*
    T_STRING(purple_notify_user_info_entry_new, 1)     // char*

    RETURN_NUMBER((long)purple_notify_user_info_entry_new((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_add_section_break, 1)
    T_NUMBER(purple_notify_user_info_add_section_break, 0)     // PurpleNotifyUserInfo*

    purple_notify_user_info_add_section_break((PurpleNotifyUserInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_prepend_section_break, 1)
    T_NUMBER(purple_notify_user_info_prepend_section_break, 0)     // PurpleNotifyUserInfo*

    purple_notify_user_info_prepend_section_break((PurpleNotifyUserInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_add_section_header, 2)
    T_NUMBER(purple_notify_user_info_add_section_header, 0)     // PurpleNotifyUserInfo*
    T_STRING(purple_notify_user_info_add_section_header, 1)     // char*

    purple_notify_user_info_add_section_header((PurpleNotifyUserInfo *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_prepend_section_header, 2)
    T_NUMBER(purple_notify_user_info_prepend_section_header, 0)     // PurpleNotifyUserInfo*
    T_STRING(purple_notify_user_info_prepend_section_header, 1)     // char*

    purple_notify_user_info_prepend_section_header((PurpleNotifyUserInfo *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_remove_last_item, 1)
    T_NUMBER(purple_notify_user_info_remove_last_item, 0)     // PurpleNotifyUserInfo*

    purple_notify_user_info_remove_last_item((PurpleNotifyUserInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_get_label, 1)
    T_NUMBER(purple_notify_user_info_entry_get_label, 0)     // PurpleNotifyUserInfoEntry*

    RETURN_STRING((char *)purple_notify_user_info_entry_get_label((PurpleNotifyUserInfoEntry *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_set_label, 2)
    T_NUMBER(purple_notify_user_info_entry_set_label, 0)     // PurpleNotifyUserInfoEntry*
    T_STRING(purple_notify_user_info_entry_set_label, 1)     // char*

    purple_notify_user_info_entry_set_label((PurpleNotifyUserInfoEntry *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_get_value, 1)
    T_NUMBER(purple_notify_user_info_entry_get_value, 0)     // PurpleNotifyUserInfoEntry*

    RETURN_STRING((char *)purple_notify_user_info_entry_get_value((PurpleNotifyUserInfoEntry *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_set_value, 2)
    T_NUMBER(purple_notify_user_info_entry_set_value, 0)     // PurpleNotifyUserInfoEntry*
    T_STRING(purple_notify_user_info_entry_set_value, 1)     // char*

    purple_notify_user_info_entry_set_value((PurpleNotifyUserInfoEntry *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_get_type, 1)
    T_NUMBER(purple_notify_user_info_entry_get_type, 0)     // PurpleNotifyUserInfoEntry*

    RETURN_NUMBER(purple_notify_user_info_entry_get_type((PurpleNotifyUserInfoEntry *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_user_info_entry_set_type, 2)
    T_NUMBER(purple_notify_user_info_entry_set_type, 0)     // PurpleNotifyUserInfoEntry*
    T_NUMBER(purple_notify_user_info_entry_set_type, 1)     // PurpleNotifyUserInfoEntryType

    purple_notify_user_info_entry_set_type((PurpleNotifyUserInfoEntry *)(long)PARAM(0), (PurpleNotifyUserInfoEntryType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_uri, 2)
    T_NUMBER(purple_notify_uri, 0)     // void*
    T_STRING(purple_notify_uri, 1)     // char*

    RETURN_NUMBER((long)purple_notify_uri((void *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_close, 2)
    T_NUMBER(purple_notify_close, 0)     // PurpleNotifyType
    T_NUMBER(purple_notify_close, 1)     // void*

    purple_notify_close((PurpleNotifyType)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_close_with_handle, 1)
    T_NUMBER(purple_notify_close_with_handle, 0)     // void*

    purple_notify_close_with_handle((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_set_ui_ops, 1)
    T_NUMBER(purple_notify_set_ui_ops, 0)

    purple_notify_set_ui_ops((PurpleNotifyUiOps *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_notify_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_get_handle, 0)

    RETURN_NUMBER((long)purple_notify_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_init, 0)

    purple_notify_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_notify_uninit, 0)

    purple_notify_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ntlm_gen_type1, 2)
    T_STRING(purple_ntlm_gen_type1, 0)     // char*
    T_STRING(purple_ntlm_gen_type1, 1)     // char*

    RETURN_STRING((char *)purple_ntlm_gen_type1((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ntlm_parse_type2, 2)
    T_STRING(purple_ntlm_parse_type2, 0)     // char*

// ... parameter 1 is by reference (guint32*)
    guint32 local_parameter_1;

    RETURN_NUMBER((long)purple_ntlm_parse_type2((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ntlm_gen_type3, 6)
    T_STRING(purple_ntlm_gen_type3, 0)     // char*
    T_STRING(purple_ntlm_gen_type3, 1)     // char*
    T_STRING(purple_ntlm_gen_type3, 2)     // char*
    T_STRING(purple_ntlm_gen_type3, 3)     // char*

// ... parameter 4 is by reference (guint8*)
    guint8 local_parameter_4;
// ... parameter 5 is by reference (guint32*)
    guint32 local_parameter_5;

    RETURN_STRING((char *)purple_ntlm_gen_type3((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), &local_parameter_4, &local_parameter_5))
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_new, 2)
    T_NUMBER(purple_plugin_new, 0)     // int
    T_STRING(purple_plugin_new, 1)     // char*

    RETURN_NUMBER((long)purple_plugin_new((int)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_probe, 1)
    T_STRING(purple_plugin_probe, 0)     // char*

    RETURN_NUMBER((long)purple_plugin_probe((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_register, 1)
    T_NUMBER(purple_plugin_register, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_register((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_load, 1)
    T_NUMBER(purple_plugin_load, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_load((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_unload, 1)
    T_NUMBER(purple_plugin_unload, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_unload((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_disable, 1)
    T_NUMBER(purple_plugin_disable, 0)     // PurplePlugin*

    purple_plugin_disable((PurplePlugin *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_reload, 1)
    T_NUMBER(purple_plugin_reload, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_reload((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_destroy, 1)
    T_NUMBER(purple_plugin_destroy, 0)     // PurplePlugin*

    purple_plugin_destroy((PurplePlugin *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_is_loaded, 1)
    T_NUMBER(purple_plugin_is_loaded, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_is_loaded((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_is_unloadable, 1)
    T_NUMBER(purple_plugin_is_unloadable, 0)     // PurplePlugin*

    RETURN_NUMBER(purple_plugin_is_unloadable((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_id, 1)
    T_NUMBER(purple_plugin_get_id, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_id((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_name, 1)
    T_NUMBER(purple_plugin_get_name, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_name((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_version, 1)
    T_NUMBER(purple_plugin_get_version, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_version((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_summary, 1)
    T_NUMBER(purple_plugin_get_summary, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_summary((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_description, 1)
    T_NUMBER(purple_plugin_get_description, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_description((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_author, 1)
    T_NUMBER(purple_plugin_get_author, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_author((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_get_homepage, 1)
    T_NUMBER(purple_plugin_get_homepage, 0)     // PurplePlugin*

    RETURN_STRING((char *)purple_plugin_get_homepage((PurplePlugin *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_plugin_ipc_register,6)
        T_NUMBER(purple_plugin_get_homepage, 0) // PurplePlugin*
        T_STRING(purple_plugin_get_homepage, 1) // char*
        T_NUMBER(purple_plugin_get_homepage, 2) // PurpleCallback
        T_NUMBER(purple_plugin_get_homepage, 3) // PurpleSignalMarshalFunc
        T_NUMBER(purple_plugin_get_homepage, 4) // PurpleValue*
        T_NUMBER(purple_plugin_get_homepage, 5) // int

        RETURN_NUMBER(purple_plugin_ipc_register((PurplePlugin*)(long)PARAM(0), (char*)PARAM(1), (PurpleCallback)PARAM(2), (PurpleSignalMarshalFunc)PARAM(3), (PurpleValue*)(long)PARAM(4), (int)PARAM(5)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_plugin_ipc_unregister,2)
        T_NUMBER(purple_plugin_ipc_unregister, 0) // PurplePlugin*
        T_STRING(purple_plugin_ipc_unregister, 1) // char*

        purple_plugin_ipc_unregister((PurplePlugin*)(long)PARAM(0), (char*)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_plugin_ipc_unregister_all,1)
        T_NUMBER(purple_plugin_ipc_unregister_all, 0) // PurplePlugin*

        purple_plugin_ipc_unregister_all((PurplePlugin*)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_plugin_ipc_get_params,5)
        T_NUMBER(purple_plugin_ipc_get_params, 0) // PurplePlugin*
        T_STRING(purple_plugin_ipc_get_params, 1) // char*

        // ... parameter 2 is by reference (PurpleValue**)
        PurpleValue* local_parameter_2;
        // ... parameter 3 is by reference (int*)
        int local_parameter_3;
        // ... parameter 4 is by reference (PurpleValue***)
        PurpleValue** local_parameter_4;

        RETURN_NUMBER(purple_plugin_ipc_get_params((PurplePlugin*)(long)PARAM(0), (char*)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_plugin_ipc_call,3)
        T_NUMBER(purple_plugin_ipc_call, 0) // PurplePlugin*
        T_STRING(purple_plugin_ipc_call, 1) // char*

        // ... parameter 2 is by reference (int*)
        int local_parameter_2;

        RETURN_NUMBER((long)purple_plugin_ipc_call((PurplePlugin*)(long)PARAM(0), (char*)PARAM(1), &local_parameter_2))
        SET_NUMBER(2, (long)local_parameter_2)
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_add_search_path, 1)
    T_STRING(purple_plugins_add_search_path, 0)     // char*

    purple_plugins_add_search_path((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_get_search_paths, 0)

    RETURN_NUMBER((long)purple_plugins_get_search_paths())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_unload_all, 0)

    purple_plugins_unload_all();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_unload, 1)
    T_NUMBER(purple_plugins_unload, 0)     // PurplePluginType

    purple_plugins_unload((PurplePluginType)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_destroy_all, 0)

    purple_plugins_destroy_all();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_save_loaded, 1)
    T_STRING(purple_plugins_save_loaded, 0)     // char*

    purple_plugins_save_loaded((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_load_saved, 1)
    T_STRING(purple_plugins_load_saved, 0)     // char*

    purple_plugins_load_saved((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_probe, 1)
    T_STRING(purple_plugins_probe, 0)     // char*

    purple_plugins_probe((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_enabled, 0)

    RETURN_NUMBER(purple_plugins_enabled())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_find_with_name, 1)
    T_STRING(purple_plugins_find_with_name, 0)     // char*

    RETURN_NUMBER((long)purple_plugins_find_with_name((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_find_with_filename, 1)
    T_STRING(purple_plugins_find_with_filename, 0)     // char*

    RETURN_NUMBER((long)purple_plugins_find_with_filename((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_find_with_basename, 1)
    T_STRING(purple_plugins_find_with_basename, 0)     // char*

    RETURN_NUMBER((long)purple_plugins_find_with_basename((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_find_with_id, 1)
    T_STRING(purple_plugins_find_with_id, 0)     // char*

    RETURN_NUMBER((long)purple_plugins_find_with_id((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_get_loaded, 0)
    ListToArr(purple_plugins_get_loaded(), RESULT);
//RETURN_NUMBER((long))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_get_protocols, 0)
    GList * lst = purple_plugins_get_protocols();
    ListToArr(lst, RESULT);

//RETURN_NUMBER((long)purple_plugins_get_protocols())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_get_all, 0)
    ListToArr(purple_plugins_get_all(), RESULT);

//RETURN_NUMBER((long)purple_plugins_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_get_handle, 0)

    RETURN_NUMBER((long)purple_plugins_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_init, 0)

    purple_plugins_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugins_uninit, 0)

    purple_plugins_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_action_free, 1)
    T_NUMBER(purple_plugin_action_free, 0)     // PurplePluginAction*

    purple_plugin_action_free((PurplePluginAction *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_frame_new, 0)

    RETURN_NUMBER((long)purple_plugin_pref_frame_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_frame_destroy, 1)
    T_NUMBER(purple_plugin_pref_frame_destroy, 0)     // PurplePluginPrefFrame*

    purple_plugin_pref_frame_destroy((PurplePluginPrefFrame *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_frame_add, 2)
    T_NUMBER(purple_plugin_pref_frame_add, 0)     // PurplePluginPrefFrame*
    T_NUMBER(purple_plugin_pref_frame_add, 1)     // PurplePluginPref*

    purple_plugin_pref_frame_add((PurplePluginPrefFrame *)(long)PARAM(0), (PurplePluginPref *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_frame_get_prefs, 1)
    T_NUMBER(purple_plugin_pref_frame_get_prefs, 0)     // PurplePluginPrefFrame*

    RETURN_NUMBER((long)purple_plugin_pref_frame_get_prefs((PurplePluginPrefFrame *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_new, 0)

    RETURN_NUMBER((long)purple_plugin_pref_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_new_with_name, 1)
    T_STRING(purple_plugin_pref_new_with_name, 0)     // char*

    RETURN_NUMBER((long)purple_plugin_pref_new_with_name((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_new_with_label, 1)
    T_STRING(purple_plugin_pref_new_with_label, 0)     // char*

    RETURN_NUMBER((long)purple_plugin_pref_new_with_label((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_new_with_name_and_label, 2)
    T_STRING(purple_plugin_pref_new_with_name_and_label, 0)     // char*
    T_STRING(purple_plugin_pref_new_with_name_and_label, 1)     // char*

    RETURN_NUMBER((long)purple_plugin_pref_new_with_name_and_label((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_destroy, 1)
    T_NUMBER(purple_plugin_pref_destroy, 0)     // PurplePluginPref*

    purple_plugin_pref_destroy((PurplePluginPref *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_name, 2)
    T_NUMBER(purple_plugin_pref_set_name, 0)     // PurplePluginPref*
    T_STRING(purple_plugin_pref_set_name, 1)     // char*

    purple_plugin_pref_set_name((PurplePluginPref *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_name, 1)
    T_NUMBER(purple_plugin_pref_get_name, 0)     // PurplePluginPref*

    RETURN_STRING((char *)purple_plugin_pref_get_name((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_label, 2)
    T_NUMBER(purple_plugin_pref_set_label, 0)     // PurplePluginPref*
    T_STRING(purple_plugin_pref_set_label, 1)     // char*

    purple_plugin_pref_set_label((PurplePluginPref *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_label, 1)
    T_NUMBER(purple_plugin_pref_get_label, 0)     // PurplePluginPref*

    RETURN_STRING((char *)purple_plugin_pref_get_label((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_bounds, 3)
    T_NUMBER(purple_plugin_pref_set_bounds, 0)     // PurplePluginPref*
    T_NUMBER(purple_plugin_pref_set_bounds, 1)     // int
    T_NUMBER(purple_plugin_pref_set_bounds, 2)     // int

    purple_plugin_pref_set_bounds((PurplePluginPref *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_bounds, 3)
    T_NUMBER(purple_plugin_pref_get_bounds, 0)     // PurplePluginPref*

// ... parameter 1 is by reference (int*)
    int local_parameter_1;
// ... parameter 2 is by reference (int*)
    int local_parameter_2;

    purple_plugin_pref_get_bounds((PurplePluginPref *)(long)PARAM(0), &local_parameter_1, &local_parameter_2);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_type, 2)
    T_NUMBER(purple_plugin_pref_set_type, 0)     // PurplePluginPref*
    T_NUMBER(purple_plugin_pref_set_type, 1)     // PurplePluginPrefType

    purple_plugin_pref_set_type((PurplePluginPref *)(long)PARAM(0), (PurplePluginPrefType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_type, 1)
    T_NUMBER(purple_plugin_pref_get_type, 0)     // PurplePluginPref*

    RETURN_NUMBER(purple_plugin_pref_get_type((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_add_choice, 3)
    T_NUMBER(purple_plugin_pref_add_choice, 0)     // PurplePluginPref*
    T_STRING(purple_plugin_pref_add_choice, 1)     // char*
    T_NUMBER(purple_plugin_pref_add_choice, 2)     // void*

    purple_plugin_pref_add_choice((PurplePluginPref *)(long)PARAM(0), (char *)PARAM(1), (void *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_choices, 1)
    T_NUMBER(purple_plugin_pref_get_choices, 0)     // PurplePluginPref*

    RETURN_NUMBER((long)purple_plugin_pref_get_choices((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_max_length, 2)
    T_NUMBER(purple_plugin_pref_set_max_length, 0)     // PurplePluginPref*
    T_NUMBER(purple_plugin_pref_set_max_length, 1)     // unsigned int

    purple_plugin_pref_set_max_length((PurplePluginPref *)(long)PARAM(0), (unsigned int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_max_length, 1)
    T_NUMBER(purple_plugin_pref_get_max_length, 0)     // PurplePluginPref*

    RETURN_NUMBER(purple_plugin_pref_get_max_length((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_masked, 2)
    T_NUMBER(purple_plugin_pref_set_masked, 0)     // PurplePluginPref*
    T_NUMBER(purple_plugin_pref_set_masked, 1)     // int

    purple_plugin_pref_set_masked((PurplePluginPref *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_masked, 1)
    T_NUMBER(purple_plugin_pref_get_masked, 0)     // PurplePluginPref*

    RETURN_NUMBER(purple_plugin_pref_get_masked((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_set_format_type, 2)
    T_NUMBER(purple_plugin_pref_set_format_type, 0)     // PurplePluginPref*
    T_NUMBER(purple_plugin_pref_set_format_type, 1)     // PurpleStringFormatType

    purple_plugin_pref_set_format_type((PurplePluginPref *)(long)PARAM(0), (PurpleStringFormatType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_plugin_pref_get_format_type, 1)
    T_NUMBER(purple_plugin_pref_get_format_type, 0)     // PurplePluginPref*

    RETURN_NUMBER(purple_plugin_pref_get_format_type((PurplePluginPref *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_new, 5)
    T_STRING(purple_pounce_new, 0)     // char*
    T_NUMBER(purple_pounce_new, 1)     // PurpleAccount*
    T_STRING(purple_pounce_new, 2)     // char*
    T_NUMBER(purple_pounce_new, 3)     // PurplePounceEvent
    T_NUMBER(purple_pounce_new, 4)     // PurplePounceOption

    RETURN_NUMBER((long)purple_pounce_new((char *)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2), (PurplePounceEvent)PARAM(3), (PurplePounceOption)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_destroy, 1)
    T_NUMBER(purple_pounce_destroy, 0)     // PurplePounce*

    purple_pounce_destroy((PurplePounce *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_destroy_all_by_account, 1)
    T_NUMBER(purple_pounce_destroy_all_by_account, 0)     // PurpleAccount*

    purple_pounce_destroy_all_by_account((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_events, 2)
    T_NUMBER(purple_pounce_set_events, 0)     // PurplePounce*
    T_NUMBER(purple_pounce_set_events, 1)     // PurplePounceEvent

    purple_pounce_set_events((PurplePounce *)(long)PARAM(0), (PurplePounceEvent)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_options, 2)
    T_NUMBER(purple_pounce_set_options, 0)     // PurplePounce*
    T_NUMBER(purple_pounce_set_options, 1)     // PurplePounceOption

    purple_pounce_set_options((PurplePounce *)(long)PARAM(0), (PurplePounceOption)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_pouncer, 2)
    T_NUMBER(purple_pounce_set_pouncer, 0)     // PurplePounce*
    T_NUMBER(purple_pounce_set_pouncer, 1)     // PurpleAccount*

    purple_pounce_set_pouncer((PurplePounce *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_pouncee, 2)
    T_NUMBER(purple_pounce_set_pouncee, 0)     // PurplePounce*
    T_STRING(purple_pounce_set_pouncee, 1)     // char*

    purple_pounce_set_pouncee((PurplePounce *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_save, 2)
    T_NUMBER(purple_pounce_set_save, 0)     // PurplePounce*
    T_NUMBER(purple_pounce_set_save, 1)     // int

    purple_pounce_set_save((PurplePounce *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_action_register, 2)
    T_NUMBER(purple_pounce_action_register, 0)     // PurplePounce*
    T_STRING(purple_pounce_action_register, 1)     // char*

    purple_pounce_action_register((PurplePounce *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_action_set_enabled, 3)
    T_NUMBER(purple_pounce_action_set_enabled, 0)     // PurplePounce*
    T_STRING(purple_pounce_action_set_enabled, 1)     // char*
    T_NUMBER(purple_pounce_action_set_enabled, 2)     // int

    purple_pounce_action_set_enabled((PurplePounce *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_action_set_attribute, 4)
    T_NUMBER(purple_pounce_action_set_attribute, 0)     // PurplePounce*
    T_STRING(purple_pounce_action_set_attribute, 1)     // char*
    T_STRING(purple_pounce_action_set_attribute, 2)     // char*
    T_STRING(purple_pounce_action_set_attribute, 3)     // char*

    purple_pounce_action_set_attribute((PurplePounce *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_set_data, 2)
    T_NUMBER(purple_pounce_set_data, 0)     // PurplePounce*
    T_NUMBER(purple_pounce_set_data, 1)     // void*

    purple_pounce_set_data((PurplePounce *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_events, 1)
    T_NUMBER(purple_pounce_get_events, 0)     // PurplePounce*

    RETURN_NUMBER(purple_pounce_get_events((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_options, 1)
    T_NUMBER(purple_pounce_get_options, 0)     // PurplePounce*

    RETURN_NUMBER(purple_pounce_get_options((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_pouncer, 1)
    T_NUMBER(purple_pounce_get_pouncer, 0)     // PurplePounce*

    RETURN_NUMBER((long)purple_pounce_get_pouncer((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_pouncee, 1)
    T_NUMBER(purple_pounce_get_pouncee, 0)     // PurplePounce*

    RETURN_STRING((char *)purple_pounce_get_pouncee((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_save, 1)
    T_NUMBER(purple_pounce_get_save, 0)     // PurplePounce*

    RETURN_NUMBER(purple_pounce_get_save((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_action_is_enabled, 2)
    T_NUMBER(purple_pounce_action_is_enabled, 0)     // PurplePounce*
    T_STRING(purple_pounce_action_is_enabled, 1)     // char*

    RETURN_NUMBER(purple_pounce_action_is_enabled((PurplePounce *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_action_get_attribute, 3)
    T_NUMBER(purple_pounce_action_get_attribute, 0)     // PurplePounce*
    T_STRING(purple_pounce_action_get_attribute, 1)     // char*
    T_STRING(purple_pounce_action_get_attribute, 2)     // char*

    RETURN_STRING((char *)purple_pounce_action_get_attribute((PurplePounce *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_get_data, 1)
    T_NUMBER(purple_pounce_get_data, 0)     // PurplePounce*

    RETURN_NUMBER((long)purple_pounce_get_data((PurplePounce *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounce_execute, 3)
    T_NUMBER(purple_pounce_execute, 0)     // PurpleAccount*
    T_STRING(purple_pounce_execute, 1)     // char*
    T_NUMBER(purple_pounce_execute, 2)     // PurplePounceEvent

    purple_pounce_execute((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (PurplePounceEvent)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_pounce, 3)
    T_NUMBER(purple_find_pounce, 0)     // PurpleAccount*
    T_STRING(purple_find_pounce, 1)     // char*
    T_NUMBER(purple_find_pounce, 2)     // PurplePounceEvent

    RETURN_NUMBER((long)purple_find_pounce((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (PurplePounceEvent)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_load, 0)

    RETURN_NUMBER(purple_pounces_load())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_unregister_handler, 1)
    T_STRING(purple_pounces_unregister_handler, 0)     // char*

    purple_pounces_unregister_handler((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_get_all, 0)

    RETURN_NUMBER((long)purple_pounces_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_get_all_for_ui, 1)
    T_STRING(purple_pounces_get_all_for_ui, 0)     // char*

    RETURN_NUMBER((long)purple_pounces_get_all_for_ui((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_get_handle, 0)

    RETURN_NUMBER((long)purple_pounces_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_init, 0)

    purple_pounces_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_pounces_uninit, 0)

    purple_pounces_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_handle, 0)

    RETURN_NUMBER((long)purple_prefs_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_init, 0)

    purple_prefs_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_uninit, 0)

    purple_prefs_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_none, 1)
    T_STRING(purple_prefs_add_none, 0)     // char*

    purple_prefs_add_none((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_bool, 2)
    T_STRING(purple_prefs_add_bool, 0)     // char*
    T_NUMBER(purple_prefs_add_bool, 1)     // int

    purple_prefs_add_bool((char *)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_int, 2)
    T_STRING(purple_prefs_add_int, 0)     // char*
    T_NUMBER(purple_prefs_add_int, 1)     // int

    purple_prefs_add_int((char *)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_string, 2)
    T_STRING(purple_prefs_add_string, 0)     // char*
    T_STRING(purple_prefs_add_string, 1)     // char*

    purple_prefs_add_string((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_string_list, 2)
    T_STRING(purple_prefs_add_string_list, 0)     // char*
    T_NUMBER(purple_prefs_add_string_list, 1)     // GList*

    purple_prefs_add_string_list((char *)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_path, 2)
    T_STRING(purple_prefs_add_path, 0)     // char*
    T_STRING(purple_prefs_add_path, 1)     // char*

    purple_prefs_add_path((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_add_path_list, 2)
    T_STRING(purple_prefs_add_path_list, 0)     // char*
    T_NUMBER(purple_prefs_add_path_list, 1)     // GList*

    purple_prefs_add_path_list((char *)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_remove, 1)
    T_STRING(purple_prefs_remove, 0)     // char*

    purple_prefs_remove((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_rename, 2)
    T_STRING(purple_prefs_rename, 0)     // char*
    T_STRING(purple_prefs_rename, 1)     // char*

    purple_prefs_rename((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_rename_boolean_toggle, 2)
    T_STRING(purple_prefs_rename_boolean_toggle, 0)     // char*
    T_STRING(purple_prefs_rename_boolean_toggle, 1)     // char*

    purple_prefs_rename_boolean_toggle((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_destroy, 0)

    purple_prefs_destroy();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_generic, 2)
    T_STRING(purple_prefs_set_generic, 0)     // char*
    T_NUMBER(purple_prefs_set_generic, 1)     // void*

    purple_prefs_set_generic((char *)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_bool, 2)
    T_STRING(purple_prefs_set_bool, 0)     // char*
    T_NUMBER(purple_prefs_set_bool, 1)     // int

    purple_prefs_set_bool((char *)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_int, 2)
    T_STRING(purple_prefs_set_int, 0)     // char*
    T_NUMBER(purple_prefs_set_int, 1)     // int

    purple_prefs_set_int((char *)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_string, 2)
    T_STRING(purple_prefs_set_string, 0)     // char*
    T_STRING(purple_prefs_set_string, 1)     // char*

    purple_prefs_set_string((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_string_list, 2)
    T_STRING(purple_prefs_set_string_list, 0)     // char*
    T_NUMBER(purple_prefs_set_string_list, 1)     // GList*

    purple_prefs_set_string_list((char *)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_path, 2)
    T_STRING(purple_prefs_set_path, 0)     // char*
    T_STRING(purple_prefs_set_path, 1)     // char*

    purple_prefs_set_path((char *)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_set_path_list, 2)
    T_STRING(purple_prefs_set_path_list, 0)     // char*
    T_NUMBER(purple_prefs_set_path_list, 1)     // GList*

    purple_prefs_set_path_list((char *)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_exists, 1)
    T_STRING(purple_prefs_exists, 0)     // char*

    RETURN_NUMBER(purple_prefs_exists((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_type, 1)
    T_STRING(purple_prefs_get_type, 0)     // char*

    RETURN_NUMBER(purple_prefs_get_type((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_bool, 1)
    T_STRING(purple_prefs_get_bool, 0)     // char*

    RETURN_NUMBER(purple_prefs_get_bool((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_int, 1)
    T_STRING(purple_prefs_get_int, 0)     // char*

    RETURN_NUMBER(purple_prefs_get_int((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_string, 1)
    T_STRING(purple_prefs_get_string, 0)     // char*

    RETURN_STRING((char *)purple_prefs_get_string((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_string_list, 1)
    T_STRING(purple_prefs_get_string_list, 0)     // char*

    RETURN_NUMBER((long)purple_prefs_get_string_list((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_path, 1)
    T_STRING(purple_prefs_get_path, 0)     // char*

    RETURN_STRING((char *)purple_prefs_get_path((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_path_list, 1)
    T_STRING(purple_prefs_get_path_list, 0)     // char*

    RETURN_NUMBER((long)purple_prefs_get_path_list((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_get_children_names, 1)
    T_STRING(purple_prefs_get_children_names, 0)     // char*

    RETURN_NUMBER((long)purple_prefs_get_children_names((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_connect_callback, 3)
    T_NUMBER(purple_prefs_connect_callback, 0)     // void*
    T_STRING(purple_prefs_connect_callback, 1)     // char*
    T_DELEGATE(purple_prefs_connect_callback, 2)   // PurplePrefCallback
//T_NUMBER(3) // void*

    RETURN_NUMBER(purple_prefs_connect_callback((void *)(long)PARAM(0), (char *)PARAM(1), PurplePrefCallback_F, (void *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_disconnect_callback, 1)
    T_NUMBER(purple_prefs_disconnect_callback, 0)     // guint

    purple_prefs_disconnect_callback((guint)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_disconnect_by_handle, 1)
    T_NUMBER(purple_prefs_disconnect_by_handle, 0)     // void*

    purple_prefs_disconnect_by_handle((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_trigger_callback, 1)
    T_STRING(purple_prefs_trigger_callback, 0)     // char*

    purple_prefs_trigger_callback((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_load, 0)

    RETURN_NUMBER(purple_prefs_load())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prefs_update_old, 0)

    purple_prefs_update_old();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_permit_add, 3)
    T_NUMBER(purple_privacy_permit_add, 0)     // PurpleAccount*
    T_STRING(purple_privacy_permit_add, 1)     // char*
    T_NUMBER(purple_privacy_permit_add, 2)     // int

    RETURN_NUMBER(purple_privacy_permit_add((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_permit_remove, 3)
    T_NUMBER(purple_privacy_permit_remove, 0)     // PurpleAccount*
    T_STRING(purple_privacy_permit_remove, 1)     // char*
    T_NUMBER(purple_privacy_permit_remove, 2)     // int

    RETURN_NUMBER(purple_privacy_permit_remove((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_deny_add, 3)
    T_NUMBER(purple_privacy_deny_add, 0)     // PurpleAccount*
    T_STRING(purple_privacy_deny_add, 1)     // char*
    T_NUMBER(purple_privacy_deny_add, 2)     // int

    RETURN_NUMBER(purple_privacy_deny_add((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_deny_remove, 3)
    T_NUMBER(purple_privacy_deny_remove, 0)     // PurpleAccount*
    T_STRING(purple_privacy_deny_remove, 1)     // char*
    T_NUMBER(purple_privacy_deny_remove, 2)     // int

    RETURN_NUMBER(purple_privacy_deny_remove((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_allow, 4)
    T_NUMBER(purple_privacy_allow, 0)     // PurpleAccount*
    T_STRING(purple_privacy_allow, 1)     // char*
    T_NUMBER(purple_privacy_allow, 2)     // int
    T_NUMBER(purple_privacy_allow, 3)     // int

    purple_privacy_allow((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_deny, 4)
    T_NUMBER(purple_privacy_deny, 0)     // PurpleAccount*
    T_STRING(purple_privacy_deny, 1)     // char*
    T_NUMBER(purple_privacy_deny, 2)     // int
    T_NUMBER(purple_privacy_deny, 3)     // int

    purple_privacy_deny((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_check, 2)
    T_NUMBER(purple_privacy_check, 0)     // PurpleAccount*
    T_STRING(purple_privacy_check, 1)     // char*

    RETURN_NUMBER(purple_privacy_check((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_set_ui_ops, 1)
    T_NUMBER(purple_privacy_set_ui_ops, 0)

    purple_privacy_set_ui_ops((PurplePrivacyUiOps *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_privacy_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_privacy_init, 0)

    purple_privacy_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_new, 0)

    RETURN_NUMBER((long)purple_proxy_info_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_destroy, 1)
    T_NUMBER(purple_proxy_info_destroy, 0)     // PurpleProxyInfo*

    purple_proxy_info_destroy((PurpleProxyInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_set_type, 2)
    T_NUMBER(purple_proxy_info_set_type, 0)     // PurpleProxyInfo*
    T_NUMBER(purple_proxy_info_set_type, 1)     // PurpleProxyType

    purple_proxy_info_set_type((PurpleProxyInfo *)(long)PARAM(0), (PurpleProxyType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_set_host, 2)
    T_NUMBER(purple_proxy_info_set_host, 0)     // PurpleProxyInfo*
    T_STRING(purple_proxy_info_set_host, 1)     // char*

    purple_proxy_info_set_host((PurpleProxyInfo *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_set_port, 2)
    T_NUMBER(purple_proxy_info_set_port, 0)     // PurpleProxyInfo*
    T_NUMBER(purple_proxy_info_set_port, 1)     // int

    purple_proxy_info_set_port((PurpleProxyInfo *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_set_username, 2)
    T_NUMBER(purple_proxy_info_set_username, 0)     // PurpleProxyInfo*
    T_STRING(purple_proxy_info_set_username, 1)     // char*

    purple_proxy_info_set_username((PurpleProxyInfo *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_set_password, 2)
    T_NUMBER(purple_proxy_info_set_password, 0)     // PurpleProxyInfo*
    T_STRING(purple_proxy_info_set_password, 1)     // char*

    purple_proxy_info_set_password((PurpleProxyInfo *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_get_type, 1)
    T_NUMBER(purple_proxy_info_get_type, 0)     // PurpleProxyInfo*

    RETURN_NUMBER(purple_proxy_info_get_type((PurpleProxyInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_get_host, 1)
    T_NUMBER(purple_proxy_info_get_host, 0)     // PurpleProxyInfo*

    RETURN_STRING((char *)purple_proxy_info_get_host((PurpleProxyInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_get_port, 1)
    T_NUMBER(purple_proxy_info_get_port, 0)     // PurpleProxyInfo*

    RETURN_NUMBER(purple_proxy_info_get_port((PurpleProxyInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_get_username, 1)
    T_NUMBER(purple_proxy_info_get_username, 0)     // PurpleProxyInfo*

    RETURN_STRING((char *)purple_proxy_info_get_username((PurpleProxyInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_info_get_password, 1)
    T_NUMBER(purple_proxy_info_get_password, 0)     // PurpleProxyInfo*

    RETURN_STRING((char *)purple_proxy_info_get_password((PurpleProxyInfo *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_global_proxy_get_info, 0)

    RETURN_NUMBER((long)purple_global_proxy_get_info())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_global_proxy_set_info, 1)
    T_NUMBER(purple_global_proxy_set_info, 0)     // PurpleProxyInfo*

    purple_global_proxy_set_info((PurpleProxyInfo *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_get_handle, 0)

    RETURN_NUMBER((long)purple_proxy_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_init, 0)

    purple_proxy_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_uninit, 0)

    purple_proxy_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_get_setup, 1)
    T_NUMBER(purple_proxy_get_setup, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_proxy_get_setup((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_connect, 5)
    T_NUMBER(purple_proxy_connect, 0)     // void*
    T_NUMBER(purple_proxy_connect, 1)     // PurpleAccount*
    T_STRING(purple_proxy_connect, 2)     // char*
    T_NUMBER(purple_proxy_connect, 3)     // int
    T_DELEGATE(purple_proxy_connect, 4)   // PurpleProxyConnectFunction
//T_NUMBER(5) // void*

    RETURN_NUMBER((long)purple_proxy_connect((void *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2), (int)PARAM(3), PurpleProxyConnectFunction_F, (void *)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_connect_udp, 4)
    T_NUMBER(purple_proxy_connect_udp, 0)     // void*
    T_NUMBER(purple_proxy_connect_udp, 1)     // PurpleAccount*
    T_STRING(purple_proxy_connect_udp, 2)     // char*
    T_NUMBER(purple_proxy_connect_udp, 3)     // int
    T_DELEGATE(purple_proxy_connect_udp, 4)   // PurpleProxyConnectFunction
//T_NUMBER(5) // void*

    RETURN_NUMBER((long)purple_proxy_connect_udp((void *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1), (char *)PARAM(2), (int)PARAM(3), PurpleProxyConnectFunction_F, (void *)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_connect_socks5, 4)
    T_NUMBER(purple_proxy_connect_socks5, 0)     // void*
    T_NUMBER(purple_proxy_connect_socks5, 1)     // PurpleProxyInfo*
    T_STRING(purple_proxy_connect_socks5, 2)     // char*
    T_NUMBER(purple_proxy_connect_socks5, 3)     // int
    T_DELEGATE(purple_proxy_connect_socks5, 4)   // PurpleProxyConnectFunction
//T_NUMBER(5) // void*

    RETURN_NUMBER((long)purple_proxy_connect_socks5((void *)(long)PARAM(0), (PurpleProxyInfo *)(long)PARAM(1), (char *)PARAM(2), (int)PARAM(3), PurpleProxyConnectFunction_F, (void *)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_connect_cancel, 1)
    T_NUMBER(purple_proxy_connect_cancel, 0)     // PurpleProxyConnectData*

    purple_proxy_connect_cancel((PurpleProxyConnectData *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_proxy_connect_cancel_with_handle, 1)
    T_NUMBER(purple_proxy_connect_cancel_with_handle, 0)     // void*

    purple_proxy_connect_cancel_with_handle((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_new, 4)
    T_STRING(purple_attention_type_new, 0)     // char*
    T_STRING(purple_attention_type_new, 1)     // char*
    T_STRING(purple_attention_type_new, 2)     // char*
    T_STRING(purple_attention_type_new, 3)     // char*

    RETURN_NUMBER((long)purple_attention_type_new((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_set_name, 2)
    T_NUMBER(purple_attention_type_set_name, 0)     // PurpleAttentionType*
    T_STRING(purple_attention_type_set_name, 1)     // char*

    purple_attention_type_set_name((PurpleAttentionType *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_set_incoming_desc, 2)
    T_NUMBER(purple_attention_type_set_incoming_desc, 0)     // PurpleAttentionType*
    T_STRING(purple_attention_type_set_incoming_desc, 1)     // char*

    purple_attention_type_set_incoming_desc((PurpleAttentionType *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_set_outgoing_desc, 2)
    T_NUMBER(purple_attention_type_set_outgoing_desc, 0)     // PurpleAttentionType*
    T_STRING(purple_attention_type_set_outgoing_desc, 1)     // char*

    purple_attention_type_set_outgoing_desc((PurpleAttentionType *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_set_icon_name, 2)
    T_NUMBER(purple_attention_type_set_icon_name, 0)     // PurpleAttentionType*
    T_STRING(purple_attention_type_set_icon_name, 1)     // char*

    purple_attention_type_set_icon_name((PurpleAttentionType *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_set_unlocalized_name, 2)
    T_NUMBER(purple_attention_type_set_unlocalized_name, 0)     // PurpleAttentionType*
    T_STRING(purple_attention_type_set_unlocalized_name, 1)     // char*

    purple_attention_type_set_unlocalized_name((PurpleAttentionType *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_get_name, 1)
    T_NUMBER(purple_attention_type_get_name, 0)     // PurpleAttentionType*

    RETURN_STRING((char *)purple_attention_type_get_name((PurpleAttentionType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_get_incoming_desc, 1)
    T_NUMBER(purple_attention_type_get_incoming_desc, 0)     // PurpleAttentionType*

    RETURN_STRING((char *)purple_attention_type_get_incoming_desc((PurpleAttentionType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_get_outgoing_desc, 1)
    T_NUMBER(purple_attention_type_get_outgoing_desc, 0)     // PurpleAttentionType*

    RETURN_STRING((char *)purple_attention_type_get_outgoing_desc((PurpleAttentionType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_get_icon_name, 1)
    T_NUMBER(purple_attention_type_get_icon_name, 0)     // PurpleAttentionType*

    RETURN_STRING((char *)purple_attention_type_get_icon_name((PurpleAttentionType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_attention_type_get_unlocalized_name, 1)
    T_NUMBER(purple_attention_type_get_unlocalized_name, 0)     // PurpleAttentionType*

    RETURN_STRING((char *)purple_attention_type_get_unlocalized_name((PurpleAttentionType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_account_idle, 3)
    T_NUMBER(purple_prpl_got_account_idle, 0)     // PurpleAccount*
    T_NUMBER(purple_prpl_got_account_idle, 1)     // int
    T_NUMBER(purple_prpl_got_account_idle, 2)     // time_t

    purple_prpl_got_account_idle((PurpleAccount *)(long)PARAM(0), (int)PARAM(1), (time_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_account_login_time, 2)
    T_NUMBER(purple_prpl_got_account_login_time, 0)     // PurpleAccount*
    T_NUMBER(purple_prpl_got_account_login_time, 1)     // time_t

    purple_prpl_got_account_login_time((PurpleAccount *)(long)PARAM(0), (time_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_account_status, 2)
    T_NUMBER(purple_prpl_got_account_status, 0)     // PurpleAccount*
    T_STRING(purple_prpl_got_account_status, 1)     // char*

    purple_prpl_got_account_status((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_account_actions, 1)
    T_NUMBER(purple_prpl_got_account_actions, 0)     // PurpleAccount*

    purple_prpl_got_account_actions((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_user_idle, 4)
    T_NUMBER(purple_prpl_got_user_idle, 0)     // PurpleAccount*
    T_STRING(purple_prpl_got_user_idle, 1)     // char*
    T_NUMBER(purple_prpl_got_user_idle, 2)     // int
    T_NUMBER(purple_prpl_got_user_idle, 3)     // time_t

    purple_prpl_got_user_idle((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2), (time_t)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_user_login_time, 3)
    T_NUMBER(purple_prpl_got_user_login_time, 0)     // PurpleAccount*
    T_STRING(purple_prpl_got_user_login_time, 1)     // char*
    T_NUMBER(purple_prpl_got_user_login_time, 2)     // time_t

    purple_prpl_got_user_login_time((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (time_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_user_status, 3)
    T_NUMBER(purple_prpl_got_user_status, 0)     // PurpleAccount*
    T_STRING(purple_prpl_got_user_status, 1)     // char*
    T_STRING(purple_prpl_got_user_status, 2)     // char*

    purple_prpl_got_user_status((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_user_status_deactive, 3)
    T_NUMBER(purple_prpl_got_user_status_deactive, 0)     // PurpleAccount*
    T_STRING(purple_prpl_got_user_status_deactive, 1)     // char*
    T_STRING(purple_prpl_got_user_status_deactive, 2)     // char*

    purple_prpl_got_user_status_deactive((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_change_account_status, 3)
    T_NUMBER(purple_prpl_change_account_status, 0)     // PurpleAccount*
    T_NUMBER(purple_prpl_change_account_status, 1)     // PurpleStatus*
    T_NUMBER(purple_prpl_change_account_status, 2)     // PurpleStatus*

    purple_prpl_change_account_status((PurpleAccount *)(long)PARAM(0), (PurpleStatus *)(long)PARAM(1), (PurpleStatus *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_get_statuses, 2)
    T_NUMBER(purple_prpl_get_statuses, 0)     // PurpleAccount*
    T_NUMBER(purple_prpl_get_statuses, 1)     // PurplePresence*

    RETURN_NUMBER((long)purple_prpl_get_statuses((PurpleAccount *)(long)PARAM(0), (PurplePresence *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_send_attention, 3)
    T_NUMBER(purple_prpl_send_attention, 0)     // PurpleConnection*
    T_STRING(purple_prpl_send_attention, 1)     // char*
    T_NUMBER(purple_prpl_send_attention, 2)     // guint

    purple_prpl_send_attention((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (guint)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_attention, 3)
    T_NUMBER(purple_prpl_got_attention, 0)     // PurpleConnection*
    T_STRING(purple_prpl_got_attention, 1)     // char*
    T_NUMBER(purple_prpl_got_attention, 2)     // guint

    purple_prpl_got_attention((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (guint)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_got_attention_in_chat, 4)
    T_NUMBER(purple_prpl_got_attention_in_chat, 0)     // PurpleConnection*
    T_NUMBER(purple_prpl_got_attention_in_chat, 1)     // int
    T_STRING(purple_prpl_got_attention_in_chat, 2)     // char*
    T_NUMBER(purple_prpl_got_attention_in_chat, 3)     // guint

    purple_prpl_got_attention_in_chat((PurpleConnection *)(long)PARAM(0), (int)PARAM(1), (char *)PARAM(2), (guint)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_get_media_caps, 2)
    T_NUMBER(purple_prpl_get_media_caps, 0)     // PurpleAccount*
    T_STRING(purple_prpl_get_media_caps, 1)     // char*

    RETURN_NUMBER(purple_prpl_get_media_caps((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_prpl_initiate_media, 3)
    T_NUMBER(purple_prpl_initiate_media, 0)     // PurpleAccount*
    T_STRING(purple_prpl_initiate_media, 1)     // char*
    T_NUMBER(purple_prpl_initiate_media, 2)     // PurpleMediaSessionType

    RETURN_NUMBER(purple_prpl_initiate_media((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1), (PurpleMediaSessionType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_find_prpl, 1)
    T_STRING(purple_find_prpl, 0)     // char*

    RETURN_NUMBER((long)purple_find_prpl((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_new, 0)

    RETURN_NUMBER((long)purple_request_fields_new())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_destroy, 1)
    T_NUMBER(purple_request_fields_destroy, 0)     // PurpleRequestFields*

    purple_request_fields_destroy((PurpleRequestFields *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_add_group, 2)
    T_NUMBER(purple_request_fields_add_group, 0)     // PurpleRequestFields*
    T_NUMBER(purple_request_fields_add_group, 1)     // PurpleRequestFieldGroup*

    purple_request_fields_add_group((PurpleRequestFields *)(long)PARAM(0), (PurpleRequestFieldGroup *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_groups, 1)
    T_NUMBER(purple_request_fields_get_groups, 0)     // PurpleRequestFields*

    RETURN_NUMBER((long)purple_request_fields_get_groups((PurpleRequestFields *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_exists, 2)
    T_NUMBER(purple_request_fields_exists, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_exists, 1)     // char*

    RETURN_NUMBER(purple_request_fields_exists((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_required, 1)
    T_NUMBER(purple_request_fields_get_required, 0)     // PurpleRequestFields*

    RETURN_NUMBER((long)purple_request_fields_get_required((PurpleRequestFields *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_is_field_required, 2)
    T_NUMBER(purple_request_fields_is_field_required, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_is_field_required, 1)     // char*

    RETURN_NUMBER(purple_request_fields_is_field_required((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_all_required_filled, 1)
    T_NUMBER(purple_request_fields_all_required_filled, 0)     // PurpleRequestFields*

    RETURN_NUMBER(purple_request_fields_all_required_filled((PurpleRequestFields *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_field, 2)
    T_NUMBER(purple_request_fields_get_field, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_field, 1)     // char*

    RETURN_NUMBER((long)purple_request_fields_get_field((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_string, 2)
    T_NUMBER(purple_request_fields_get_string, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_string, 1)     // char*

    RETURN_STRING((char *)purple_request_fields_get_string((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_integer, 2)
    T_NUMBER(purple_request_fields_get_integer, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_integer, 1)     // char*

    RETURN_NUMBER(purple_request_fields_get_integer((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_bool, 2)
    T_NUMBER(purple_request_fields_get_bool, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_bool, 1)     // char*

    RETURN_NUMBER(purple_request_fields_get_bool((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_choice, 2)
    T_NUMBER(purple_request_fields_get_choice, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_choice, 1)     // char*

    RETURN_NUMBER(purple_request_fields_get_choice((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_fields_get_account, 2)
    T_NUMBER(purple_request_fields_get_account, 0)     // PurpleRequestFields*
    T_STRING(purple_request_fields_get_account, 1)     // char*

    RETURN_NUMBER((long)purple_request_fields_get_account((PurpleRequestFields *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_group_new, 1)
    T_STRING(purple_request_field_group_new, 0)     // char*

    RETURN_NUMBER((long)purple_request_field_group_new((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_group_destroy, 1)
    T_NUMBER(purple_request_field_group_destroy, 0)     // PurpleRequestFieldGroup*

    purple_request_field_group_destroy((PurpleRequestFieldGroup *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_group_add_field, 2)
    T_NUMBER(purple_request_field_group_add_field, 0)     // PurpleRequestFieldGroup*
    T_NUMBER(purple_request_field_group_add_field, 1)     // PurpleRequestField*

    purple_request_field_group_add_field((PurpleRequestFieldGroup *)(long)PARAM(0), (PurpleRequestField *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_group_get_title, 1)
    T_NUMBER(purple_request_field_group_get_title, 0)     // PurpleRequestFieldGroup*

    RETURN_STRING((char *)purple_request_field_group_get_title((PurpleRequestFieldGroup *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_group_get_fields, 1)
    T_NUMBER(purple_request_field_group_get_fields, 0)     // PurpleRequestFieldGroup*

    RETURN_NUMBER((long)purple_request_field_group_get_fields((PurpleRequestFieldGroup *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_new, 3)
    T_STRING(purple_request_field_new, 0)     // char*
    T_STRING(purple_request_field_new, 1)     // char*
    T_NUMBER(purple_request_field_new, 2)     // PurpleRequestFieldType

    RETURN_NUMBER((long)purple_request_field_new((char *)PARAM(0), (char *)PARAM(1), (PurpleRequestFieldType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_destroy, 1)
    T_NUMBER(purple_request_field_destroy, 0)     // PurpleRequestField*

    purple_request_field_destroy((PurpleRequestField *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_set_label, 2)
    T_NUMBER(purple_request_field_set_label, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_set_label, 1)     // char*

    purple_request_field_set_label((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_set_visible, 2)
    T_NUMBER(purple_request_field_set_visible, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_set_visible, 1)     // int

    purple_request_field_set_visible((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_set_type_hint, 2)
    T_NUMBER(purple_request_field_set_type_hint, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_set_type_hint, 1)     // char*

    purple_request_field_set_type_hint((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_set_required, 2)
    T_NUMBER(purple_request_field_set_required, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_set_required, 1)     // int

    purple_request_field_set_required((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_type, 1)
    T_NUMBER(purple_request_field_get_type, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_get_type((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_group, 1)
    T_NUMBER(purple_request_field_get_group, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_get_group((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_id, 1)
    T_NUMBER(purple_request_field_get_id, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_get_id((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_label, 1)
    T_NUMBER(purple_request_field_get_label, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_get_label((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_is_visible, 1)
    T_NUMBER(purple_request_field_is_visible, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_is_visible((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_type_hint, 1)
    T_NUMBER(purple_request_field_get_type_hint, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_get_type_hint((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_is_required, 1)
    T_NUMBER(purple_request_field_is_required, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_is_required((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_get_ui_data, 1)
    T_NUMBER(purple_request_field_get_ui_data, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_get_ui_data((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_set_ui_data, 2)
    T_NUMBER(purple_request_field_set_ui_data, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_set_ui_data, 1)     // void*

    purple_request_field_set_ui_data((PurpleRequestField *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_new, 4)
    T_STRING(purple_request_field_string_new, 0)     // char*
    T_STRING(purple_request_field_string_new, 1)     // char*
    T_STRING(purple_request_field_string_new, 2)     // char*
    T_NUMBER(purple_request_field_string_new, 3)     // int

    RETURN_NUMBER((long)purple_request_field_string_new((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_set_default_value, 2)
    T_NUMBER(purple_request_field_string_set_default_value, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_string_set_default_value, 1)     // char*

    purple_request_field_string_set_default_value((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_set_value, 2)
    T_NUMBER(purple_request_field_string_set_value, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_string_set_value, 1)     // char*

    purple_request_field_string_set_value((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_set_masked, 2)
    T_NUMBER(purple_request_field_string_set_masked, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_string_set_masked, 1)     // int

    purple_request_field_string_set_masked((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_set_editable, 2)
    T_NUMBER(purple_request_field_string_set_editable, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_string_set_editable, 1)     // int

    purple_request_field_string_set_editable((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_get_default_value, 1)
    T_NUMBER(purple_request_field_string_get_default_value, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_string_get_default_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_get_value, 1)
    T_NUMBER(purple_request_field_string_get_value, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_string_get_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_is_multiline, 1)
    T_NUMBER(purple_request_field_string_is_multiline, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_string_is_multiline((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_is_masked, 1)
    T_NUMBER(purple_request_field_string_is_masked, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_string_is_masked((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_string_is_editable, 1)
    T_NUMBER(purple_request_field_string_is_editable, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_string_is_editable((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_int_new, 3)
    T_STRING(purple_request_field_int_new, 0)     // char*
    T_STRING(purple_request_field_int_new, 1)     // char*
    T_NUMBER(purple_request_field_int_new, 2)     // int

    RETURN_NUMBER((long)purple_request_field_int_new((char *)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_int_set_default_value, 2)
    T_NUMBER(purple_request_field_int_set_default_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_int_set_default_value, 1)     // int

    purple_request_field_int_set_default_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_int_set_value, 2)
    T_NUMBER(purple_request_field_int_set_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_int_set_value, 1)     // int

    purple_request_field_int_set_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_int_get_default_value, 1)
    T_NUMBER(purple_request_field_int_get_default_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_int_get_default_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_int_get_value, 1)
    T_NUMBER(purple_request_field_int_get_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_int_get_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_bool_new, 3)
    T_STRING(purple_request_field_bool_new, 0)     // char*
    T_STRING(purple_request_field_bool_new, 1)     // char*
    T_NUMBER(purple_request_field_bool_new, 2)     // int

    RETURN_NUMBER((long)purple_request_field_bool_new((char *)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_bool_set_default_value, 2)
    T_NUMBER(purple_request_field_bool_set_default_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_bool_set_default_value, 1)     // int

    purple_request_field_bool_set_default_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_bool_set_value, 2)
    T_NUMBER(purple_request_field_bool_set_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_bool_set_value, 1)     // int

    purple_request_field_bool_set_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_bool_get_default_value, 1)
    T_NUMBER(purple_request_field_bool_get_default_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_bool_get_default_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_bool_get_value, 1)
    T_NUMBER(purple_request_field_bool_get_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_bool_get_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_new, 3)
    T_STRING(purple_request_field_choice_new, 0)     // char*
    T_STRING(purple_request_field_choice_new, 1)     // char*
    T_NUMBER(purple_request_field_choice_new, 2)     // int

    RETURN_NUMBER((long)purple_request_field_choice_new((char *)PARAM(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_add, 2)
    T_NUMBER(purple_request_field_choice_add, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_choice_add, 1)     // char*

    purple_request_field_choice_add((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_set_default_value, 2)
    T_NUMBER(purple_request_field_choice_set_default_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_choice_set_default_value, 1)     // int

    purple_request_field_choice_set_default_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_set_value, 2)
    T_NUMBER(purple_request_field_choice_set_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_choice_set_value, 1)     // int

    purple_request_field_choice_set_value((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_get_default_value, 1)
    T_NUMBER(purple_request_field_choice_get_default_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_choice_get_default_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_get_value, 1)
    T_NUMBER(purple_request_field_choice_get_value, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_choice_get_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_choice_get_labels, 1)
    T_NUMBER(purple_request_field_choice_get_labels, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_choice_get_labels((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_new, 2)
    T_STRING(purple_request_field_list_new, 0)     // char*
    T_STRING(purple_request_field_list_new, 1)     // char*

    RETURN_NUMBER((long)purple_request_field_list_new((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_set_multi_select, 2)
    T_NUMBER(purple_request_field_list_set_multi_select, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_list_set_multi_select, 1)     // int

    purple_request_field_list_set_multi_select((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_get_multi_select, 1)
    T_NUMBER(purple_request_field_list_get_multi_select, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_list_get_multi_select((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_get_data, 2)
    T_NUMBER(purple_request_field_list_get_data, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_list_get_data, 1)     // char*

    RETURN_NUMBER((long)purple_request_field_list_get_data((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_add, 3)
    T_NUMBER(purple_request_field_list_add, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_list_add, 1)     // char*
    T_NUMBER(purple_request_field_list_add, 2)     // void*

    purple_request_field_list_add((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1), (void *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_add_selected, 2)
    T_NUMBER(purple_request_field_list_add_selected, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_list_add_selected, 1)     // char*

    purple_request_field_list_add_selected((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_clear_selected, 1)
    T_NUMBER(purple_request_field_list_clear_selected, 0)     // PurpleRequestField*

    purple_request_field_list_clear_selected((PurpleRequestField *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_set_selected, 2)
    T_NUMBER(purple_request_field_list_set_selected, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_list_set_selected, 1)     // GList*

    purple_request_field_list_set_selected((PurpleRequestField *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_is_selected, 2)
    T_NUMBER(purple_request_field_list_is_selected, 0)     // PurpleRequestField*
    T_STRING(purple_request_field_list_is_selected, 1)     // char*

    RETURN_NUMBER(purple_request_field_list_is_selected((PurpleRequestField *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_get_selected, 1)
    T_NUMBER(purple_request_field_list_get_selected, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_list_get_selected((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_list_get_items, 1)
    T_NUMBER(purple_request_field_list_get_items, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_list_get_items((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_label_new, 2)
    T_STRING(purple_request_field_label_new, 0)     // char*
    T_STRING(purple_request_field_label_new, 1)     // char*

    RETURN_NUMBER((long)purple_request_field_label_new((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_new, 4)
    T_STRING(purple_request_field_image_new, 0)     // char*
    T_STRING(purple_request_field_image_new, 1)     // char*
    T_STRING(purple_request_field_image_new, 2)     // char*
    T_NUMBER(purple_request_field_image_new, 3)     // gsize

    RETURN_NUMBER((long)purple_request_field_image_new((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (gsize)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_set_scale, 3)
    T_NUMBER(purple_request_field_image_set_scale, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_image_set_scale, 1)     // unsigned int
    T_NUMBER(purple_request_field_image_set_scale, 2)     // unsigned int

    purple_request_field_image_set_scale((PurpleRequestField *)(long)PARAM(0), (unsigned int)PARAM(1), (unsigned int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_get_buffer, 1)
    T_NUMBER(purple_request_field_image_get_buffer, 0)     // PurpleRequestField*

    RETURN_STRING((char *)purple_request_field_image_get_buffer((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_get_size, 1)
    T_NUMBER(purple_request_field_image_get_size, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_image_get_size((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_get_scale_x, 1)
    T_NUMBER(purple_request_field_image_get_scale_x, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_image_get_scale_x((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_image_get_scale_y, 1)
    T_NUMBER(purple_request_field_image_get_scale_y, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_image_get_scale_y((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_new, 3)
    T_STRING(purple_request_field_account_new, 0)     // char*
    T_STRING(purple_request_field_account_new, 1)     // char*
    T_NUMBER(purple_request_field_account_new, 2)     // PurpleAccount*

    RETURN_NUMBER((long)purple_request_field_account_new((char *)PARAM(0), (char *)PARAM(1), (PurpleAccount *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_set_default_value, 2)
    T_NUMBER(purple_request_field_account_set_default_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_account_set_default_value, 1)     // PurpleAccount*

    purple_request_field_account_set_default_value((PurpleRequestField *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_set_value, 2)
    T_NUMBER(purple_request_field_account_set_value, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_account_set_value, 1)     // PurpleAccount*

    purple_request_field_account_set_value((PurpleRequestField *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_set_show_all, 2)
    T_NUMBER(purple_request_field_account_set_show_all, 0)     // PurpleRequestField*
    T_NUMBER(purple_request_field_account_set_show_all, 1)     // int

    purple_request_field_account_set_show_all((PurpleRequestField *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_get_default_value, 1)
    T_NUMBER(purple_request_field_account_get_default_value, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_account_get_default_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_get_value, 1)
    T_NUMBER(purple_request_field_account_get_value, 0)     // PurpleRequestField*

    RETURN_NUMBER((long)purple_request_field_account_get_value((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_request_field_account_get_show_all, 1)
    T_NUMBER(purple_request_field_account_get_show_all, 0)     // PurpleRequestField*

    RETURN_NUMBER(purple_request_field_account_get_show_all((PurpleRequestField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_request_field_account_get_filter,1)
        T_NUMBER(purple_request_field_account_get_show_all, 0) // PurpleRequestField*

        RETURN_NUMBER(purple_request_field_account_get_filter((PurpleRequestField*)(long)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_input,16)
        T_NUMBER(purple_request_input, 0) // void*
        T_STRING(purple_request_input, 1) // char*
        T_STRING(purple_request_input, 2) // char*
        T_STRING(purple_request_input, 3) // char*
        T_STRING(purple_request_input, 4) // char*
        T_NUMBER(purple_request_input, 5) // int
        T_NUMBER(purple_request_input, 6) // int
        T_STRING(purple_request_input, 7) // char*
        T_STRING(purple_request_input, 8) // char*
        T_NUMBER(purple_request_input, 9) // GCallback
        T_STRING(purple_request_input, 10) // char*
        T_NUMBER(purple_request_input, 11) // GCallback
        T_NUMBER(purple_request_input, 12) // PurpleAccount*
        T_STRING(purple_request_input, 13) // char*
        T_NUMBER(purple_request_input, 14) // PurpleConversation*
        T_NUMBER(purple_request_input, 15) // void*

        RETURN_NUMBER((long)purple_request_input((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (char*)PARAM(4), (int)PARAM(5), (int)PARAM(6), (char*)PARAM(7), (char*)PARAM(8), (GCallback)PARAM(9), (char*)PARAM(10), (GCallback)PARAM(11), (PurpleAccount*)(long)PARAM(12), (char*)PARAM(13), (PurpleConversation*)(long)PARAM(14), (void *)(long)PARAM(15)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_choice,13)
        T_NUMBER(purple_request_choice, 0) // void*
        T_STRING(purple_request_choice, 1) // char*
        T_STRING(purple_request_choice, 2) // char*
        T_STRING(purple_request_choice, 3) // char*
        T_NUMBER(purple_request_choice, 4) // int
        T_STRING(purple_request_choice, 5) // char*
        T_NUMBER(purple_request_choice, 6) // GCallback
        T_STRING(purple_request_choice, 7) // char*
        T_NUMBER(purple_request_choice, 8) // GCallback
        T_NUMBER(purple_request_choice, 9) // PurpleAccount*
        T_STRING(purple_request_choice, 10) // char*
        T_NUMBER(purple_request_choice, 11) // PurpleConversation*
        T_NUMBER(purple_request_choice, 12) // void*

        RETURN_NUMBER((long)purple_request_choice((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (char*)PARAM(5), (GCallback)PARAM(6), (char*)PARAM(7), (GCallback)PARAM(8), (PurpleAccount*)(long)PARAM(9), (char*)PARAM(10), (PurpleConversation*)(long)PARAM(11), (void *)(long)PARAM(12)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_choice_varg,14)
        T_NUMBER(purple_request_choice_varg, 0) // void*
        T_STRING(purple_request_choice_varg, 1) // char*
        T_STRING(purple_request_choice_varg, 2) // char*
        T_STRING(purple_request_choice_varg, 3) // char*
        T_NUMBER(purple_request_choice_varg, 4) // int
        T_STRING(purple_request_choice_varg, 5) // char*
        T_NUMBER(purple_request_choice_varg, 6) // GCallback
        T_STRING(purple_request_choice_varg, 7) // char*
        T_NUMBER(purple_request_choice_varg, 8) // GCallback
        T_NUMBER(purple_request_choice_varg, 9) // PurpleAccount*
        T_STRING(purple_request_choice_varg, 10) // char*
        T_NUMBER(purple_request_choice_varg, 11) // PurpleConversation*
        T_NUMBER(purple_request_choice_varg, 12) // void*
        T_NUMBER(purple_request_choice_varg, 13) // va_list

        RETURN_NUMBER((long)purple_request_choice_varg((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (char*)PARAM(5), (GCallback)PARAM(6), (char*)PARAM(7), (GCallback)PARAM(8), (PurpleAccount*)(long)PARAM(9), (char*)PARAM(10), (PurpleConversation*)(long)PARAM(11), (void *)(long)PARAM(12), (va_list)PARAM(13)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_action,10)
        T_NUMBER(purple_request_action, 0) // void*
        T_STRING(purple_request_action, 1) // char*
        T_STRING(purple_request_action, 2) // char*
        T_STRING(purple_request_action, 3) // char*
        T_NUMBER(purple_request_action, 4) // int
        T_NUMBER(purple_request_action, 5) // PurpleAccount*
        T_STRING(purple_request_action, 6) // char*
        T_NUMBER(purple_request_action, 7) // PurpleConversation*
        T_NUMBER(purple_request_action, 8) // void*
        T_NUMBER(purple_request_action, 9) // size_t

        RETURN_NUMBER((long)purple_request_action((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (PurpleAccount*)(long)PARAM(5), (char*)PARAM(6), (PurpleConversation*)(long)PARAM(7), (void *)(long)PARAM(8), (size_t)PARAM(9)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_action_varg,11)
        T_NUMBER(purple_request_action_varg, 0) // void*
        T_STRING(purple_request_action_varg, 1) // char*
        T_STRING(purple_request_action_varg, 2) // char*
        T_STRING(purple_request_action_varg, 3) // char*
        T_NUMBER(purple_request_action_varg, 4) // int
        T_NUMBER(purple_request_action_varg, 5) // PurpleAccount*
        T_STRING(purple_request_action_varg, 6) // char*
        T_NUMBER(purple_request_action_varg, 7) // PurpleConversation*
        T_NUMBER(purple_request_action_varg, 8) // void*
        T_NUMBER(purple_request_action_varg, 9) // size_t
        T_NUMBER(purple_request_action_varg, 10) // va_list

        RETURN_NUMBER((long)purple_request_action_varg((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (PurpleAccount*)(long)PARAM(5), (char*)PARAM(6), (PurpleConversation*)(long)PARAM(7), (void *)(long)PARAM(8), (size_t)PARAM(9), (va_list)PARAM(10)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_fields,13)
        T_NUMBER(purple_request_fields, 0) // void*
        T_STRING(purple_request_fields, 1) // char*
        T_STRING(purple_request_fields, 2) // char*
        T_STRING(purple_request_fields, 3) // char*
        T_NUMBER(purple_request_fields, 4) // PurpleRequestFields*
        T_STRING(purple_request_fields, 5) // char*
        T_NUMBER(purple_request_fields, 6) // GCallback
        T_STRING(purple_request_fields, 7) // char*
        T_NUMBER(purple_request_fields, 8) // GCallback
        T_NUMBER(purple_request_fields, 9) // PurpleAccount*
        T_STRING(purple_request_fields, 10) // char*
        T_NUMBER(purple_request_fields, 11) // PurpleConversation*
        T_NUMBER(purple_request_fields, 12) // void*

        RETURN_NUMBER((long)purple_request_fields((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (char*)PARAM(3), (PurpleRequestFields*)(long)PARAM(4), (char*)PARAM(5), (GCallback)PARAM(6), (char*)PARAM(7), (GCallback)PARAM(8), (PurpleAccount*)(long)PARAM(9), (char*)PARAM(10), (PurpleConversation*)(long)PARAM(11), (void *)(long)PARAM(12)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_close,2)
        T_NUMBER(purple_request_close, 0) // PurpleRequestType
        T_NUMBER(purple_request_close, 1) // void*

        purple_request_close((PurpleRequestType)PARAM(0), (void *)(long)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_close_with_handle,1)
        T_NUMBER(purple_request_close_with_handle, 0) // void*

        purple_request_close_with_handle((void *)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_file,10)
        T_NUMBER(purple_request_file, 0) // void*
        T_STRING(purple_request_file, 1) // char*
        T_STRING(purple_request_file, 2) // char*
        T_NUMBER(purple_request_file, 3) // int
        T_NUMBER(purple_request_file, 4) // GCallback
        T_NUMBER(purple_request_file, 5) // GCallback
        T_NUMBER(purple_request_file, 6) // PurpleAccount*
        T_STRING(purple_request_file, 7) // char*
        T_NUMBER(purple_request_file, 8) // PurpleConversation*
        T_NUMBER(purple_request_file, 9) // void*

        RETURN_NUMBER((long)purple_request_file((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (int)PARAM(3), (GCallback)PARAM(4), (GCallback)PARAM(5), (PurpleAccount*)(long)PARAM(6), (char*)PARAM(7), (PurpleConversation*)(long)PARAM(8), (void *)(long)PARAM(9)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_folder,9)
        T_NUMBER(purple_request_folder, 0) // void*
        T_STRING(purple_request_folder, 1) // char*
        T_STRING(purple_request_folder, 2) // char*
        T_NUMBER(purple_request_folder, 3) // GCallback
        T_NUMBER(purple_request_folder, 4) // GCallback
        T_NUMBER(purple_request_folder, 5) // PurpleAccount*
        T_STRING(purple_request_folder, 6) // char*
        T_NUMBER(purple_request_folder, 7) // PurpleConversation*
        T_NUMBER(purple_request_folder, 8) // void*

        RETURN_NUMBER((long)purple_request_folder((void *)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (GCallback)PARAM(3), (GCallback)PARAM(4), (PurpleAccount*)(long)PARAM(5), (char*)PARAM(6), (PurpleConversation*)(long)PARAM(7), (void *)(long)PARAM(8)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_set_ui_ops,1)

        // ... parameter 0 is by reference (PurpleRequestUiOps*)
        PurpleRequestUiOps local_parameter_0;

        purple_request_set_ui_ops(&local_parameter_0);
        SET_NUMBER(0, (long)local_parameter_0)
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_request_get_ui_ops,0)

        RETURN_NUMBER((long)purple_request_get_ui_ops())
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_show_with_account, 1)
    T_NUMBER(purple_roomlist_show_with_account, 0)     // PurpleAccount*

    purple_roomlist_show_with_account((PurpleAccount *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_new, 1)
    T_NUMBER(purple_roomlist_new, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_roomlist_new((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_ref, 1)
    T_NUMBER(purple_roomlist_ref, 0)     // PurpleRoomlist*

    purple_roomlist_ref((PurpleRoomlist *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_unref, 1)
    T_NUMBER(purple_roomlist_unref, 0)     // PurpleRoomlist*

    purple_roomlist_unref((PurpleRoomlist *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_set_fields, 2)
    T_NUMBER(purple_roomlist_set_fields, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_set_fields, 1)     // GList*

    purple_roomlist_set_fields((PurpleRoomlist *)(long)PARAM(0), (GList *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_set_in_progress, 2)
    T_NUMBER(purple_roomlist_set_in_progress, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_set_in_progress, 1)     // int

    purple_roomlist_set_in_progress((PurpleRoomlist *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_get_in_progress, 1)
    T_NUMBER(purple_roomlist_get_in_progress, 0)     // PurpleRoomlist*

    RETURN_NUMBER(purple_roomlist_get_in_progress((PurpleRoomlist *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_add, 2)
    T_NUMBER(purple_roomlist_room_add, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_room_add, 1)     // PurpleRoomlistRoom*

    purple_roomlist_room_add((PurpleRoomlist *)(long)PARAM(0), (PurpleRoomlistRoom *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_get_list, 1)
    T_NUMBER(purple_roomlist_get_list, 0)     // PurpleConnection*

    RETURN_NUMBER((long)purple_roomlist_get_list((PurpleConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_cancel_get_list, 1)
    T_NUMBER(purple_roomlist_cancel_get_list, 0)     // PurpleRoomlist*

    purple_roomlist_cancel_get_list((PurpleRoomlist *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_expand_category, 2)
    T_NUMBER(purple_roomlist_expand_category, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_expand_category, 1)     // PurpleRoomlistRoom*

    purple_roomlist_expand_category((PurpleRoomlist *)(long)PARAM(0), (PurpleRoomlistRoom *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_get_fields, 1)
    T_NUMBER(purple_roomlist_get_fields, 0)     // PurpleRoomlist*

    RETURN_NUMBER((long)purple_roomlist_get_fields((PurpleRoomlist *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_new, 3)
    T_NUMBER(purple_roomlist_room_new, 0)     // PurpleRoomlistRoomType
    T_STRING(purple_roomlist_room_new, 1)     // char*
    T_NUMBER(purple_roomlist_room_new, 2)     // PurpleRoomlistRoom*

    RETURN_NUMBER((long)purple_roomlist_room_new((PurpleRoomlistRoomType)PARAM(0), (char *)PARAM(1), (PurpleRoomlistRoom *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_add_field, 3)
    T_NUMBER(purple_roomlist_room_add_field, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_room_add_field, 1)     // PurpleRoomlistRoom*
    T_NUMBER(purple_roomlist_room_add_field, 2)     // void*

    purple_roomlist_room_add_field((PurpleRoomlist *)(long)PARAM(0), (PurpleRoomlistRoom *)(long)PARAM(1), (void *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_join, 2)
    T_NUMBER(purple_roomlist_room_join, 0)     // PurpleRoomlist*
    T_NUMBER(purple_roomlist_room_join, 1)     // PurpleRoomlistRoom*

    purple_roomlist_room_join((PurpleRoomlist *)(long)PARAM(0), (PurpleRoomlistRoom *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_get_type, 1)
    T_NUMBER(purple_roomlist_room_get_type, 0)     // PurpleRoomlistRoom*

    RETURN_NUMBER(purple_roomlist_room_get_type((PurpleRoomlistRoom *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_get_name, 1)
    T_NUMBER(purple_roomlist_room_get_name, 0)     // PurpleRoomlistRoom*

    RETURN_STRING((char *)purple_roomlist_room_get_name((PurpleRoomlistRoom *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_get_parent, 1)
    T_NUMBER(purple_roomlist_room_get_parent, 0)     // PurpleRoomlistRoom*

    RETURN_NUMBER((long)purple_roomlist_room_get_parent((PurpleRoomlistRoom *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_room_get_fields, 1)
    T_NUMBER(purple_roomlist_room_get_fields, 0)     // PurpleRoomlistRoom*

    RETURN_NUMBER((long)purple_roomlist_room_get_fields((PurpleRoomlistRoom *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_field_new, 4)
    T_NUMBER(purple_roomlist_field_new, 0)     // PurpleRoomlistFieldType
    T_STRING(purple_roomlist_field_new, 1)     // char*
    T_STRING(purple_roomlist_field_new, 2)     // char*
    T_NUMBER(purple_roomlist_field_new, 3)     // int

    RETURN_NUMBER((long)purple_roomlist_field_new((PurpleRoomlistFieldType)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_field_get_type, 1)
    T_NUMBER(purple_roomlist_field_get_type, 0)     // PurpleRoomlistField*

    RETURN_NUMBER(purple_roomlist_field_get_type((PurpleRoomlistField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_field_get_label, 1)
    T_NUMBER(purple_roomlist_field_get_label, 0)     // PurpleRoomlistField*

    RETURN_STRING((char *)purple_roomlist_field_get_label((PurpleRoomlistField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_field_get_hidden, 1)
    T_NUMBER(purple_roomlist_field_get_hidden, 0)     // PurpleRoomlistField*

    RETURN_NUMBER(purple_roomlist_field_get_hidden((PurpleRoomlistField *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_set_ui_ops, 1)
    T_NUMBER(purple_roomlist_set_ui_ops, 0)     // PurpleRoomlistUiOps*

    purple_roomlist_set_ui_ops((PurpleRoomlistUiOps *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_roomlist_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_roomlist_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_new, 2)
    T_STRING(purple_savedstatus_new, 0)     // char*
    T_NUMBER(purple_savedstatus_new, 1)     // PurpleStatusPrimitive

    RETURN_NUMBER((long)purple_savedstatus_new((char *)PARAM(0), (PurpleStatusPrimitive)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_set_title, 2)
    T_NUMBER(purple_savedstatus_set_title, 0)     // PurpleSavedStatus*
    T_STRING(purple_savedstatus_set_title, 1)     // char*

    purple_savedstatus_set_title((PurpleSavedStatus *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_set_type, 2)
    T_NUMBER(purple_savedstatus_set_type, 0)     // PurpleSavedStatus*
    T_NUMBER(purple_savedstatus_set_type, 1)     // PurpleStatusPrimitive

    purple_savedstatus_set_type((PurpleSavedStatus *)(long)PARAM(0), (PurpleStatusPrimitive)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_set_message, 2)
    T_NUMBER(purple_savedstatus_set_message, 0)     // PurpleSavedStatus*
    T_STRING(purple_savedstatus_set_message, 1)     // char*

    purple_savedstatus_set_message((PurpleSavedStatus *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_set_substatus, 4)
    T_NUMBER(purple_savedstatus_set_substatus, 0)     // PurpleSavedStatus*
    T_NUMBER(purple_savedstatus_set_substatus, 1)     // PurpleAccount*
    T_NUMBER(purple_savedstatus_set_substatus, 2)     // PurpleStatusType*
    T_STRING(purple_savedstatus_set_substatus, 3)     // char*

    purple_savedstatus_set_substatus((PurpleSavedStatus *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1), (PurpleStatusType *)(long)PARAM(2), (char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_unset_substatus, 2)
    T_NUMBER(purple_savedstatus_unset_substatus, 0)     // PurpleSavedStatus*
    T_NUMBER(purple_savedstatus_unset_substatus, 1)     // PurpleAccount*

    purple_savedstatus_unset_substatus((PurpleSavedStatus *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_delete, 1)
    T_STRING(purple_savedstatus_delete, 0)     // char*

    RETURN_NUMBER(purple_savedstatus_delete((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_delete_by_status, 1)
    T_NUMBER(purple_savedstatus_delete_by_status, 0)     // PurpleSavedStatus*

    purple_savedstatus_delete_by_status((PurpleSavedStatus *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatuses_get_all, 0)

    RETURN_NUMBER((long)purple_savedstatuses_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatuses_get_popular, 1)
    T_NUMBER(purple_savedstatuses_get_popular, 0)     // unsigned int

    RETURN_NUMBER((long)purple_savedstatuses_get_popular((unsigned int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_current, 0)

    RETURN_NUMBER((long)purple_savedstatus_get_current())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_default, 0)

    RETURN_NUMBER((long)purple_savedstatus_get_default())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_idleaway, 0)

    RETURN_NUMBER((long)purple_savedstatus_get_idleaway())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_is_idleaway, 0)

    RETURN_NUMBER(purple_savedstatus_is_idleaway())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_set_idleaway, 1)
    T_NUMBER(purple_savedstatus_set_idleaway, 0)     // int

    purple_savedstatus_set_idleaway((int)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_startup, 0)

    RETURN_NUMBER((long)purple_savedstatus_get_startup())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_find, 1)
    T_STRING(purple_savedstatus_find, 0)     // char*

    RETURN_NUMBER((long)purple_savedstatus_find((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_find_by_creation_time, 1)
    T_NUMBER(purple_savedstatus_find_by_creation_time, 0)     // time_t

    RETURN_NUMBER((long)purple_savedstatus_find_by_creation_time((time_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_find_transient_by_type_and_message, 2)
    T_NUMBER(purple_savedstatus_find_transient_by_type_and_message, 0)     // PurpleStatusPrimitive
    T_STRING(purple_savedstatus_find_transient_by_type_and_message, 1)     // char*

    RETURN_NUMBER((long)purple_savedstatus_find_transient_by_type_and_message((PurpleStatusPrimitive)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_is_transient, 1)
    T_NUMBER(purple_savedstatus_is_transient, 0)     // PurpleSavedStatus*

    RETURN_NUMBER(purple_savedstatus_is_transient((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_title, 1)
    T_NUMBER(purple_savedstatus_get_title, 0)     // PurpleSavedStatus*

    RETURN_STRING((char *)purple_savedstatus_get_title((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_type, 1)
    T_NUMBER(purple_savedstatus_get_type, 0)     // PurpleSavedStatus*

    RETURN_NUMBER(purple_savedstatus_get_type((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_message, 1)
    T_NUMBER(purple_savedstatus_get_message, 0)     // PurpleSavedStatus*

    RETURN_STRING((char *)purple_savedstatus_get_message((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_creation_time, 1)
    T_NUMBER(purple_savedstatus_get_creation_time, 0)     // PurpleSavedStatus*

    RETURN_NUMBER(purple_savedstatus_get_creation_time((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_has_substatuses, 1)
    T_NUMBER(purple_savedstatus_has_substatuses, 0)     // PurpleSavedStatus*

    RETURN_NUMBER(purple_savedstatus_has_substatuses((PurpleSavedStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_get_substatus, 2)
    T_NUMBER(purple_savedstatus_get_substatus, 0)     // PurpleSavedStatus*
    T_NUMBER(purple_savedstatus_get_substatus, 1)     // PurpleAccount*

    RETURN_NUMBER((long)purple_savedstatus_get_substatus((PurpleSavedStatus *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_substatus_get_type, 1)
    T_NUMBER(purple_savedstatus_substatus_get_type, 0)     // PurpleSavedStatusSub*

    RETURN_NUMBER((long)purple_savedstatus_substatus_get_type((PurpleSavedStatusSub *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_substatus_get_message, 1)
    T_NUMBER(purple_savedstatus_substatus_get_message, 0)     // PurpleSavedStatusSub*

    RETURN_STRING((char *)purple_savedstatus_substatus_get_message((PurpleSavedStatusSub *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_activate, 1)
    T_NUMBER(purple_savedstatus_activate, 0)     // PurpleSavedStatus*

    purple_savedstatus_activate((PurpleSavedStatus *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatus_activate_for_account, 2)
    T_NUMBER(purple_savedstatus_activate_for_account, 0)     // PurpleSavedStatus*
    T_NUMBER(purple_savedstatus_activate_for_account, 1)     // PurpleAccount*

    purple_savedstatus_activate_for_account((PurpleSavedStatus *)(long)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatuses_get_handle, 0)

    RETURN_NUMBER((long)purple_savedstatuses_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatuses_init, 0)

    purple_savedstatuses_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_savedstatuses_uninit, 0)

    purple_savedstatuses_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(serv_send_typing, 3)
    T_NUMBER(serv_send_typing, 0)     // PurpleConnection*
    T_STRING(serv_send_typing, 1)     // char*
    T_NUMBER(serv_send_typing, 2)     // PurpleTypingState

    RETURN_NUMBER(serv_send_typing((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (PurpleTypingState)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(serv_move_buddy,0)

        serv_move_buddy();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_send_im,1)
        T_NUMBER(serv_send_im, 0) // PurpleMessageFlags

        RETURN_NUMBER(serv_send_im((PurpleMessageFlags)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_get_attention_type_from_code,2)
        T_NUMBER(purple_get_attention_type_from_code, 0) // PurpleAccount*
        T_NUMBER(purple_get_attention_type_from_code, 1) // guint

        RETURN_NUMBER((long)purple_get_attention_type_from_code((PurpleAccount*)(long)PARAM(0), (guint)PARAM(1)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_send_attention,3)
        T_NUMBER(serv_send_attention, 0) // PurpleConnection*
        T_STRING(serv_send_attention, 1) // char*
        T_NUMBER(serv_send_attention, 2) // guint

        serv_send_attention((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (guint)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_got_attention,3)
        T_NUMBER(serv_got_attention, 0) // PurpleConnection*
        T_STRING(serv_got_attention, 1) // char*
        T_NUMBER(serv_got_attention, 2) // guint

        serv_got_attention((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (guint)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_get_info,0)

        serv_get_info();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_set_info,0)

        serv_set_info();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_add_permit,0)

        serv_add_permit();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_add_deny,0)

        serv_add_deny();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_rem_permit,0)

        serv_rem_permit();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_rem_deny,0)

        serv_rem_deny();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_set_permit_deny,0)

        serv_set_permit_deny();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_chat_invite,0)

        serv_chat_invite();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_chat_leave,0)

        serv_chat_leave();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_chat_whisper,0)

        serv_chat_whisper();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_chat_send,1)
        T_NUMBER(serv_chat_send, 0) // PurpleMessageFlags

        RETURN_NUMBER(serv_chat_send((PurpleMessageFlags)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_alias_buddy,0)

        serv_alias_buddy();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_got_alias,3)
        T_NUMBER(serv_got_alias, 0) // PurpleConnection*
        T_STRING(serv_got_alias, 1) // char*
        T_STRING(serv_got_alias, 2) // char*

        serv_got_alias((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_serv_got_private_alias,3)
        T_NUMBER(purple_serv_got_private_alias, 0) // PurpleConnection*
        T_STRING(purple_serv_got_private_alias, 1) // char*
        T_STRING(purple_serv_got_private_alias, 2) // char*

        purple_serv_got_private_alias((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_got_typing,4)
        T_NUMBER(serv_got_typing, 0) // PurpleConnection*
        T_STRING(serv_got_typing, 1) // char*
        T_NUMBER(serv_got_typing, 2) // int
        T_NUMBER(serv_got_typing, 3) // PurpleTypingState

        serv_got_typing((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (int)PARAM(2), (PurpleTypingState)PARAM(3));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_got_typing_stopped,2)
        T_NUMBER(serv_got_typing_stopped, 0) // PurpleConnection*
        T_STRING(serv_got_typing_stopped, 1) // char*

        serv_got_typing_stopped((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_got_im,5)
        T_NUMBER(serv_got_im, 0) // PurpleConnection*
        T_STRING(serv_got_im, 1) // char*
        T_STRING(serv_got_im, 2) // char*
        T_NUMBER(serv_got_im, 3) // PurpleMessageFlags
        T_NUMBER(serv_got_im, 4) // time_t

        serv_got_im((PurpleConnection*)(long)PARAM(0), (char*)PARAM(1), (char*)PARAM(2), (PurpleMessageFlags)PARAM(3), (time_t)PARAM(4));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_join_chat,1)
        T_NUMBER(serv_join_chat, 0) // GHashTable*

        serv_join_chat((GHashTable*)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(serv_reject_chat,1)
        T_NUMBER(serv_reject_chat, 0) // GHashTable*

        serv_reject_chat((GHashTable*)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
 */
CONCEPT_FUNCTION_IMPL(serv_got_chat_invite, 5)
    T_NUMBER(serv_got_chat_invite, 0)     // PurpleConnection*
    T_STRING(serv_got_chat_invite, 1)     // char*
    T_STRING(serv_got_chat_invite, 2)     // char*
    T_STRING(serv_got_chat_invite, 3)     // char*
    T_NUMBER(serv_got_chat_invite, 4)     // GHashTable*

    serv_got_chat_invite((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (GHashTable *)(long)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(serv_got_joined_chat, 3)
    T_NUMBER(serv_got_joined_chat, 0)     // PurpleConnection*
    T_NUMBER(serv_got_joined_chat, 1)     // int
    T_STRING(serv_got_joined_chat, 2)     // char*

    RETURN_NUMBER((long)serv_got_joined_chat((PurpleConnection *)(long)PARAM(0), (int)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_serv_got_join_chat_failed, 2)
    T_NUMBER(purple_serv_got_join_chat_failed, 0)     // PurpleConnection*
    T_NUMBER(purple_serv_got_join_chat_failed, 1)     // GHashTable*

    purple_serv_got_join_chat_failed((PurpleConnection *)(long)PARAM(0), (GHashTable *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(serv_got_chat_left, 2)
    T_NUMBER(serv_got_chat_left, 0)     // PurpleConnection*
    T_NUMBER(serv_got_chat_left, 1)     // int

    serv_got_chat_left((PurpleConnection *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(serv_got_chat_in, 6)
    T_NUMBER(serv_got_chat_in, 0)     // PurpleConnection*
    T_NUMBER(serv_got_chat_in, 1)     // int
    T_STRING(serv_got_chat_in, 2)     // char*
    T_NUMBER(serv_got_chat_in, 3)     // PurpleMessageFlags
    T_STRING(serv_got_chat_in, 4)     // char*
    T_NUMBER(serv_got_chat_in, 5)     // time_t

    serv_got_chat_in((PurpleConnection *)(long)PARAM(0), (int)PARAM(1), (char *)PARAM(2), (PurpleMessageFlags)PARAM(3), (char *)PARAM(4), (time_t)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(serv_send_file, 3)
    T_NUMBER(serv_send_file, 0)     // PurpleConnection*
    T_STRING(serv_send_file, 1)     // char*
    T_STRING(serv_send_file, 2)     // char*

    serv_send_file((PurpleConnection *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_signal_register,5)
        T_NUMBER(serv_send_file, 0) // void*
        T_STRING(serv_send_file, 1) // char*
        T_NUMBER(serv_send_file, 2) // PurpleSignalMarshalFunc
        T_NUMBER(serv_send_file, 3) // PurpleValue*
        T_NUMBER(serv_send_file, 4) // int

        RETURN_NUMBER(purple_signal_register((void *)(long)PARAM(0), (char*)PARAM(1), (PurpleSignalMarshalFunc)PARAM(2), (PurpleValue*)(long)PARAM(3), (int)PARAM(4)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_unregister,2)
        T_NUMBER(purple_signal_unregister, 0) // void*
        T_STRING(purple_signal_unregister, 1) // char*

        purple_signal_unregister((void *)(long)PARAM(0), (char*)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signals_unregister_by_instance,1)
        T_NUMBER(purple_signals_unregister_by_instance, 0) // void*

        purple_signals_unregister_by_instance((void *)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_get_values,5)
        T_NUMBER(purple_signal_get_values, 0) // void*
        T_STRING(purple_signal_get_values, 1) // char*

        // ... parameter 2 is by reference (PurpleValue**)
        PurpleValue* local_parameter_2;
        // ... parameter 3 is by reference (int*)
        int local_parameter_3;
        // ... parameter 4 is by reference (PurpleValue***)
        PurpleValue** local_parameter_4;

        purple_signal_get_values((void *)(long)PARAM(0), (char*)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4);
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
 */
CONCEPT_FUNCTION_IMPL(purple_signal_connect_priority, 5)
    T_NUMBER(purple_signal_connect_priority, 0)     // void*
    T_STRING(purple_signal_connect_priority, 1)     // char*
//T_NUMBER(2) // void*
    T_DELEGATE(purple_signal_connect_priority, 3)   // PurpleCallback
//T_NUMBER(4) // void*
    T_NUMBER(purple_signal_connect_priority, 4)     // int

    int handle = 0;
    RETURN_NUMBER(purple_signal_connect_priority((void *)(long)PARAM(0), (char *)PARAM(1), &handle, PURPLE_CALLBACK(PurpleCallback_F), (void *)PARAMETER(3), (int)PARAM(4)))
    SET_NUMBER(2, handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signal_connect, 4)
    T_NUMBER(purple_signal_connect, 0)     // void*
    T_STRING(purple_signal_connect, 1)     // char*
//T_NUMBER(2) // void*
    T_DELEGATE(purple_signal_connect, 3)   // PurpleCallback

    int handle = 0;
    RETURN_NUMBER(purple_signal_connect((void *)(long)PARAM(0), (char *)PARAM(1), &handle, PURPLE_CALLBACK(PurpleCallback_F), (void *)PARAMETER(3)))
    SET_NUMBER(2, handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signal_connect2, 4)
    T_NUMBER(purple_signal_connect2, 0)     // void*
    T_STRING(purple_signal_connect2, 1)     // char*
//T_NUMBER(2) // void*
    T_DELEGATE(purple_signal_connect2, 3)   // PurpleCallback

    int handle = 0;
    RETURN_NUMBER(purple_signal_connect((void *)(long)PARAM(0), (char *)PARAM(1), &handle, PURPLE_CALLBACK(PurpleCallback_F2), (void *)PARAMETER(3)))
    SET_NUMBER(2, handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signal_connect3, 4)
    T_NUMBER(purple_signal_connect3, 0)     // void*
    T_STRING(purple_signal_connect3, 1)     // char*
//T_NUMBER(2) // void*
    T_DELEGATE(purple_signal_connect3, 3)   // PurpleCallback

    int handle = 0;
    RETURN_NUMBER(purple_signal_connect((void *)(long)PARAM(0), (char *)PARAM(1), &handle, PURPLE_CALLBACK(PurpleCallback_F3), (void *)PARAMETER(3)))
    SET_NUMBER(2, handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signal_connect4, 4)
    T_NUMBER(purple_signal_connect4, 0)     // void*
    T_STRING(purple_signal_connect4, 1)     // char*
//T_NUMBER(2) // void*
    T_DELEGATE(purple_signal_connect4, 3)   // PurpleCallback

    int handle = 0;
    RETURN_NUMBER(purple_signal_connect((void *)(long)PARAM(0), (char *)PARAM(1), &handle, PURPLE_CALLBACK(PurpleCallback_F4), (void *)PARAMETER(3)))
    SET_NUMBER(2, handle);
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_signal_connect_priority_vargs,6)
        T_NUMBER(purple_signal_connect4, 0) // void*
        T_STRING(purple_signal_connect4, 1) // char*
        T_NUMBER(purple_signal_connect4, 2) // void*
        T_NUMBER(purple_signal_connect4, 3) // PurpleCallback
        T_NUMBER(purple_signal_connect4, 4) // void*
        T_NUMBER(purple_signal_connect4, 5) // int

        RETURN_NUMBER(purple_signal_connect_priority_vargs((void *)(long)PARAM(0), (char*)PARAM(1), (void *)(long)PARAM(2), (PurpleCallback)PARAM(3), (void *)(long)PARAM(4), (int)PARAM(5)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_connect_vargs,5)
        T_NUMBER(purple_signal_connect_vargs, 0) // void*
        T_STRING(purple_signal_connect_vargs, 1) // char*
        T_NUMBER(purple_signal_connect_vargs, 2) // void*
        T_NUMBER(purple_signal_connect_vargs, 3) // PurpleCallback
        T_NUMBER(purple_signal_connect_vargs, 4) // void*

        RETURN_NUMBER(purple_signal_connect_vargs((void *)(long)PARAM(0), (char*)PARAM(1), (void *)(long)PARAM(2), (PurpleCallback)PARAM(3), (void *)(long)PARAM(4)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_disconnect,4)
        T_NUMBER(purple_signal_disconnect, 0) // void*
        T_STRING(purple_signal_disconnect, 1) // char*
        T_NUMBER(purple_signal_disconnect, 2) // void*
        T_NUMBER(purple_signal_disconnect, 3) // PurpleCallback

        purple_signal_disconnect((void *)(long)PARAM(0), (char*)PARAM(1), (void *)(long)PARAM(2), (PurpleCallback)PARAM(3));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------*/
CONCEPT_FUNCTION_IMPL(purple_signals_disconnect_by_handle, 1)
    T_NUMBER(purple_signals_disconnect_by_handle, 0)     // void*

    purple_signals_disconnect_by_handle((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signal_emit, 2)
    T_NUMBER(purple_signal_emit, 0)     // void*
    T_STRING(purple_signal_emit, 1)     // char*

    purple_signal_emit((void *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*
   CONCEPT_FUNCTION_IMPL(purple_signal_emit_vargs,3)
        T_NUMBER(purple_signal_emit_vargs, 0) // void*
        T_STRING(purple_signal_emit_vargs, 1) // char*
        T_NUMBER(purple_signal_emit_vargs, 2) // va_list

        purple_signal_emit_vargs((void *)(long)PARAM(0), (char*)PARAM(1), (va_list)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_emit_return_1,2)
        T_NUMBER(purple_signal_emit_return_1, 0) // void*
        T_STRING(purple_signal_emit_return_1, 1) // char*

        RETURN_NUMBER((long)purple_signal_emit_return_1((void *)(long)PARAM(0), (char*)PARAM(1)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_signal_emit_vargs_return_1,3)
        T_NUMBER(purple_signal_emit_vargs_return_1, 0) // void*
        T_STRING(purple_signal_emit_vargs_return_1, 1) // char*
        T_NUMBER(purple_signal_emit_vargs_return_1, 2) // va_list

        RETURN_NUMBER((long)purple_signal_emit_vargs_return_1((void *)(long)PARAM(0), (char*)PARAM(1), (va_list)PARAM(2)))
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signals_init, 0)

    purple_signals_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_signals_uninit, 0)

    purple_signals_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_type, 0)

    RETURN_NUMBER(purple_smiley_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_new, 2)
    T_NUMBER(purple_smiley_new, 0)     // PurpleStoredImage*
    T_STRING(purple_smiley_new, 1)     // char*

    RETURN_NUMBER((long)purple_smiley_new((PurpleStoredImage *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_new_from_file, 2)
    T_STRING(purple_smiley_new_from_file, 0)     // char*
    T_STRING(purple_smiley_new_from_file, 1)     // char*

    RETURN_NUMBER((long)purple_smiley_new_from_file((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_delete, 1)
    T_NUMBER(purple_smiley_delete, 0)     // PurpleSmiley*

    purple_smiley_delete((PurpleSmiley *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_set_shortcut, 2)
    T_NUMBER(purple_smiley_set_shortcut, 0)     // PurpleSmiley*
    T_STRING(purple_smiley_set_shortcut, 1)     // char*

    RETURN_NUMBER(purple_smiley_set_shortcut((PurpleSmiley *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_set_data, 3)
    T_NUMBER(purple_smiley_set_data, 0)     // PurpleSmiley*
    T_STRING(purple_smiley_set_data, 1)     // unsigned char*
    T_NUMBER(purple_smiley_set_data, 2)     // size_t

    purple_smiley_set_data((PurpleSmiley *)(long)PARAM(0), (unsigned char *)PARAM(1), (size_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_shortcut, 1)
    T_NUMBER(purple_smiley_get_shortcut, 0)     // PurpleSmiley*

    RETURN_STRING((char *)purple_smiley_get_shortcut((PurpleSmiley *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_checksum, 1)
    T_NUMBER(purple_smiley_get_checksum, 0)     // PurpleSmiley*

    RETURN_STRING((char *)purple_smiley_get_checksum((PurpleSmiley *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_stored_image, 1)
    T_NUMBER(purple_smiley_get_stored_image, 0)     // PurpleSmiley*

    RETURN_NUMBER((long)purple_smiley_get_stored_image((PurpleSmiley *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_data, 2)
    T_NUMBER(purple_smiley_get_data, 0)     // PurpleSmiley*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    RETURN_NUMBER((long)purple_smiley_get_data((PurpleSmiley *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_extension, 1)
    T_NUMBER(purple_smiley_get_extension, 0)     // PurpleSmiley*

    RETURN_STRING((char *)purple_smiley_get_extension((PurpleSmiley *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smiley_get_full_path, 1)
    T_NUMBER(purple_smiley_get_full_path, 0)     // PurpleSmiley*

    RETURN_STRING((char *)purple_smiley_get_full_path((PurpleSmiley *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_get_all, 0)

    RETURN_NUMBER((long)purple_smileys_get_all())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_find_by_shortcut, 1)
    T_STRING(purple_smileys_find_by_shortcut, 0)     // char*

    RETURN_NUMBER((long)purple_smileys_find_by_shortcut((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_find_by_checksum, 1)
    T_STRING(purple_smileys_find_by_checksum, 0)     // char*

    RETURN_NUMBER((long)purple_smileys_find_by_checksum((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_get_storing_dir, 0)

    RETURN_STRING((char *)purple_smileys_get_storing_dir())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_init, 0)

    purple_smileys_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_smileys_uninit, 0)

    purple_smileys_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_play_file, 2)
    T_STRING(purple_sound_play_file, 0)     // char*
    T_NUMBER(purple_sound_play_file, 1)     // PurpleAccount*

    purple_sound_play_file((char *)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_play_event, 2)
    T_NUMBER(purple_sound_play_event, 0)     // PurpleSoundEventID
    T_NUMBER(purple_sound_play_event, 1)     // PurpleAccount*

    purple_sound_play_event((PurpleSoundEventID)PARAM(0), (PurpleAccount *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_set_ui_ops, 1)
    T_NUMBER(purple_sound_set_ui_ops, 0)
    purple_sound_set_ui_ops((PurpleSoundUiOps *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_get_ui_ops, 0)

    RETURN_NUMBER((long)purple_sound_get_ui_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_init, 0)

    purple_sound_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_uninit, 0)

    purple_sound_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sounds_get_handle, 0)

    RETURN_NUMBER((long)purple_sounds_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_theme_get_type, 0)

    RETURN_NUMBER(purple_sound_theme_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_theme_get_file, 2)
    T_NUMBER(purple_sound_theme_get_file, 0)     // PurpleSoundTheme*
    T_STRING(purple_sound_theme_get_file, 1)     // char*

    RETURN_STRING((char *)purple_sound_theme_get_file((PurpleSoundTheme *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_theme_get_file_full, 2)
    T_NUMBER(purple_sound_theme_get_file_full, 0)     // PurpleSoundTheme*
    T_STRING(purple_sound_theme_get_file_full, 1)     // char*

    RETURN_STRING((char *)purple_sound_theme_get_file_full((PurpleSoundTheme *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_sound_theme_set_file, 3)
    T_NUMBER(purple_sound_theme_set_file, 0)     // PurpleSoundTheme*
    T_STRING(purple_sound_theme_set_file, 1)     // char*
    T_STRING(purple_sound_theme_set_file, 2)     // char*

    purple_sound_theme_set_file((PurpleSoundTheme *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_is_supported, 0)

    RETURN_NUMBER(purple_ssl_is_supported())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_strerror, 1)
    T_NUMBER(purple_ssl_strerror, 0)     // PurpleSslErrorType

    RETURN_STRING((char *)purple_ssl_strerror((PurpleSslErrorType)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*
   CONCEPT_FUNCTION_IMPL(purple_ssl_connect,6)
        T_NUMBER(purple_ssl_connect, 0) // PurpleAccount*
        T_STRING(purple_ssl_connect, 1) // char*
        T_NUMBER(purple_ssl_connect, 2) // int
        T_NUMBER(purple_ssl_connect, 3) // PurpleSslInputFunction
        T_NUMBER(purple_ssl_connect, 4) // PurpleSslErrorFunction
        T_NUMBER(purple_ssl_connect, 5) // void*

        RETURN_NUMBER((long)purple_ssl_connect((PurpleAccount*)(long)PARAM(0), (char*)PARAM(1), (int)PARAM(2), (PurpleSslInputFunction)PARAM(3), (PurpleSslErrorFunction)PARAM(4), (void *)(long)PARAM(5)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_ssl_connect_with_ssl_cn,7)
        T_NUMBER(purple_ssl_connect_with_ssl_cn, 0) // PurpleAccount*
        T_STRING(purple_ssl_connect_with_ssl_cn, 1) // char*
        T_NUMBER(purple_ssl_connect_with_ssl_cn, 2) // int
        T_NUMBER(purple_ssl_connect_with_ssl_cn, 3) // PurpleSslInputFunction
        T_NUMBER(purple_ssl_connect_with_ssl_cn, 4) // PurpleSslErrorFunction
        T_STRING(purple_ssl_connect_with_ssl_cn, 5) // char*
        T_NUMBER(purple_ssl_connect_with_ssl_cn, 6) // void*

        RETURN_NUMBER((long)purple_ssl_connect_with_ssl_cn((PurpleAccount*)(long)PARAM(0), (char*)PARAM(1), (int)PARAM(2), (PurpleSslInputFunction)PARAM(3), (PurpleSslErrorFunction)PARAM(4), (char*)PARAM(5), (void *)(long)PARAM(6)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_ssl_connect_with_host_fd,6)
        T_NUMBER(purple_ssl_connect_with_host_fd, 0) // PurpleAccount*
        T_NUMBER(purple_ssl_connect_with_host_fd, 1) // int
        T_NUMBER(purple_ssl_connect_with_host_fd, 2) // PurpleSslInputFunction
        T_NUMBER(purple_ssl_connect_with_host_fd, 3) // PurpleSslErrorFunction
        T_STRING(purple_ssl_connect_with_host_fd, 4) // char*
        T_NUMBER(purple_ssl_connect_with_host_fd, 5) // void*

        RETURN_NUMBER((long)purple_ssl_connect_with_host_fd((PurpleAccount*)(long)PARAM(0), (int)PARAM(1), (PurpleSslInputFunction)PARAM(2), (PurpleSslErrorFunction)PARAM(3), (char*)PARAM(4), (void *)(long)PARAM(5)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_ssl_input_add,3)
        T_NUMBER(purple_ssl_input_add, 0) // PurpleSslConnection*
        T_NUMBER(purple_ssl_input_add, 1) // PurpleSslInputFunction
        T_NUMBER(purple_ssl_input_add, 2) // void*

        purple_ssl_input_add((PurpleSslConnection*)(long)PARAM(0), (PurpleSslInputFunction)PARAM(1), (void *)(long)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_close, 1)
    T_NUMBER(purple_ssl_close, 0)     // PurpleSslConnection*

    purple_ssl_close((PurpleSslConnection *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_read, 3)
    T_NUMBER(purple_ssl_read, 0)     // PurpleSslConnection*
    T_NUMBER(purple_ssl_read, 1)     // void*
    T_NUMBER(purple_ssl_read, 2)     // size_t

    RETURN_NUMBER(purple_ssl_read((PurpleSslConnection *)(long)PARAM(0), (void *)(long)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_write, 3)
    T_NUMBER(purple_ssl_write, 0)     // PurpleSslConnection*
    T_NUMBER(purple_ssl_write, 1)     // void*
    T_NUMBER(purple_ssl_write, 2)     // size_t

    RETURN_NUMBER(purple_ssl_write((PurpleSslConnection *)(long)PARAM(0), (void *)(long)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_get_peer_certificates, 1)
    T_NUMBER(purple_ssl_get_peer_certificates, 0)     // PurpleSslConnection*

    RETURN_NUMBER((long)purple_ssl_get_peer_certificates((PurpleSslConnection *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_set_ops, 1)
    T_NUMBER(purple_ssl_set_ops, 0)     // PurpleSslOps*

    purple_ssl_set_ops((PurpleSslOps *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_get_ops, 0)

    RETURN_NUMBER((long)purple_ssl_get_ops())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_init, 0)

    purple_ssl_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ssl_uninit, 0)

    purple_ssl_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_primitive_get_id_from_type, 1)
    T_NUMBER(purple_primitive_get_id_from_type, 0)     // PurpleStatusPrimitive

    RETURN_STRING((char *)purple_primitive_get_id_from_type((PurpleStatusPrimitive)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_primitive_get_name_from_type, 1)
    T_NUMBER(purple_primitive_get_name_from_type, 0)     // PurpleStatusPrimitive

    RETURN_STRING((char *)purple_primitive_get_name_from_type((PurpleStatusPrimitive)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_primitive_get_type_from_id, 1)
    T_STRING(purple_primitive_get_type_from_id, 0)     // char*

    RETURN_NUMBER(purple_primitive_get_type_from_id((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_new_full, 6)
    T_NUMBER(purple_status_type_new_full, 0)     // PurpleStatusPrimitive
    T_STRING(purple_status_type_new_full, 1)     // char*
    T_STRING(purple_status_type_new_full, 2)     // char*
    T_NUMBER(purple_status_type_new_full, 3)     // int
    T_NUMBER(purple_status_type_new_full, 4)     // int
    T_NUMBER(purple_status_type_new_full, 5)     // int

    RETURN_NUMBER((long)purple_status_type_new_full((PurpleStatusPrimitive)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_new, 4)
    T_NUMBER(purple_status_type_new, 0)     // PurpleStatusPrimitive
    T_STRING(purple_status_type_new, 1)     // char*
    T_STRING(purple_status_type_new, 2)     // char*
    T_NUMBER(purple_status_type_new, 3)     // int

    RETURN_NUMBER((long)purple_status_type_new((PurpleStatusPrimitive)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_new_with_attrs, 9)
    T_NUMBER(purple_status_type_new_with_attrs, 0)     // PurpleStatusPrimitive
    T_STRING(purple_status_type_new_with_attrs, 1)     // char*
    T_STRING(purple_status_type_new_with_attrs, 2)     // char*
    T_NUMBER(purple_status_type_new_with_attrs, 3)     // int
    T_NUMBER(purple_status_type_new_with_attrs, 4)     // int
    T_NUMBER(purple_status_type_new_with_attrs, 5)     // int
    T_STRING(purple_status_type_new_with_attrs, 6)     // char*
    T_STRING(purple_status_type_new_with_attrs, 7)     // char*
    T_NUMBER(purple_status_type_new_with_attrs, 8)     // PurpleValue*

    RETURN_NUMBER((long)purple_status_type_new_with_attrs((PurpleStatusPrimitive)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (char *)PARAM(6), (char *)PARAM(7), (PurpleValue *)(long)PARAM(8)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_destroy, 1)
    T_NUMBER(purple_status_type_destroy, 0)     // PurpleStatusType*

    purple_status_type_destroy((PurpleStatusType *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_get_primitive, 1)
    T_NUMBER(purple_status_type_get_primitive, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_get_primitive((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_get_id, 1)
    T_NUMBER(purple_status_type_get_id, 0)     // PurpleStatusType*

    RETURN_STRING((char *)purple_status_type_get_id((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_get_name, 1)
    T_NUMBER(purple_status_type_get_name, 0)     // PurpleStatusType*

    RETURN_STRING((char *)purple_status_type_get_name((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_is_saveable, 1)
    T_NUMBER(purple_status_type_is_saveable, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_is_saveable((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_is_user_settable, 1)
    T_NUMBER(purple_status_type_is_user_settable, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_is_user_settable((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_is_independent, 1)
    T_NUMBER(purple_status_type_is_independent, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_is_independent((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_is_exclusive, 1)
    T_NUMBER(purple_status_type_is_exclusive, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_is_exclusive((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_is_available, 1)
    T_NUMBER(purple_status_type_is_available, 0)     // PurpleStatusType*

    RETURN_NUMBER(purple_status_type_is_available((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_get_attr, 2)
    T_NUMBER(purple_status_type_get_attr, 0)     // PurpleStatusType*
    T_STRING(purple_status_type_get_attr, 1)     // char*

    RETURN_NUMBER((long)purple_status_type_get_attr((PurpleStatusType *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_get_attrs, 1)
    T_NUMBER(purple_status_type_get_attrs, 0)     // PurpleStatusType*

    RETURN_NUMBER((long)purple_status_type_get_attrs((PurpleStatusType *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_type_find_with_id, 2)
    T_NUMBER(purple_status_type_find_with_id, 0)     // GList*
    T_STRING(purple_status_type_find_with_id, 1)     // char*

    RETURN_NUMBER((long)purple_status_type_find_with_id((GList *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_attr_new, 3)
    T_STRING(purple_status_attr_new, 0)     // char*
    T_STRING(purple_status_attr_new, 1)     // char*
    T_NUMBER(purple_status_attr_new, 2)     // PurpleValue*

    RETURN_NUMBER((long)purple_status_attr_new((char *)PARAM(0), (char *)PARAM(1), (PurpleValue *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_attr_destroy, 1)
    T_NUMBER(purple_status_attr_destroy, 0)     // PurpleStatusAttr*

    purple_status_attr_destroy((PurpleStatusAttr *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_attr_get_id, 1)
    T_NUMBER(purple_status_attr_get_id, 0)     // PurpleStatusAttr*

    RETURN_STRING((char *)purple_status_attr_get_id((PurpleStatusAttr *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_attr_get_name, 1)
    T_NUMBER(purple_status_attr_get_name, 0)     // PurpleStatusAttr*

    RETURN_STRING((char *)purple_status_attr_get_name((PurpleStatusAttr *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_attr_get_value, 1)
    T_NUMBER(purple_status_attr_get_value, 0)     // PurpleStatusAttr*

    RETURN_NUMBER((long)purple_status_attr_get_value((PurpleStatusAttr *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_new, 2)
    T_NUMBER(purple_status_new, 0)     // PurpleStatusType*
    T_NUMBER(purple_status_new, 1)     // PurplePresence*

    RETURN_NUMBER((long)purple_status_new((PurpleStatusType *)(long)PARAM(0), (PurplePresence *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_destroy, 1)
    T_NUMBER(purple_status_destroy, 0)     // PurpleStatus*

    purple_status_destroy((PurpleStatus *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_set_active, 2)
    T_NUMBER(purple_status_set_active, 0)     // PurpleStatus*
    T_NUMBER(purple_status_set_active, 1)     // int

    purple_status_set_active((PurpleStatus *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_status_set_active_with_attrs,3)
        T_NUMBER(purple_status_set_active, 0) // PurpleStatus*
        T_NUMBER(purple_status_set_active, 1) // int
        T_NUMBER(purple_status_set_active, 2) // va_list

        purple_status_set_active_with_attrs((PurpleStatus*)(long)PARAM(0), (int)PARAM(1), (va_list)PARAM(2));
        RETURN_NUMBER(0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_set_active_with_attrs_list, 3)
    T_NUMBER(purple_status_set_active_with_attrs_list, 0)     // PurpleStatus*
    T_NUMBER(purple_status_set_active_with_attrs_list, 1)     // int
    T_NUMBER(purple_status_set_active_with_attrs_list, 2)     // GList*

    purple_status_set_active_with_attrs_list((PurpleStatus *)(long)PARAM(0), (int)PARAM(1), (GList *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_type, 1)
    T_NUMBER(purple_status_get_type, 0)     // PurpleStatus*

    RETURN_NUMBER((long)purple_status_get_type((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_presence, 1)
    T_NUMBER(purple_status_get_presence, 0)     // PurpleStatus*

    RETURN_NUMBER((long)purple_status_get_presence((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_id, 1)
    T_NUMBER(purple_status_get_id, 0)     // PurpleStatus*

    RETURN_STRING((char *)purple_status_get_id((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_name, 1)
    T_NUMBER(purple_status_get_name, 0)     // PurpleStatus*

    RETURN_STRING((char *)purple_status_get_name((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_is_independent, 1)
    T_NUMBER(purple_status_is_independent, 0)     // PurpleStatus*

    RETURN_NUMBER(purple_status_is_independent((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_is_exclusive, 1)
    T_NUMBER(purple_status_is_exclusive, 0)     // PurpleStatus*

    RETURN_NUMBER(purple_status_is_exclusive((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_is_available, 1)
    T_NUMBER(purple_status_is_available, 0)     // PurpleStatus*

    RETURN_NUMBER(purple_status_is_available((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_is_active, 1)
    T_NUMBER(purple_status_is_active, 0)     // PurpleStatus*

    RETURN_NUMBER(purple_status_is_active((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_is_online, 1)
    T_NUMBER(purple_status_is_online, 0)     // PurpleStatus*

    RETURN_NUMBER(purple_status_is_online((PurpleStatus *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_attr_value, 2)
    T_NUMBER(purple_status_get_attr_value, 0)     // PurpleStatus*
    T_STRING(purple_status_get_attr_value, 1)     // char*

    RETURN_NUMBER((long)purple_status_get_attr_value((PurpleStatus *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_attr_boolean, 2)
    T_NUMBER(purple_status_get_attr_boolean, 0)     // PurpleStatus*
    T_STRING(purple_status_get_attr_boolean, 1)     // char*

    RETURN_NUMBER(purple_status_get_attr_boolean((PurpleStatus *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_attr_int, 2)
    T_NUMBER(purple_status_get_attr_int, 0)     // PurpleStatus*
    T_STRING(purple_status_get_attr_int, 1)     // char*

    RETURN_NUMBER(purple_status_get_attr_int((PurpleStatus *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_attr_string, 2)
    T_NUMBER(purple_status_get_attr_string, 0)     // PurpleStatus*
    T_STRING(purple_status_get_attr_string, 1)     // char*

    RETURN_STRING((char *)purple_status_get_attr_string((PurpleStatus *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_compare, 2)
    T_NUMBER(purple_status_compare, 0)     // PurpleStatus*
    T_NUMBER(purple_status_compare, 1)     // PurpleStatus*

    RETURN_NUMBER(purple_status_compare((PurpleStatus *)(long)PARAM(0), (PurpleStatus *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_new, 1)
    T_NUMBER(purple_presence_new, 0)     // PurplePresenceContext

    RETURN_NUMBER((long)purple_presence_new((PurplePresenceContext)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_new_for_account, 1)
    T_NUMBER(purple_presence_new_for_account, 0)     // PurpleAccount*

    RETURN_NUMBER((long)purple_presence_new_for_account((PurpleAccount *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_new_for_conv, 1)
    T_NUMBER(purple_presence_new_for_conv, 0)     // PurpleConversation*

    RETURN_NUMBER((long)purple_presence_new_for_conv((PurpleConversation *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_new_for_buddy, 1)
    T_NUMBER(purple_presence_new_for_buddy, 0)     // PurpleBuddy*

    RETURN_NUMBER((long)purple_presence_new_for_buddy((PurpleBuddy *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_destroy, 1)
    T_NUMBER(purple_presence_destroy, 0)     // PurplePresence*

    purple_presence_destroy((PurplePresence *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_set_status_active, 3)
    T_NUMBER(purple_presence_set_status_active, 0)     // PurplePresence*
    T_STRING(purple_presence_set_status_active, 1)     // char*
    T_NUMBER(purple_presence_set_status_active, 2)     // int

    purple_presence_set_status_active((PurplePresence *)(long)PARAM(0), (char *)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_switch_status, 2)
    T_NUMBER(purple_presence_switch_status, 0)     // PurplePresence*
    T_STRING(purple_presence_switch_status, 1)     // char*

    purple_presence_switch_status((PurplePresence *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_set_idle, 3)
    T_NUMBER(purple_presence_set_idle, 0)     // PurplePresence*
    T_NUMBER(purple_presence_set_idle, 1)     // int
    T_NUMBER(purple_presence_set_idle, 2)     // time_t

    purple_presence_set_idle((PurplePresence *)(long)PARAM(0), (int)PARAM(1), (time_t)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_set_login_time, 2)
    T_NUMBER(purple_presence_set_login_time, 0)     // PurplePresence*
    T_NUMBER(purple_presence_set_login_time, 1)     // time_t

    purple_presence_set_login_time((PurplePresence *)(long)PARAM(0), (time_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_context, 1)
    T_NUMBER(purple_presence_get_context, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_get_context((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_account, 1)
    T_NUMBER(purple_presence_get_account, 0)     // PurplePresence*

    RETURN_NUMBER((long)purple_presence_get_account((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_conversation, 1)
    T_NUMBER(purple_presence_get_conversation, 0)     // PurplePresence*

    RETURN_NUMBER((long)purple_presence_get_conversation((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_chat_user, 1)
    T_NUMBER(purple_presence_get_chat_user, 0)     // PurplePresence*

    RETURN_STRING((char *)purple_presence_get_chat_user((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_buddy, 1)
    T_NUMBER(purple_presence_get_buddy, 0)     // PurplePresence*

    RETURN_NUMBER((long)purple_presence_get_buddy((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_statuses, 1)
    T_NUMBER(purple_presence_get_statuses, 0)     // PurplePresence*

    RETURN_NUMBER((long)purple_presence_get_statuses((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_status, 2)
    T_NUMBER(purple_presence_get_status, 0)     // PurplePresence*
    T_STRING(purple_presence_get_status, 1)     // char*

    RETURN_NUMBER((long)purple_presence_get_status((PurplePresence *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_active_status, 1)
    T_NUMBER(purple_presence_get_active_status, 0)     // PurplePresence*

    RETURN_NUMBER((long)purple_presence_get_active_status((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_is_available, 1)
    T_NUMBER(purple_presence_is_available, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_is_available((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_is_online, 1)
    T_NUMBER(purple_presence_is_online, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_is_online((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_is_status_active, 2)
    T_NUMBER(purple_presence_is_status_active, 0)     // PurplePresence*
    T_STRING(purple_presence_is_status_active, 1)     // char*

    RETURN_NUMBER(purple_presence_is_status_active((PurplePresence *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_is_status_primitive_active, 2)
    T_NUMBER(purple_presence_is_status_primitive_active, 0)     // PurplePresence*
    T_NUMBER(purple_presence_is_status_primitive_active, 1)     // PurpleStatusPrimitive

    RETURN_NUMBER(purple_presence_is_status_primitive_active((PurplePresence *)(long)PARAM(0), (PurpleStatusPrimitive)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_is_idle, 1)
    T_NUMBER(purple_presence_is_idle, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_is_idle((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_idle_time, 1)
    T_NUMBER(purple_presence_get_idle_time, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_get_idle_time((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_get_login_time, 1)
    T_NUMBER(purple_presence_get_login_time, 0)     // PurplePresence*

    RETURN_NUMBER(purple_presence_get_login_time((PurplePresence *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_presence_compare, 2)
    T_NUMBER(purple_presence_compare, 0)     // PurplePresence*
    T_NUMBER(purple_presence_compare, 1)     // PurplePresence*

    RETURN_NUMBER(purple_presence_compare((PurplePresence *)(long)PARAM(0), (PurplePresence *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_get_handle, 0)

    RETURN_NUMBER((long)purple_status_get_handle())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_init, 0)

    purple_status_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_status_uninit, 0)

    purple_status_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_type, 0)

    RETURN_NUMBER(purple_theme_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_name, 1)
    T_NUMBER(purple_theme_get_name, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_name((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_set_name, 2)
    T_NUMBER(purple_theme_set_name, 0)     // PurpleTheme*
    T_STRING(purple_theme_set_name, 1)     // char*

    purple_theme_set_name((PurpleTheme *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_description, 1)
    T_NUMBER(purple_theme_get_description, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_description((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_set_description, 2)
    T_NUMBER(purple_theme_set_description, 0)     // PurpleTheme*
    T_STRING(purple_theme_set_description, 1)     // char*

    purple_theme_set_description((PurpleTheme *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_author, 1)
    T_NUMBER(purple_theme_get_author, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_author((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_set_author, 2)
    T_NUMBER(purple_theme_set_author, 0)     // PurpleTheme*
    T_STRING(purple_theme_set_author, 1)     // char*

    purple_theme_set_author((PurpleTheme *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_type_string, 1)
    T_NUMBER(purple_theme_get_type_string, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_type_string((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_dir, 1)
    T_NUMBER(purple_theme_get_dir, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_dir((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_set_dir, 2)
    T_NUMBER(purple_theme_set_dir, 0)     // PurpleTheme*
    T_STRING(purple_theme_set_dir, 1)     // char*

    purple_theme_set_dir((PurpleTheme *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_image, 1)
    T_NUMBER(purple_theme_get_image, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_image((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_get_image_full, 1)
    T_NUMBER(purple_theme_get_image_full, 0)     // PurpleTheme*

    RETURN_STRING((char *)purple_theme_get_image_full((PurpleTheme *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_set_image, 2)
    T_NUMBER(purple_theme_set_image, 0)     // PurpleTheme*
    T_STRING(purple_theme_set_image, 1)     // char*

    purple_theme_set_image((PurpleTheme *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_loader_get_type, 0)

    RETURN_NUMBER(purple_theme_loader_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_loader_get_type_string, 1)
    T_NUMBER(purple_theme_loader_get_type_string, 0)     // PurpleThemeLoader*

    RETURN_STRING((char *)purple_theme_loader_get_type_string((PurpleThemeLoader *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_loader_build, 2)
    T_NUMBER(purple_theme_loader_build, 0)     // PurpleThemeLoader*
    T_STRING(purple_theme_loader_build, 1)     // char*

    RETURN_NUMBER((long)purple_theme_loader_build((PurpleThemeLoader *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_get_type, 0)

    RETURN_NUMBER(purple_theme_manager_get_type())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_init, 0)

    purple_theme_manager_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_uninit, 0)

    purple_theme_manager_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_refresh, 0)

    purple_theme_manager_refresh();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_find_theme, 2)
    T_STRING(purple_theme_manager_find_theme, 0)     // char*
    T_STRING(purple_theme_manager_find_theme, 1)     // char*

    RETURN_NUMBER((long)purple_theme_manager_find_theme((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_add_theme, 1)
    T_NUMBER(purple_theme_manager_add_theme, 0)     // PurpleTheme*

    purple_theme_manager_add_theme((PurpleTheme *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_remove_theme, 1)
    T_NUMBER(purple_theme_manager_remove_theme, 0)     // PurpleTheme*

    purple_theme_manager_remove_theme((PurpleTheme *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_register_type, 1)
    T_NUMBER(purple_theme_manager_register_type, 0)     // PurpleThemeLoader*

    purple_theme_manager_register_type((PurpleThemeLoader *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_unregister_type, 1)
    T_NUMBER(purple_theme_manager_unregister_type, 0)     // PurpleThemeLoader*

    purple_theme_manager_unregister_type((PurpleThemeLoader *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_theme_manager_for_each_theme,1)
        T_NUMBER(purple_theme_manager_unregister_type, 0) // PTFunc

        purple_theme_manager_for_each_theme((PTFunc)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_theme_manager_load_theme, 2)
    T_STRING(purple_theme_manager_load_theme, 0)     // char*
    T_STRING(purple_theme_manager_load_theme, 1)     // char*

    RETURN_NUMBER((long)purple_theme_manager_load_theme((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_upnp_init,0)

        purple_upnp_init();
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_upnp_discover,2)
        T_NUMBER(purple_upnp_discover, 0) // PurpleUPnPCallback
        T_NUMBER(purple_upnp_discover, 1) // void*

        purple_upnp_discover((PurpleUPnPCallback)PARAM(0), (void *)(long)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_upnp_get_public_ip,0)

        RETURN_STRING((char*)purple_upnp_get_public_ip())
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_upnp_cancel_port_mapping,1)
        T_NUMBER(purple_upnp_cancel_port_mapping, 0) // UPnPMappingAddRemove*

        purple_upnp_cancel_port_mapping((UPnPMappingAddRemove*)(long)PARAM(0));
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_upnp_set_port_mapping,4)
        T_NUMBER(purple_upnp_set_port_mapping, 0) // unsigned short
        T_STRING(purple_upnp_set_port_mapping, 1) // char*
        T_NUMBER(purple_upnp_set_port_mapping, 2) // PurpleUPnPCallback
        T_NUMBER(purple_upnp_set_port_mapping, 3) // void*

        RETURN_NUMBER((long)purple_upnp_set_port_mapping((unsigned short)PARAM(0), (char*)PARAM(1), (PurpleUPnPCallback)PARAM(2), (void *)(long)PARAM(3)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_upnp_remove_port_mapping,4)
        T_NUMBER(purple_upnp_remove_port_mapping, 0) // unsigned short
        T_STRING(purple_upnp_remove_port_mapping, 1) // char*
        T_NUMBER(purple_upnp_remove_port_mapping, 2) // PurpleUPnPCallback
        T_NUMBER(purple_upnp_remove_port_mapping, 3) // void*

        RETURN_NUMBER((long)purple_upnp_remove_port_mapping((unsigned short)PARAM(0), (char*)PARAM(1), (PurpleUPnPCallback)PARAM(2), (void *)(long)PARAM(3)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_menu_action_new,4)
        T_STRING(purple_menu_action_new, 0) // char*
        T_NUMBER(purple_menu_action_new, 1) // PurpleCallback
        T_NUMBER(purple_menu_action_new, 2) // void*
        T_NUMBER(purple_menu_action_new, 3) // GList*

        RETURN_NUMBER((long)purple_menu_action_new((char*)PARAM(0), (PurpleCallback)PARAM(1), (void *)(long)PARAM(2), (GList*)(long)PARAM(3)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_menu_action_free,1)

        // ... parameter 0 is by reference (PurpleMenuAction*)
        PurpleMenuAction local_parameter_0;

        purple_menu_action_free(&local_parameter_0);
        SET_NUMBER(0, (long)local_parameter_0)
        RETURN_NUMBER(0)
   END_IMPL
   //------------------------------------------------------------------------
 */
CONCEPT_FUNCTION_IMPL(purple_util_set_current_song, 3)
    T_STRING(purple_util_set_current_song, 0)     // char*
    T_STRING(purple_util_set_current_song, 1)     // char*
    T_STRING(purple_util_set_current_song, 2)     // char*

    purple_util_set_current_song((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_format_song_info, 4)
    T_STRING(purple_util_format_song_info, 0)     // char*
    T_STRING(purple_util_format_song_info, 1)     // char*
    T_STRING(purple_util_format_song_info, 2)     // char*
    T_NUMBER(purple_util_format_song_info, 3)     // void*

    RETURN_STRING((char *)purple_util_format_song_info((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2), (void *)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_init, 0)

    purple_util_init();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_uninit, 0)

    purple_util_uninit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_base16_encode, 2)
    T_STRING(purple_base16_encode, 0)     // unsigned char*
    T_NUMBER(purple_base16_encode, 1)     // gsize

    RETURN_STRING((char *)purple_base16_encode((unsigned char *)PARAM(0), (gsize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_base16_decode, 2)
    T_STRING(purple_base16_decode, 0)     // char*

// ... parameter 1 is by reference (gsize*)
    gsize local_parameter_1;

    RETURN_STRING((char *)purple_base16_decode((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_base16_encode_chunked, 2)
    T_STRING(purple_base16_encode_chunked, 0)     // unsigned char*
    T_NUMBER(purple_base16_encode_chunked, 1)     // gsize

    RETURN_STRING((char *)purple_base16_encode_chunked((unsigned char *)PARAM(0), (gsize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_base64_encode, 2)
    T_STRING(purple_base64_encode, 0)     // unsigned char*
    T_NUMBER(purple_base64_encode, 1)     // gsize

    RETURN_STRING((char *)purple_base64_encode((unsigned char *)PARAM(0), (gsize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_base64_decode, 2)
    T_STRING(purple_base64_decode, 0)     // char*

// ... parameter 1 is by reference (gsize*)
    gsize local_parameter_1;

    RETURN_STRING((char *)purple_base64_decode((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_quotedp_decode, 2)
    T_STRING(purple_quotedp_decode, 0)     // char*

// ... parameter 1 is by reference (gsize*)
    gsize local_parameter_1;

    RETURN_STRING((char *)purple_quotedp_decode((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mime_decode_field, 1)
    T_STRING(purple_mime_decode_field, 0)     // char*

    RETURN_STRING((char *)purple_mime_decode_field((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_utf8_strftime,2)
        T_STRING(purple_mime_decode_field, 0) // char*

        // ... parameter 1 is by reference (tm*)
        tm local_parameter_1;

        RETURN_STRING((char*)purple_utf8_strftime((char*)PARAM(0), &local_parameter_1))
        SET_NUMBER(1, (long)local_parameter_1)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_get_tzoff_str,2)
        T_NUMBER(purple_get_tzoff_str, 1) // int

        // ... parameter 0 is by reference (tm*)
        tm local_parameter_0;

        RETURN_STRING((char*)purple_get_tzoff_str(&local_parameter_0, (int)PARAM(1)))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_date_format_short,1)

        // ... parameter 0 is by reference (tm*)
        tm local_parameter_0;

        RETURN_STRING((char*)purple_date_format_short(&local_parameter_0))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_date_format_long,1)

        // ... parameter 0 is by reference (tm*)
        tm local_parameter_0;

        RETURN_STRING((char*)purple_date_format_long(&local_parameter_0))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_date_format_full,1)

        // ... parameter 0 is by reference (tm*)
        tm local_parameter_0;

        RETURN_STRING((char*)purple_date_format_full(&local_parameter_0))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_time_format,1)

        // ... parameter 0 is by reference (tm*)
        tm local_parameter_0;

        RETURN_STRING((char*)purple_time_format(&local_parameter_0))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_time_build, 6)
    T_NUMBER(purple_time_build, 0)     // int
    T_NUMBER(purple_time_build, 1)     // int
    T_NUMBER(purple_time_build, 2)     // int
    T_NUMBER(purple_time_build, 3)     // int
    T_NUMBER(purple_time_build, 4)     // int
    T_NUMBER(purple_time_build, 5)     // int

    RETURN_NUMBER(purple_time_build((int)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_str_to_time,5)
        T_STRING(purple_time_build, 0) // char*
        T_NUMBER(purple_time_build, 1) // int

        // ... parameter 2 is by reference (tm*)
        tm local_parameter_2;
        // ... parameter 3 is by reference (long*)
        long local_parameter_3;
        // ... parameter 4 is by reference (char**)
        char* local_parameter_4;

        RETURN_NUMBER(purple_str_to_time((char*)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_escape_text, 2)
    T_STRING(purple_markup_escape_text, 0)     // char*
    T_NUMBER(purple_markup_escape_text, 1)     // gssize

    RETURN_STRING((char *)purple_markup_escape_text((char *)PARAM(0), (gssize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(purple_markup_find_tag,5)
        T_STRING(purple_markup_escape_text, 0) // char*
        T_STRING(purple_markup_escape_text, 1) // char*

        // ... parameter 2 is by reference (char**)
        char* local_parameter_2;
        // ... parameter 3 is by reference (char**)
        char* local_parameter_3;
        // ... parameter 4 is by reference (GData**)
        GData* local_parameter_4;

        RETURN_NUMBER(purple_markup_find_tag((char*)PARAM(0), (char*)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_markup_extract_info_field,12)
        T_STRING(purple_markup_extract_info_field, 0) // char*
        T_NUMBER(purple_markup_extract_info_field, 1) // int
        T_NUMBER(purple_markup_extract_info_field, 2) // PurpleNotifyUserInfo*
        T_STRING(purple_markup_extract_info_field, 3) // char*
        T_NUMBER(purple_markup_extract_info_field, 4) // int
        T_STRING(purple_markup_extract_info_field, 5) // char*
        T_NUMBER(purple_markup_extract_info_field, 6) // char
        T_STRING(purple_markup_extract_info_field, 7) // char*
        T_STRING(purple_markup_extract_info_field, 8) // char*
        T_NUMBER(purple_markup_extract_info_field, 9) // int
        T_STRING(purple_markup_extract_info_field, 10) // char*
        T_NUMBER(purple_markup_extract_info_field, 11) // PurpleInfoFieldFormatCallback

        RETURN_NUMBER(purple_markup_extract_info_field((char*)PARAM(0), (int)PARAM(1), (PurpleNotifyUserInfo*)(long)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (char*)PARAM(5), (char)PARAM(6), (char*)PARAM(7), (char*)PARAM(8), (int)PARAM(9), (char*)PARAM(10), (PurpleInfoFieldFormatCallback)PARAM(11)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_markup_html_to_xhtml,3)
        T_STRING(purple_markup_html_to_xhtml, 0) // char*

        // ... parameter 1 is by reference (char**)
        char* local_parameter_1;
        // ... parameter 2 is by reference (char**)
        char* local_parameter_2;

        purple_markup_html_to_xhtml((char*)PARAM(0), &local_parameter_1, &local_parameter_2);
        SET_NUMBER(1, (long)local_parameter_1)
        SET_NUMBER(2, (long)local_parameter_2)
        RETURN_NUMBER(0)
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_strip_html, 1)
    T_STRING(purple_markup_strip_html, 0)     // char*

    RETURN_STRING((char *)purple_markup_strip_html((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_linkify, 1)
    T_STRING(purple_markup_linkify, 0)     // char*

    RETURN_STRING((char *)purple_markup_linkify((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_unescape_html, 1)
    T_STRING(purple_unescape_html, 0)     // char*

    RETURN_STRING((char *)purple_unescape_html((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_slice, 3)
    T_STRING(purple_markup_slice, 0)     // char*
    T_NUMBER(purple_markup_slice, 1)     // guint
    T_NUMBER(purple_markup_slice, 2)     // guint

    RETURN_STRING((char *)purple_markup_slice((char *)PARAM(0), (guint)PARAM(1), (guint)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_get_tag_name, 1)
    T_STRING(purple_markup_get_tag_name, 0)     // char*

    RETURN_STRING((char *)purple_markup_get_tag_name((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_unescape_entity, 2)
    T_STRING(purple_markup_unescape_entity, 0)     // char*

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_STRING((char *)purple_markup_unescape_entity((char *)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_get_css_property, 2)
    T_STRING(purple_markup_get_css_property, 0)     // char*
    T_STRING(purple_markup_get_css_property, 1)     // char*

    RETURN_STRING((char *)purple_markup_get_css_property((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_markup_is_rtl, 1)
    T_STRING(purple_markup_is_rtl, 0)     // char*

    RETURN_NUMBER(purple_markup_is_rtl((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_home_dir, 0)

    RETURN_STRING((char *)purple_home_dir())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_user_dir, 0)

    RETURN_STRING((char *)purple_user_dir())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_set_user_dir, 1)
    T_STRING(purple_util_set_user_dir, 0)     // char*

    purple_util_set_user_dir((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_build_dir, 2)
    T_STRING(purple_build_dir, 0)     // char*
    T_NUMBER(purple_build_dir, 1)     // int

    RETURN_NUMBER(purple_build_dir((char *)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_write_data_to_file, 3)
    T_STRING(purple_util_write_data_to_file, 0)     // char*
    T_STRING(purple_util_write_data_to_file, 1)     // char*
    T_NUMBER(purple_util_write_data_to_file, 2)     // gssize

    RETURN_NUMBER(purple_util_write_data_to_file((char *)PARAM(0), (char *)PARAM(1), (gssize)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_write_data_to_file_absolute, 3)
    T_STRING(purple_util_write_data_to_file_absolute, 0)     // char*
    T_STRING(purple_util_write_data_to_file_absolute, 1)     // char*
    T_NUMBER(purple_util_write_data_to_file_absolute, 2)     // gssize

    RETURN_NUMBER(purple_util_write_data_to_file_absolute((char *)PARAM(0), (char *)PARAM(1), (gssize)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_read_xml_from_file, 2)
    T_STRING(purple_util_read_xml_from_file, 0)     // char*
    T_STRING(purple_util_read_xml_from_file, 1)     // char*

    RETURN_NUMBER((long)purple_util_read_xml_from_file((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_mkstemp, 2)
    T_NUMBER(purple_mkstemp, 1)     // int

// ... parameter 0 is by reference (char**)
    char *local_parameter_0;

    RETURN_NUMBER((long)purple_mkstemp(&local_parameter_0, (int)PARAM(1)))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_get_image_extension, 2)
    T_NUMBER(purple_util_get_image_extension, 0)     // void*
    T_NUMBER(purple_util_get_image_extension, 1)     // size_t

    RETURN_STRING((char *)purple_util_get_image_extension((void *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_get_image_checksum, 2)
    T_NUMBER(purple_util_get_image_checksum, 0)     // void*
    T_NUMBER(purple_util_get_image_checksum, 1)     // size_t

    RETURN_STRING((char *)purple_util_get_image_checksum((void *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_get_image_filename, 2)
    T_NUMBER(purple_util_get_image_filename, 0)     // void*
    T_NUMBER(purple_util_get_image_filename, 1)     // size_t

    RETURN_STRING((char *)purple_util_get_image_filename((void *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_program_is_valid, 1)
    T_STRING(purple_program_is_valid, 0)     // char*

    RETURN_NUMBER(purple_program_is_valid((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_running_gnome, 0)

    RETURN_NUMBER(purple_running_gnome())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_running_kde, 0)

    RETURN_NUMBER(purple_running_kde())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_running_osx, 0)

    RETURN_NUMBER(purple_running_osx())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_fd_get_ip, 1)
    T_NUMBER(purple_fd_get_ip, 0)     // int

    RETURN_STRING((char *)purple_fd_get_ip((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_strequal, 2)
    T_STRING(purple_strequal, 0)     // char*
    T_STRING(purple_strequal, 1)     // char*

    RETURN_NUMBER(purple_strequal((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_normalize, 2)
    T_NUMBER(purple_normalize, 0)     // PurpleAccount*
    T_STRING(purple_normalize, 1)     // char*

    RETURN_STRING((char *)purple_normalize((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_normalize_nocase, 2)
    T_NUMBER(purple_normalize_nocase, 0)     // PurpleAccount*
    T_STRING(purple_normalize_nocase, 1)     // char*

    RETURN_STRING((char *)purple_normalize_nocase((PurpleAccount *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_has_prefix, 2)
    T_STRING(purple_str_has_prefix, 0)     // char*
    T_STRING(purple_str_has_prefix, 1)     // char*

    RETURN_NUMBER(purple_str_has_prefix((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_has_suffix, 2)
    T_STRING(purple_str_has_suffix, 0)     // char*
    T_STRING(purple_str_has_suffix, 1)     // char*

    RETURN_NUMBER(purple_str_has_suffix((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_strdup_withhtml, 1)
    T_STRING(purple_strdup_withhtml, 0)     // char*

    RETURN_STRING((char *)purple_strdup_withhtml((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_add_cr, 1)
    T_STRING(purple_str_add_cr, 0)     // char*

    RETURN_STRING((char *)purple_str_add_cr((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_strip_char, 2)
    T_STRING(purple_str_strip_char, 0)     // char*
    T_NUMBER(purple_str_strip_char, 1)     // char

    purple_str_strip_char((char *)PARAM(0), (char)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_chrreplace, 3)
    T_STRING(purple_util_chrreplace, 0)     // char*
    T_NUMBER(purple_util_chrreplace, 1)     // char
    T_NUMBER(purple_util_chrreplace, 2)     // char

    purple_util_chrreplace((char *)PARAM(0), (char)PARAM(1), (char)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_strreplace, 3)
    T_STRING(purple_strreplace, 0)     // char*
    T_STRING(purple_strreplace, 1)     // char*
    T_STRING(purple_strreplace, 2)     // char*

    RETURN_STRING((char *)purple_strreplace((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_ncr_encode, 1)
    T_STRING(purple_utf8_ncr_encode, 0)     // char*

    RETURN_STRING((char *)purple_utf8_ncr_encode((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_ncr_decode, 1)
    T_STRING(purple_utf8_ncr_decode, 0)     // char*

    RETURN_STRING((char *)purple_utf8_ncr_decode((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_strcasereplace, 3)
    T_STRING(purple_strcasereplace, 0)     // char*
    T_STRING(purple_strcasereplace, 1)     // char*
    T_STRING(purple_strcasereplace, 2)     // char*

    RETURN_STRING((char *)purple_strcasereplace((char *)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_strcasestr, 2)
    T_STRING(purple_strcasestr, 0)     // char*
    T_STRING(purple_strcasestr, 1)     // char*

    RETURN_STRING((char *)purple_strcasestr((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_size_to_units, 1)
    T_NUMBER(purple_str_size_to_units, 0)     // size_t

    RETURN_STRING((char *)purple_str_size_to_units((size_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_seconds_to_string, 1)
    T_NUMBER(purple_str_seconds_to_string, 0)     // guint

    RETURN_STRING((char *)purple_str_seconds_to_string((guint)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_str_binary_to_ascii, 2)
    T_STRING(purple_str_binary_to_ascii, 0)     // unsigned char*
    T_NUMBER(purple_str_binary_to_ascii, 1)     // guint

    RETURN_STRING((char *)purple_str_binary_to_ascii((unsigned char *)PARAM(0), (guint)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_got_protocol_handler_uri, 1)
    T_STRING(purple_got_protocol_handler_uri, 0)     // char*

    purple_got_protocol_handler_uri((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*
   CONCEPT_FUNCTION_IMPL(purple_url_parse,6)
        T_STRING(purple_url_parse, 0) // char*

        // ... parameter 1 is by reference (char**)
        char* local_parameter_1;
        // ... parameter 2 is by reference (int*)
        int local_parameter_2;
        // ... parameter 3 is by reference (char**)
        char* local_parameter_3;
        // ... parameter 4 is by reference (char**)
        char* local_parameter_4;
        // ... parameter 5 is by reference (char**)
        char* local_parameter_5;

        RETURN_NUMBER(purple_url_parse((char*)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
        SET_NUMBER(1, (long)local_parameter_1)
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
        SET_NUMBER(5, (long)local_parameter_5)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_util_fetch_url_request,8)
        T_STRING(purple_util_fetch_url_request, 0) // char*
        T_NUMBER(purple_util_fetch_url_request, 1) // int
        T_STRING(purple_util_fetch_url_request, 2) // char*
        T_NUMBER(purple_util_fetch_url_request, 3) // int
        T_STRING(purple_util_fetch_url_request, 4) // char*
        T_NUMBER(purple_util_fetch_url_request, 5) // int
        T_NUMBER(purple_util_fetch_url_request, 6) // PurpleUtilFetchUrlCallback
        T_NUMBER(purple_util_fetch_url_request, 7) // void*

        RETURN_NUMBER((long)purple_util_fetch_url_request((char*)PARAM(0), (int)PARAM(1), (char*)PARAM(2), (int)PARAM(3), (char*)PARAM(4), (int)PARAM(5), (PurpleUtilFetchUrlCallback)PARAM(6), (void *)(long)PARAM(7)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_util_fetch_url_request_len,9)
        T_STRING(purple_util_fetch_url_request_len, 0) // char*
        T_NUMBER(purple_util_fetch_url_request_len, 1) // int
        T_STRING(purple_util_fetch_url_request_len, 2) // char*
        T_NUMBER(purple_util_fetch_url_request_len, 3) // int
        T_STRING(purple_util_fetch_url_request_len, 4) // char*
        T_NUMBER(purple_util_fetch_url_request_len, 5) // int
        T_NUMBER(purple_util_fetch_url_request_len, 6) // gssize
        T_NUMBER(purple_util_fetch_url_request_len, 7) // PurpleUtilFetchUrlCallback
        T_NUMBER(purple_util_fetch_url_request_len, 8) // void*

        RETURN_NUMBER((long)purple_util_fetch_url_request_len((char*)PARAM(0), (int)PARAM(1), (char*)PARAM(2), (int)PARAM(3), (char*)PARAM(4), (int)PARAM(5), (gssize)PARAM(6), (PurpleUtilFetchUrlCallback)PARAM(7), (void *)(long)PARAM(8)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(purple_util_fetch_url_request_len_with_account,10)
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 0) // PurpleAccount*
        T_STRING(purple_util_fetch_url_request_len_with_account, 1) // char*
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 2) // int
        T_STRING(purple_util_fetch_url_request_len_with_account, 3) // char*
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 4) // int
        T_STRING(purple_util_fetch_url_request_len_with_account, 5) // char*
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 6) // int
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 7) // gssize
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 8) // PurpleUtilFetchUrlCallback
        T_NUMBER(purple_util_fetch_url_request_len_with_account, 9) // void*

        RETURN_NUMBER((long)purple_util_fetch_url_request_len_with_account((PurpleAccount*)(long)PARAM(0), (char*)PARAM(1), (int)PARAM(2), (char*)PARAM(3), (int)PARAM(4), (char*)PARAM(5), (int)PARAM(6), (gssize)PARAM(7), (PurpleUtilFetchUrlCallback)PARAM(8), (void *)(long)PARAM(9)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_util_fetch_url_cancel, 1)
    T_NUMBER(purple_util_fetch_url_cancel, 0)     // PurpleUtilFetchUrlData*

    purple_util_fetch_url_cancel((PurpleUtilFetchUrlData *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_url_decode, 1)
    T_STRING(purple_url_decode, 0)     // char*

    RETURN_STRING((char *)purple_url_decode((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_url_encode, 1)
    T_STRING(purple_url_encode, 0)     // char*

    RETURN_STRING((char *)purple_url_encode((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_email_is_valid, 1)
    T_STRING(purple_email_is_valid, 0)     // char*

    RETURN_NUMBER(purple_email_is_valid((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ip_address_is_valid, 1)
    T_STRING(purple_ip_address_is_valid, 0)     // char*

    RETURN_NUMBER(purple_ip_address_is_valid((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ipv4_address_is_valid, 1)
    T_STRING(purple_ipv4_address_is_valid, 0)     // char*

    RETURN_NUMBER(purple_ipv4_address_is_valid((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_ipv6_address_is_valid, 1)
    T_STRING(purple_ipv6_address_is_valid, 0)     // char*

    RETURN_NUMBER(purple_ipv6_address_is_valid((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_uri_list_extract_uris, 1)
    T_STRING(purple_uri_list_extract_uris, 0)     // char*

    RETURN_NUMBER((long)purple_uri_list_extract_uris((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_uri_list_extract_filenames, 1)
    T_STRING(purple_uri_list_extract_filenames, 0)     // char*

    RETURN_NUMBER((long)purple_uri_list_extract_filenames((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_try_convert, 1)
    T_STRING(purple_utf8_try_convert, 0)     // char*

    RETURN_STRING((char *)purple_utf8_try_convert((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_salvage, 1)
    T_STRING(purple_utf8_salvage, 0)     // char*

    RETURN_STRING((char *)purple_utf8_salvage((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_strip_unprintables, 1)
    T_STRING(purple_utf8_strip_unprintables, 0)     // char*

    RETURN_STRING((char *)purple_utf8_strip_unprintables((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_gai_strerror, 1)
    T_NUMBER(purple_gai_strerror, 0)     // int

    RETURN_STRING((char *)purple_gai_strerror((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_strcasecmp, 2)
    T_STRING(purple_utf8_strcasecmp, 0)     // char*
    T_STRING(purple_utf8_strcasecmp, 1)     // char*

    RETURN_NUMBER(purple_utf8_strcasecmp((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_utf8_has_word, 2)
    T_STRING(purple_utf8_has_word, 0)     // char*
    T_STRING(purple_utf8_has_word, 1)     // char*

    RETURN_NUMBER(purple_utf8_has_word((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_print_utf8_to_console, 2)
    T_NUMBER(purple_print_utf8_to_console, 0)     // FILE*
    T_STRING(purple_print_utf8_to_console, 1)     // char*

    purple_print_utf8_to_console((FILE *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_message_meify, 2)
    T_STRING(purple_message_meify, 0)     // char*
    T_NUMBER(purple_message_meify, 1)     // gssize

    RETURN_NUMBER(purple_message_meify((char *)PARAM(0), (gssize)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_text_strip_mnemonic, 1)
    T_STRING(purple_text_strip_mnemonic, 0)     // char*

    RETURN_STRING((char *)purple_text_strip_mnemonic((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_unescape_filename, 1)
    T_STRING(purple_unescape_filename, 0)     // char*

    RETURN_STRING((char *)purple_unescape_filename((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_escape_filename, 1)
    T_STRING(purple_escape_filename, 0)     // char*

    RETURN_STRING((char *)purple_escape_filename((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_purple_oscar_convert, 2)
    T_STRING(_purple_oscar_convert, 0)     // char*
    T_STRING(_purple_oscar_convert, 1)     // char*

    RETURN_STRING((char *)_purple_oscar_convert((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_restore_default_signal_handlers, 0)

    purple_restore_default_signal_handlers();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_get_host_name, 0)

    RETURN_STRING((char *)purple_get_host_name())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_new, 1)
    T_NUMBER(purple_value_new, 0)     // PurpleType

    RETURN_NUMBER((long)purple_value_new((PurpleType)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_new_outgoing, 1)
    T_NUMBER(purple_value_new_outgoing, 0)     // PurpleType

    RETURN_NUMBER((long)purple_value_new_outgoing((PurpleType)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_destroy, 1)
    T_NUMBER(purple_value_destroy, 0)     // PurpleValue*

    purple_value_destroy((PurpleValue *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_dup, 1)
    T_NUMBER(purple_value_dup, 0)     // PurpleValue*

    RETURN_NUMBER((long)purple_value_dup((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_type, 1)
    T_NUMBER(purple_value_get_type, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_type((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_subtype, 1)
    T_NUMBER(purple_value_get_subtype, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_subtype((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_specific_type, 1)
    T_NUMBER(purple_value_get_specific_type, 0)     // PurpleValue*

    RETURN_STRING((char *)purple_value_get_specific_type((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_is_outgoing, 1)
    T_NUMBER(purple_value_is_outgoing, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_is_outgoing((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_char, 2)
    T_NUMBER(purple_value_set_char, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_char, 1)     // char

    purple_value_set_char((PurpleValue *)(long)PARAM(0), (char)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_uchar, 2)
    T_NUMBER(purple_value_set_uchar, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_uchar, 1)     // unsigned char

    purple_value_set_uchar((PurpleValue *)(long)PARAM(0), (unsigned char)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_boolean, 2)
    T_NUMBER(purple_value_set_boolean, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_boolean, 1)     // int

    purple_value_set_boolean((PurpleValue *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_short, 2)
    T_NUMBER(purple_value_set_short, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_short, 1)     // short

    purple_value_set_short((PurpleValue *)(long)PARAM(0), (short)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_ushort, 2)
    T_NUMBER(purple_value_set_ushort, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_ushort, 1)     // unsigned short

    purple_value_set_ushort((PurpleValue *)(long)PARAM(0), (unsigned short)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_int, 2)
    T_NUMBER(purple_value_set_int, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_int, 1)     // int

    purple_value_set_int((PurpleValue *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_uint, 2)
    T_NUMBER(purple_value_set_uint, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_uint, 1)     // unsigned int

    purple_value_set_uint((PurpleValue *)(long)PARAM(0), (unsigned int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_long, 2)
    T_NUMBER(purple_value_set_long, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_long, 1)     // long

    purple_value_set_long((PurpleValue *)(long)PARAM(0), (long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_ulong, 2)
    T_NUMBER(purple_value_set_ulong, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_ulong, 1)     // unsigned long

    purple_value_set_ulong((PurpleValue *)(long)PARAM(0), (unsigned long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_int64, 2)
    T_NUMBER(purple_value_set_int64, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_int64, 1)     // int64

    purple_value_set_int64((PurpleValue *)(long)PARAM(0), (long long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_uint64, 2)
    T_NUMBER(purple_value_set_uint64, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_uint64, 1)     // guint64

    purple_value_set_uint64((PurpleValue *)(long)PARAM(0), (guint64)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_string, 2)
    T_NUMBER(purple_value_set_string, 0)     // PurpleValue*
    T_STRING(purple_value_set_string, 1)     // char*

    purple_value_set_string((PurpleValue *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_object, 2)
    T_NUMBER(purple_value_set_object, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_object, 1)     // void*

    purple_value_set_object((PurpleValue *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_pointer, 2)
    T_NUMBER(purple_value_set_pointer, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_pointer, 1)     // void*

    purple_value_set_pointer((PurpleValue *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_enum, 2)
    T_NUMBER(purple_value_set_enum, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_enum, 1)     // int

    purple_value_set_enum((PurpleValue *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_set_boxed, 2)
    T_NUMBER(purple_value_set_boxed, 0)     // PurpleValue*
    T_NUMBER(purple_value_set_boxed, 1)     // void*

    purple_value_set_boxed((PurpleValue *)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_char, 1)
    T_NUMBER(purple_value_get_char, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_char((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_uchar, 1)
    T_NUMBER(purple_value_get_uchar, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_uchar((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_boolean, 1)
    T_NUMBER(purple_value_get_boolean, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_boolean((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_short, 1)
    T_NUMBER(purple_value_get_short, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_short((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_ushort, 1)
    T_NUMBER(purple_value_get_ushort, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_ushort((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_int, 1)
    T_NUMBER(purple_value_get_int, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_int((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_uint, 1)
    T_NUMBER(purple_value_get_uint, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_uint((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_long, 1)
    T_NUMBER(purple_value_get_long, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_long((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_ulong, 1)
    T_NUMBER(purple_value_get_ulong, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_ulong((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_int64, 1)
    T_NUMBER(purple_value_get_int64, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_int64((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_uint64, 1)
    T_NUMBER(purple_value_get_uint64, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_uint64((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_string, 1)
    T_NUMBER(purple_value_get_string, 0)     // PurpleValue*

    RETURN_STRING((char *)purple_value_get_string((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_object, 1)
    T_NUMBER(purple_value_get_object, 0)     // PurpleValue*

    RETURN_NUMBER((long)purple_value_get_object((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_pointer, 1)
    T_NUMBER(purple_value_get_pointer, 0)     // PurpleValue*

    RETURN_NUMBER((long)purple_value_get_pointer((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_enum, 1)
    T_NUMBER(purple_value_get_enum, 0)     // PurpleValue*

    RETURN_NUMBER(purple_value_get_enum((PurpleValue *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(purple_value_get_boxed, 1)
    T_NUMBER(purple_value_get_boxed, 0)     // PurpleValue*

    RETURN_NUMBER((long)purple_value_get_boxed((PurpleValue *)(long)PARAM(0)))
END_IMPL

