//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
//extern "C" {
#include <curl/curl.h>
//}
#include "twitcurl.h"

//=====================================================================================//
INVOKE_CALL InvokePtr = 0;
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_Auth, 6)
    T_HANDLE2(Twitter_Auth, 0, twitCurl *)
    T_STRING(Twitter_Auth, 1) // public key file
    T_STRING(Twitter_Auth, 2) // private key file
    T_STRING(Twitter_Auth, 3) // twitter provided pin
    T_STRING(Twitter_Auth, 4) // twitter provided pin
    T_STRING(Twitter_Auth, 5) // twitter provided pin

    twitCurl * twitterObj = PARAM(0);
/* OAuth flow begins */
/* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj->getOAuth().setConsumerKey(std::string(PARAM(4)));
    twitterObj->getOAuth().setConsumerSecret(std::string(PARAM(5)));

/* Step 1: Check if we alredy have OAuth access token from a previous run */
//char szKey[1024];
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
//ifstream oAuthTokenKeyIn;
//ifstream oAuthTokenSecretIn;

//oAuthTokenKeyIn.open( PARAM(1) );
//oAuthTokenSecretIn.open( PARAM(2) );

//memset( szKey, 0, 1024 );
//oAuthTokenKeyIn >> szKey;
//myOAuthAccessTokenKey = szKey;

//memset( szKey, 0, 1024 );
//oAuthTokenSecretIn >> szKey;
    myOAuthAccessTokenKey    = PARAM(1);
    myOAuthAccessTokenSecret = PARAM(2);

//oAuthTokenKeyIn.close();
//oAuthTokenSecretIn.close();

    if (myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size()) {
        twitterObj->getOAuth().setOAuthTokenKey(myOAuthAccessTokenKey);
        twitterObj->getOAuth().setOAuthTokenSecret(myOAuthAccessTokenSecret);
    } else {
        std::string tmpStr("");
        /* Step 2: Get request token key and secret */
        twitterObj->oAuthRequestToken(tmpStr);

        /* Step 3: Ask user to visit web link and get PIN */
        //char szOAuthVerifierPin[1024];
        //memset( szOAuthVerifierPin, 0, 1024 );
        tmpStr = PARAM(3);
        twitterObj->getOAuth().setOAuthPin(tmpStr);

        /* Step 4: Exchange request token with access token */
        twitterObj->oAuthAccessToken();

        /* Step 5: Now, save this access token key and secret for future use without PIN */
        twitterObj->getOAuth().getOAuthTokenKey(myOAuthAccessTokenKey);
        twitterObj->getOAuth().getOAuthTokenSecret(myOAuthAccessTokenSecret);

        /* Step 6: Save these keys in a file or wherever */

        /*ofstream oAuthTokenKeyOut;
           ofstream oAuthTokenSecretOut;

           oAuthTokenKeyOut.open( PARAM(1) );
           oAuthTokenSecretOut.open( PARAM(2) );

           oAuthTokenKeyOut.clear();
           oAuthTokenSecretOut.clear();

           oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
           oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

           oAuthTokenKeyOut.close();
           oAuthTokenSecretOut.close();*/
    }
