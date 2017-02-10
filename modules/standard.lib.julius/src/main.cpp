//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <string.h>
#include "AnsiString.h"
#include <julius/julius.h>

static INVOKE_CALL InvokePtr = 0; 

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    DEFINE_ECONSTANT(J_RESULT_STATUS_REJECT_POWER)
    DEFINE_ECONSTANT(J_RESULT_STATUS_TERMINATE)
    DEFINE_ECONSTANT(J_RESULT_STATUS_ONLY_SILENCE)
    DEFINE_ECONSTANT(J_RESULT_STATUS_REJECT_GMM)
    DEFINE_ECONSTANT(J_RESULT_STATUS_REJECT_SHORT)
    DEFINE_ECONSTANT(J_RESULT_STATUS_FAIL)
    DEFINE_ECONSTANT(J_RESULT_STATUS_SUCCESS)
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
static void status_recog_event(Recog *recog, void *delegate) {
    void *RES             = 0;
    void *EXCEPTION       = 0;

    if (delegate) {
        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)0);//, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)0);
        if (EXCEPTION)
            InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
        if (RES) {
            INTEGER type  = 0;
            NUMBER  nData = 0;
            char    *str  = 0;

            InvokePtr(INVOKE_GET_VARIABLE, RES, &type, &str, &nData);
            if ((type == VARIABLE_NUMBER) && (nData == 1))
                j_request_terminate(recog);
            InvokePtr(INVOKE_FREE_VARIABLE, RES);
        }
    }
}

static AnsiString put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo) {
    int         i, j;
    WORD_ID     w;
    AnsiString  res;
    static char buf[MAX_HMMNAME_LEN];

    if (seq != NULL)
        for (i = 0; i < n; i++) {
            if (i > 0)
                res += (char *)" |";
            w = seq[i];
            for (j = 0; j < winfo->wlen[w]; j++) {
                if (j)
                    res += (char *)" ";
                center_name(winfo->wseq[w][j]->name, buf);
                res += buf;
            }
        }
    return res;
} 

