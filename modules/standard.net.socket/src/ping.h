#ifndef __PING_H
#define __PING_H

#include "stdlibrary.h"

int Ping(char *pszRemoteHost, int nCount, int nMessageSize, int nTimeOut, INVOKE_CALL Invoke, void *RESULT);
#endif // __PING_H