/* OAuth flow ends */
    RETURN_NUMBER(0)
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_Create, 0)
    twitCurl * inst = new twitCurl();
    RETURN_NUMBER((long)inst);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_Destroy, 1)
    T_HANDLE2(Twitter_Destroy, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    delete inst;
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getTwitterUsername, 1)
    T_HANDLE2(Twitter_getTwitterUsername, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING(inst->getTwitterUsername().c_str());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getTwitterPassword, 1)
    T_HANDLE2(Twitter_getTwitterPassword, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING(inst->getTwitterPassword().c_str());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setTwitterUsername, 2)
    T_HANDLE2(Twitter_setTwitterUsername, 0, twitCurl *)
    T_STRING(Twitter_setTwitterUsername, 1)
    twitCurl * inst = PARAM(0);
    std::string tmp = PARAM(1);

    inst->setTwitterUsername(tmp);
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setTwitterPassword, 2)
    T_HANDLE2(Twitter_setTwitterPassword, 0, twitCurl *)
    T_STRING(Twitter_setTwitterPassword, 1)
    twitCurl * inst = PARAM(0);
    std::string tmp = PARAM(1);

    inst->setTwitterPassword(tmp);
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_search, 2)
    T_HANDLE2(Twitter_search, 0, twitCurl *)
    T_STRING(Twitter_search, 1)
    twitCurl * inst = PARAM(0);
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->search(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_statusUpdate, 2)
    T_HANDLE2(Twitter_statusUpdate, 0, twitCurl *)
    T_STRING(Twitter_statusUpdate, 1)
    twitCurl * inst = PARAM(0);
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->statusUpdate(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_statusShowById, 2)
    T_HANDLE2(Twitter_statusShowById, 0, twitCurl *)
    T_STRING(Twitter_statusShowById, 1)
    std::string tmp = PARAM(1);
    twitCurl *inst = PARAM(0);

    RETURN_NUMBER(inst->statusShowById(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_statusDestroyById, 2)
    T_HANDLE2(Twitter_statusDestroyById, 0, twitCurl *)
    T_STRING(Twitter_statusDestroyById, 1)
    twitCurl * inst = PARAM(0);
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->statusDestroyById(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_timelinePublicGet, 1)
    T_HANDLE2(Twitter_timelinePublicGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->timelinePublicGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_timelineFriendsGet, 1)
    T_HANDLE2(Twitter_timelineFriendsGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->timelineFriendsGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(Twitter_timelineUserGet, 3, 5)
    T_HANDLE2(Twitter_timelineUserGet, 0, twitCurl *)
    T_STRING(Twitter_timelineUserGet, 1)
    T_NUMBER(Twitter_timelineUserGet, 2)
    std::string tmp = PARAM(1);
    twitCurl *inst = PARAM(0);

    bool retwits = false;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(Twitter_timelineUserGet, 3)
        retwits = (bool)PARAM_INT(3);
    }
    int count = 200;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(Twitter_timelineUserGet, 4)
        count = PARAM_INT(4);
    }

    RETURN_NUMBER(inst->timelineUserGet(false, retwits, count, tmp, (bool)PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_featuredUsersGet, 1)
    T_HANDLE2(Twitter_featuredUsersGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->featuredUsersGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_mentionsGet, 1)
    T_HANDLE2(Twitter_mentionsGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->mentionsGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_userGet, 3)
    T_HANDLE2(Twitter_userGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_userGet, 1)
    T_NUMBER(Twitter_userGet, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->userGet(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_friendsGet, 3)
    T_HANDLE2(Twitter_friendsGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_friendsGet, 1)
    T_NUMBER(Twitter_friendsGet, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->friendsGet(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_followersGet, 3)
    T_HANDLE2(Twitter_followersGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_followersGet, 1)
    T_NUMBER(Twitter_followersGet, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->followersGet(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_directMessageGet, 1)
    T_HANDLE2(Twitter_directMessageGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->directMessageGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_directMessageSend, 4)
    T_HANDLE2(Twitter_directMessageSend, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_directMessageSend, 1)
    std::string tmp = PARAM(1);
    T_STRING(Twitter_directMessageSend, 2)
    std::string tmp2 = PARAM(2);
    T_NUMBER(Twitter_directMessageSend, 3)

    RETURN_NUMBER(inst->directMessageSend(tmp, tmp2, PARAM_INT(3)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_directMessageGetSent, 1)
    T_HANDLE2(Twitter_directMessageGetSent, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->directMessageGetSent());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_directMessageDestroyById, 2)
    T_HANDLE2(Twitter_directMessageDestroyById, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_directMessageDestroyById, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->directMessageDestroyById(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_friendshipCreate, 3)
    T_HANDLE2(Twitter_friendshipCreate, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_friendshipCreate, 1)
    T_NUMBER(Twitter_friendshipCreate, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->friendshipCreate(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_friendshipDestroy, 3)
    T_HANDLE2(Twitter_friendshipDestroy, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_friendshipDestroy, 1)
    T_NUMBER(Twitter_friendshipDestroy, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->friendshipDestroy(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_friendshipShow, 3)
    T_HANDLE2(Twitter_friendshipShow, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_friendshipShow, 1)
    T_NUMBER(Twitter_friendshipShow, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->friendshipShow(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_friendsIdsGet, 3)
    T_HANDLE2(Twitter_friendsIdsGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_friendsIdsGet, 1)
    T_NUMBER(Twitter_friendsIdsGet, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->friendsIdsGet(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_followersIdsGet, 3)
    T_HANDLE2(Twitter_followersIdsGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_followersIdsGet, 1)
    T_NUMBER(Twitter_followersIdsGet, 2)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->followersIdsGet(tmp, PARAM_INT(2)));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_accountRateLimitGet, 1)
    T_HANDLE2(Twitter_accountRateLimitGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->accountRateLimitGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_favoriteGet, 1)
    T_HANDLE2(Twitter_favoriteGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->favoriteGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_favoriteCreate, 2)
    T_HANDLE2(Twitter_favoriteCreate, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_favoriteCreate, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->favoriteCreate(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_favoriteDestroy, 2)
    T_HANDLE2(Twitter_favoriteDestroy, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_favoriteDestroy, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->favoriteDestroy(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_blockCreate, 2)
    T_HANDLE2(Twitter_blockCreate, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_blockCreate, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->blockCreate(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_blockDestroy, 2)
    T_HANDLE2(Twitter_blockDestroy, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_blockDestroy, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->blockDestroy(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_savedSearchGet, 1)
    T_HANDLE2(Twitter_savedSearchGet, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->savedSearchGet());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_savedSearchCreate, 2)
    T_HANDLE2(Twitter_savedSearchCreate, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_savedSearchCreate, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->savedSearchCreate(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_savedSearchShow, 2)
    T_HANDLE2(Twitter_savedSearchShow, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_savedSearchShow, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->savedSearchShow(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_savedSearchDestroy, 2)
    T_HANDLE2(Twitter_savedSearchDestroy, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_savedSearchDestroy, 1)
    std::string tmp = PARAM(1);

    RETURN_NUMBER(inst->savedSearchDestroy(tmp));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_isCurlInit, 1)
    T_HANDLE2(Twitter_isCurlInit, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_NUMBER(inst->isCurlInit());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getLastWebResponse, 1)
    T_HANDLE2(Twitter_getLastWebResponse, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    std::string tmp;

    inst->getLastWebResponse(tmp);
    RETURN_BUFFER(tmp.c_str(), tmp.length())
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getLastCurlError, 1)
    T_HANDLE2(Twitter_getLastCurlError, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    std::string tmp;

    inst->getLastCurlError(tmp);
    RETURN_BUFFER(tmp.c_str(), tmp.length())
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getProxyServerIp, 1)
    T_HANDLE2(Twitter_getProxyServerIp, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING((inst->getProxyServerIp().c_str()));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getProxyServerPort, 1)
    T_HANDLE2(Twitter_getProxyServerPort, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING((inst->getProxyServerPort().c_str()));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getProxyUserName, 1)
    T_HANDLE2(Twitter_getProxyUserName, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING((inst->getProxyUserName().c_str()));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_getProxyPassword, 1)
    T_HANDLE2(Twitter_getProxyPassword, 0, twitCurl *)
    twitCurl * inst = PARAM(0);

    RETURN_STRING((inst->getProxyPassword().c_str()));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setProxyServerIp, 2)
    T_HANDLE2(Twitter_setProxyServerIp, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_setProxyServerIp, 1)
    std::string tmp = PARAM(1);

    inst->setProxyServerIp(tmp);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setProxyServerPort, 2)
    T_HANDLE2(Twitter_setProxyServerPort, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_setProxyServerPort, 1)
    std::string tmp = PARAM(1);

    inst->setProxyServerPort(tmp);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setProxyUserName, 2)
    T_HANDLE2(Twitter_setProxyUserName, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_setProxyUserName, 1)
    std::string tmp = PARAM(1);

    inst->setProxyUserName(tmp);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(Twitter_setProxyPassword, 2)
    T_HANDLE2(Twitter_setProxyPassword, 0, twitCurl *)
    twitCurl * inst = PARAM(0);
    T_STRING(Twitter_setProxyPassword, 1)
    std::string tmp = PARAM(1);

    inst->setProxyPassword(tmp);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//


