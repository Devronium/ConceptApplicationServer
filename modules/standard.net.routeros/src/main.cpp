//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
extern "C" {
#include "mikrotik-api.h"
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(1, 0), &wsa);
#endif
    DEFINE_SCONSTANT("ROS_NO_CONNECTION", "-1");
    DEFINE_SCONSTANT("ROS_INVALID_USER_PASS", "-2");
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ROSConnect, 3, 4)
    T_STRING(0) // username
    T_STRING(1) // password
    T_STRING(2) // host

    int port = 8728;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(3);
        port = PARAM_INT(3);
    }

    int sock = apiConnect(PARAM(2), port);
    if (sock < 0) {
        RETURN_NUMBER(-1);
    } else {
        int iLoginResult = login(sock, PARAM(0), PARAM(1));
        if (!iLoginResult) {
            RETURN_NUMBER(-2)
        } else {
            RETURN_NUMBER(sock);
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ROSDisconnect, 1)
    T_NUMBER(0)

    int sock = PARAM_INT(0);
    if (sock > 0) {
        apiDisconnect(sock);
        SET_NUMBER(0, -1);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ROSQuery, 2)
    T_NUMBER(0)
    T_ARRAY(1)

    struct Sentence stSentence;
    struct Block    stBlock;

    int sock = PARAM_INT(0);
    if (sock < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    int len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (len) {
        char **server_groups = GetCharList(PARAMETER(1), Invoke);
        if (server_groups) {
            initializeSentence(&stSentence);
            char *word;
            int  i = 0;
            do {
                word = server_groups[i++];
                if (word)
                    addWordToSentence(&stSentence, word);
            } while (word);
            writeSentence(sock, &stSentence);
            delete server_groups;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ROSResult, 1)
    T_NUMBER(0)

    CREATE_ARRAY(RESULT);
    struct Block stBlock;

    int sock = PARAM_INT(0);
    if (sock < 0) {
        return 0;
    }

    stBlock = readBlock(sock);
    int index = 0;
    for (int i = 0; i < stBlock.iLength; i++) {
        Sentence *s = stBlock.stSentence[i];
        if (s) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, index++, &newpData);
            if (newpData) {
                CREATE_ARRAY(newpData);
                Invoke(INVOKE_SET_ARRAY_ELEMENT, newpData, 0, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)s->iReturnValue);
                for (int j = 0; j < s->iLength; j++) {
                    char *word2 = s->szSentence[j];
                    if (word2) {
                        if (word2[0] == '=') {
                            word2++;
                            char *pos = strchr(word2, '=');
                            if (pos) {
                                pos[0] = 0;
                                pos++;
                                char *key = word2;
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, key, (INTEGER)VARIABLE_STRING, (char *)pos, (NUMBER)0);
                                continue;
                            }
                        }
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, newpData, j + 1, (INTEGER)VARIABLE_STRING, (char *)word2, (NUMBER)0);
                    }
                }
            }
        }
    }
END_IMPL
//-----------------------------------------------------//