static void output_result(Recog *recog, void *delegate) {
    int          i, j;
    int          len;
    WORD_INFO    *winfo;
    WORD_ID      *seq;
    int          seqnum;
    int          n;
    Sentence     *s;
    RecogProcess *r;
    HMM_Logical  *p;

    INVOKE_CALL Invoke = InvokePtr; 
    void *var = 0;
    CREATE_VARIABLE(var);
    CREATE_ARRAY(var);

    /* all recognition results are stored at each recognition process
     * instance */
    bool       has_data = false;
    
    INTEGER idx = 0;
    for (r = recog->process_list; r; r = r->next) {
        /* skip the process if the process is not alive */
        if (!r->live)
            continue;

        /* result are in r->result.  See recog.h for details */

        void *var2 = 0;
        Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)idx++, &var2);
        /* check result status */
        if (r->result.status < 0) {         /* no results obtained */
            /* outout message according to the status code */
            Invoke(INVOKE_SET_VARIABLE, var2, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)r->result.status);
            continue;
        }

        /* output results for all the obtained sentences */
        winfo = r->lm->winfo;

        CREATE_ARRAY(var2);
        INTEGER idx2 = 0;
        for (n = 0; n < r->result.sentnum; n++) {         /* for all sentences */
            has_data = true;
            s        = &(r->result.sent[n]);
            seq      = s->word;
            seqnum   = s->word_num;
            if (!seqnum)
                continue;

            void *var3 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, var2, (INTEGER)idx2++, &var3);
            CREATE_ARRAY(var3);


            void *var4 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var3, (INTEGER)"s", &var4);
            CREATE_ARRAY(var4);

            /* output word sequence like Julius */
            for (i = 0; i < seqnum; i++)
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var4, (INTEGER)i, (INTEGER)VARIABLE_STRING, winfo->woutput[seq[i]], (NUMBER)0);

            var4 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var3, (INTEGER)"wsek", &var4);
            CREATE_ARRAY(var4);

            for (i = 0; i < seqnum; i++)
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var4, (INTEGER)i, (INTEGER)VARIABLE_STRING, winfo->wname[seq[i]], (NUMBER)0);

            /* phoneme sequence */
            AnsiString phoneme(put_hypo_phoneme(seq, seqnum, winfo));
            if (phoneme.Length())
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var3, "phoneme", (INTEGER)VARIABLE_STRING, phoneme.c_str(), (NUMBER)phoneme.Length());

            var4 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var3, (INTEGER)"score", &var4);
            CREATE_ARRAY(var4);

            /* confidence scores */
            for (i = 0; i < seqnum; i++)
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var4, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)s->confidence[i]);
        }
    }
    if (delegate) {
        void *RES             = 0;
        void *EXCEPTION       = 0;

        INTEGER type  = 0;
        NUMBER  nData = 0;
        char    *str  = 0;

        InvokePtr(INVOKE_GET_VARIABLE, var, &type, &str, &nData);

        InvokePtr(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)1, (INTEGER)type, str, nData);
        if (EXCEPTION)
            InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
        if (RES) {
            InvokePtr(INVOKE_GET_VARIABLE, RES, &type, &str, &nData);
            if ((type == VARIABLE_NUMBER) && (nData))
                j_request_terminate(recog);
            InvokePtr(INVOKE_FREE_VARIABLE, RES);
        }
    }
    FREE_VARIABLE(var);
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SpeechLoop, 1, 2)
    T_ARRAY("SpeechLoop", 0)
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    int argc = 1;
    char **argv = (char **)malloc((count * 2 + 1) * sizeof(char *));
    argv[0] = "SpeechLoop";
    bool has_mic = false;
    char *empty = (char *)"";
    void *on_result = 0;
    void *on_start = 0;
    void *on_stop = 0;
    void *on_ready = 0;
    void *on_recog_start = 0;
    void *on_recog_end = 0;
    if (count > 0) {
        for (INTEGER i = 0; i < count; i++) {
            char *key = 0;
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(0), i, &key);
            INTEGER type  = 0;
            NUMBER  dummy = 0;
            char    *str  = 0;

            if (key) {
                Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), key, &type, &str, &dummy);
                if (strcmp(key, "OnResult") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_result); 
                } else
                if (strcmp(key, "OnStart") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_start); 
                } else
                if (strcmp(key, "OnStop") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_stop); 
                } else
                if (strcmp(key, "OnReady") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_ready); 
                } else
                if (strcmp(key, "OnRecognitionStart") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_recog_start); 
                } else
                if (strcmp(key, "OnRecognitionEnd") == 0) {
                    if (type == VARIABLE_DELEGATE)
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(0), key, &on_recog_end); 
                } else
                    argv[argc++] = key;
            } else
                Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(0), i, &type, &str, &dummy);
            switch (type) {
                case VARIABLE_STRING:
                    if ((str) && (str[0])) {
                        argv[argc++] = str;
                        if ((key) && (!strcmp(key, "-input")) && (!strcmp(str, "mic")))
                            has_mic = true;
                    } else
                        argv[argc++] = empty;
                    break;
            }
        }
    }
    if (PARAMETERS_COUNT > 1) {
        SET_STRING(1, "");
    }

    Jconf *jconf = j_config_load_args_new(argc, argv);
    if (!jconf) {
        if (PARAMETERS_COUNT > 1) {
            SET_STRING(1, "j_config_load_args_new");
        }
        free(argv);
        RETURN_NUMBER(-1);
        return 0;
    }

    Recog *recog = j_create_instance_from_jconf(jconf);
    if (!recog) {
        if (PARAMETERS_COUNT > 1) {
            SET_STRING(1, "j_create_instance_from_jconf");
        }
        j_jconf_free(jconf);
        free(argv);
        RETURN_NUMBER(-2);
        return 0;
    }

    if ((has_mic) && (j_adin_init(recog) == FALSE)) {
        if (PARAMETERS_COUNT > 1) {
            SET_STRING(1, "j_adin_init");
        }
        j_jconf_free(jconf);
        free(argv);
        j_recog_free(recog);
        return 0;
    }

    if (on_ready) {
        callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recog_event, on_ready);
        Invoke(INVOKE_LOCK_VARIABLE, on_ready);
    }
    if (on_start) {
        callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recog_event, on_start);
        Invoke(INVOKE_LOCK_VARIABLE, on_start);
    }
    if (on_stop) {
        callback_add(recog, CALLBACK_EVENT_SPEECH_STOP, status_recog_event, on_stop);
        Invoke(INVOKE_LOCK_VARIABLE, on_stop);
    }
    if (on_recog_start) {
        // callback_add(recog, CALLBACK_EVENT_RECOGNITION_START status_recog_event, on_recog_start);
        Invoke(INVOKE_LOCK_VARIABLE, on_recog_start);
    }
    if (on_recog_end) {
        // callback_add(recog, CALLBACK_EVENT_RECOGNITION_END, status_recog_event, on_recog_end);
        Invoke(INVOKE_LOCK_VARIABLE, on_recog_end);
    }

    if (on_result)
        callback_add(recog, CALLBACK_RESULT, output_result, on_result);

    bool stream_ok = false;
    switch (j_open_stream(recog, NULL)) {
        case 0:
            stream_ok = true;
            break;

        case -1:
            if (PARAMETERS_COUNT > 1) {
                SET_STRING(1, "error in input stream");
            }
            break;

        case -2:
            if (PARAMETERS_COUNT > 1) {
                SET_STRING(1, "failed to begin input stream");
            }
            break;
    }

    int ret = -1;
    if (stream_ok)
        ret = j_recognize_stream(recog);
    if (jconf)
        j_jconf_free(jconf);
    if (stream_ok)
        j_close_stream(recog);
    j_recog_free(recog);
    free(argv);

    if (on_ready)
        Invoke(INVOKE_FREE_VARIABLE, on_ready);

    if (on_start)
        Invoke(INVOKE_FREE_VARIABLE, on_start);

    if (on_stop)
        Invoke(INVOKE_FREE_VARIABLE, on_stop);

    if (on_recog_start)
        Invoke(INVOKE_FREE_VARIABLE, on_recog_start);

    if (on_recog_end)
        Invoke(INVOKE_FREE_VARIABLE, on_recog_end);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
