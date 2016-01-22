#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#ifdef _WIN32
 #include <winsock2.h>
 #include <windows.h>
#else
 #include <unistd.h>
 #include <sys/time.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <string.h>
 #define SOCKET          int
 #define SOCKET_ERROR    -1
 #define SOCKADDR_IN     sockaddr_in
 #define SOCKADDR        sockaddr
#endif
#include "stdlibrary.h"

struct ICMPheader {
    unsigned char  byType;
    unsigned char  byCode;
    unsigned short nChecksum;
    unsigned short nId;
    unsigned short nSequence;
};

struct IPheader {
    unsigned char  byVerLen;
    unsigned char  byTos;
    unsigned short nTotalLength;
    unsigned short nId;
    unsigned short nOffset;
    unsigned char  byTtl;
    unsigned char  byProtocol;
    unsigned short nChecksum;
    unsigned int   nSrcAddr;
    unsigned int   nDestAddr;
};

unsigned short CalcChecksum(char *pBuffer, int nLen);
int ValidateChecksum(char *pBuffer, int nLen);
int ResolveIP(char *pszRemoteHost, char *pszIPAddress);

unsigned long GetMS() {
#ifdef _WIN32
    SYSTEMTIME tv;
    ::GetSystemTime(&tv);

    int sec = tv.wSecond;
    return sec * 1000 + tv.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

int Ping(char *pszRemoteHost, int nCount, int nMessageSize, int nTimeOut, INVOKE_CALL Invoke, void *RESULT) {
    int nSequence = 0;

    char pszRemoteIP[0xFF];
    char *pSendBuffer = NULL;

    if (ResolveIP(pszRemoteHost, pszRemoteIP) == 0) {
        //cerr << endl << "Unable to resolve hostname" << endl;
        return -2;
    }

    //cout << "Pinging " << pszRemoteHost << " [" << pszRemoteIP << "] with " << nMessageSize << " bytes of data." << endl << endl;
    ICMPheader sendHdr;

    SOCKET sock;
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //Create a raw socket which will use ICMP

    SOCKADDR_IN dest;                               //Dest address to send the ICMP request
#ifdef _WIN32
    dest.sin_addr.S_un.S_addr = inet_addr(pszRemoteIP);
#else
    dest.sin_addr.s_addr = inet_addr(pszRemoteIP);
#endif
    dest.sin_family = AF_INET;
    dest.sin_port   = rand();           //Pick a random port

    int nResult = 0;

    fd_set fdRead;
    int    timeSend, timeRecv;
    int    nTotalRoundTripTime = 0, nMaxRoundTripTime = 0, nMinRoundTripTime = -1, nRoundTripTime = 0;
    int    nPacketsSent        = 0, nPacketsReceived = 0;

    timeval timeInterval = { 0, 0 };
    int     sec          = nTimeOut / 1000;
    int     ms           = nTimeOut % 1000;
    timeInterval.tv_sec  = sec;
    timeInterval.tv_usec = ms * 1000;

    sendHdr.nId = htons(rand());        //Set the transaction Id

    void *REPLY;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "reply", &REPLY);
    Invoke(INVOKE_CREATE_ARRAY, REPLY);
    while (nPacketsSent < nCount) {
        //Create the message buffer, which is big enough to store the header and the message data
        pSendBuffer = new char [sizeof(ICMPheader) + nMessageSize];

        sendHdr.byCode    = 0;                                       //Zero for ICMP echo and reply messages
        sendHdr.nSequence = htons(nSequence++);
        sendHdr.byType    = 8;                                       //Eight for ICMP echo message
        sendHdr.nChecksum = 0;                                       //Checksum is calculated later on

        memcpy(pSendBuffer, &sendHdr, sizeof(ICMPheader));           //Copy the message header in the buffer
        memset(pSendBuffer + sizeof(ICMPheader), 'x', nMessageSize); //Fill the message with some arbitary value

        //Calculate checksum over ICMP header and message data
        sendHdr.nChecksum = htons(CalcChecksum(pSendBuffer, sizeof(ICMPheader) + nMessageSize));

        //Copy the message header back into the buffer
        memcpy(pSendBuffer, &sendHdr, sizeof(ICMPheader));

        nResult = sendto(sock, pSendBuffer, sizeof(ICMPheader) + nMessageSize, 0, (SOCKADDR *)&dest, sizeof(SOCKADDR_IN));

        //Save the time at which the ICMP echo message was sent
        //::GetSystemTime (&timeSend);
        timeSend = GetMS();

        ++nPacketsSent;

        if (nResult == SOCKET_ERROR) {
            delete []pSendBuffer;
            return -1;
        }

        FD_ZERO(&fdRead);
        FD_SET(sock, &fdRead);

        if ((nResult = select(sock + 1, &fdRead, NULL, NULL, &timeInterval))
            == SOCKET_ERROR) {
            //cerr << endl << "An error occured in select operation: " << "WSAGetLastError () = " <<
            //	WSAGetLastError () << endl;
            delete []pSendBuffer;
            return -1;
        }

        if ((nResult > 0) && FD_ISSET(sock, &fdRead)) {
            //Allocate a large buffer to store the response
            char *pRecvBuffer = new char [1500];

            if ((nResult = recvfrom(sock, pRecvBuffer, 1500, 0, 0, 0))
                == SOCKET_ERROR) {
                //cerr << endl << "An error occured in recvfrom operation: " << "WSAGetLastError () = " <<
                //	WSAGetLastError () << endl;
                delete []pSendBuffer;
                delete []pRecvBuffer;
                return -1;
            }

            //Get the time at which response is received
            //::GetSystemTime (&timeRecv);
            timeRecv = GetMS();

            //We got a response so we construct the ICMP header and message out of it
            ICMPheader recvHdr;
            char       *pICMPbuffer = NULL;

            //The response includes the IP header as well, so we move 20 bytes ahead to read the ICMP header
            pICMPbuffer = pRecvBuffer + sizeof(IPheader);

            //ICMP message length is calculated by subtracting the IP header size from the
            //total bytes received
            int nICMPMsgLen = nResult - sizeof(IPheader);

            //Construct the ICMP header
            memcpy(&recvHdr, pICMPbuffer, sizeof(recvHdr));

            //Construct the IP header from the response
            IPheader ipHdr;
            memcpy(&ipHdr, pRecvBuffer, sizeof(ipHdr));

            recvHdr.nId       = recvHdr.nId;
            recvHdr.nSequence = recvHdr.nSequence;
            recvHdr.nChecksum = ntohs(recvHdr.nChecksum);

            //Check if the response is an echo reply, transaction ID and sequence number are same
            //as for the request, and that the checksum is correct
            if ((recvHdr.byType == 0) &&
                (recvHdr.nId == sendHdr.nId) &&
                (recvHdr.nSequence == sendHdr.nSequence) &&
                ValidateChecksum(pICMPbuffer, nICMPMsgLen) &&
                (memcmp(pSendBuffer + sizeof(ICMPheader), pRecvBuffer + sizeof(ICMPheader) + sizeof(IPheader),
                        nResult - sizeof(ICMPheader) - sizeof(IPheader)) == 0)) {
                //All's OK
                int nSec = (timeRecv - timeSend) / 1000;              //timeRecv.wSecond - timeSend.wSecond;
                if (nSec < 0) {
                    nSec = nSec + 60;
                }

                int nMilliSec = abs(timeRecv - timeSend);

                int nRoundTripTime = 0;
                nRoundTripTime = abs(nSec * 1000 - nMilliSec);

                void *VAR;
                int  index = Invoke(INVOKE_GET_ARRAY_COUNT, REPLY);
                Invoke(INVOKE_ARRAY_VARIABLE, REPLY, (INTEGER)index, &VAR);
                Invoke(INVOKE_CREATE_ARRAY, VAR);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "from", (INTEGER)VARIABLE_STRING, pszRemoteIP, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "bytes", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)(nResult - sizeof(ICMPheader) - sizeof(IPheader)));
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "time", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nRoundTripTime);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "TTL", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)ipHdr.byTtl);

                nTotalRoundTripTime = nTotalRoundTripTime + nRoundTripTime;

                if (nMinRoundTripTime == -1) {
                    nMinRoundTripTime = nRoundTripTime;
                    nMaxRoundTripTime = nRoundTripTime;
                } else if (nRoundTripTime < nMinRoundTripTime) {
                    nMinRoundTripTime = nRoundTripTime;
                } else if (nRoundTripTime > nMaxRoundTripTime) {
                    nMaxRoundTripTime = nRoundTripTime;
                }

                ++nPacketsReceived;
            } else {
                void *VAR;
                Invoke(INVOKE_ARRAY_VARIABLE, REPLY, (INTEGER)Invoke(INVOKE_GET_ARRAY_COUNT, REPLY), &VAR);
                Invoke(INVOKE_CREATE_ARRAY, VAR);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "from", (INTEGER)VARIABLE_STRING, pszRemoteIP, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "msg", (INTEGER)VARIABLE_STRING, "The echo reply is not correct!", (NUMBER)ipHdr.byTtl);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "err", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)1);
            }

            delete []pRecvBuffer;
        } else {
            //cout << "Request timed out." << endl;
            void *VAR;
            Invoke(INVOKE_ARRAY_VARIABLE, REPLY, (INTEGER)Invoke(INVOKE_GET_ARRAY_COUNT, REPLY), &VAR);
            Invoke(INVOKE_CREATE_ARRAY, VAR);

            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "from", (INTEGER)VARIABLE_STRING, pszRemoteIP, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "timeout", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)1);
        }

        delete []pSendBuffer;
    }

    void *VAR;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "statistics", &VAR);
    Invoke(INVOKE_CREATE_ARRAY, VAR);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "from", (INTEGER)VARIABLE_STRING, pszRemoteIP, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "sent", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nPacketsSent);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "received", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nPacketsReceived);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "lost", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)(nPacketsSent - nPacketsReceived));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "statistics", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)((nPacketsSent - nPacketsReceived) / (float)nPacketsSent) * 100);
    //cout << endl << "Ping statistics for " << pszRemoteIP << ":" << endl << '\t' << "Packets: Sent = " << nPacketsSent << ", Received = " <<
    //	nPacketsReceived << ", Lost = " << (nPacketsSent - nPacketsReceived) << " (" <<
    //	((nPacketsSent - nPacketsReceived)/(float)nPacketsSent) * 100 << "% loss)" << endl << '\t';

    if (nPacketsReceived > 0) {
        //cout << "\rApproximate round trip times in milli-seconds:" << endl << '\t' << "Minimum = " << nMinRoundTripTime <<
        //	"ms, Maximum = " << nMaxRoundTripTime << "ms, Average = " << nTotalRoundTripTime / (float)nPacketsReceived << "ms" << endl;
    }

    //cout << '\r' << endl;
    return 0;
}

unsigned short CalcChecksum(char *pBuffer, int nLen) {
    //Checksum for ICMP is calculated in the same way as for
    //IP header

    //This code was taken from: http://www.netfor2.com/ipsum.htm

    unsigned short nWord;
    unsigned int   nSum = 0;
    int            i;

    //Make 16 bit words out of every two adjacent 8 bit words in the packet
    //and add them up
    for (i = 0; i < nLen; i = i + 2) {
        nWord = ((pBuffer [i] << 8) & 0xFF00) + (pBuffer [i + 1] & 0xFF);
        nSum  = nSum + (unsigned int)nWord;
    }

    //Take only 16 bits out of the 32 bit sum and add up the carries
    while (nSum >> 16) {
        nSum = (nSum & 0xFFFF) + (nSum >> 16);
    }

    //One's complement the result
    nSum = ~nSum;

    return (unsigned short)nSum;
}

int ValidateChecksum(char *pBuffer, int nLen) {
    unsigned short nWord;
    unsigned int   nSum = 0;
    int            i;

    //Make 16 bit words out of every two adjacent 8 bit words in the packet
    //and add them up
    for (i = 0; i < nLen; i = i + 2) {
        nWord = ((pBuffer [i] << 8) & 0xFF00) + (pBuffer [i + 1] & 0xFF);
        nSum  = nSum + (unsigned int)nWord;
    }

    //Take only 16 bits out of the 32 bit sum and add up the carries
    while (nSum >> 16) {
        nSum = (nSum & 0xFFFF) + (nSum >> 16);
    }

    //To validate the checksum on the received message we don't complement the sum
    //of one's complement
    //One's complement the result
    //nSum = ~nSum;

    //The sum of one's complement should be 0xFFFF
    return (unsigned short)nSum == 0xFFFF;
}

static char *inet_ntop4(const unsigned char *src, char *dst, size_t size) {
    static const char fmt[] = "%u.%u.%u.%u";
    char tmp[sizeof("255.255.255.255")];

    if ((size_t)sprintf(tmp, fmt, src[0], src[1], src[2], src[3]) >= size) {
        return NULL;
    }
    strcpy(dst, tmp);
    return dst;
}

int ResolveIP(char *pszRemoteHost, char *pszIPAddress) {
    hostent *pHostent = gethostbyname(pszRemoteHost);

    if (pHostent == NULL) {
        return 0;
    }

    in_addr in;
    memcpy(&in, pHostent->h_addr_list [0], sizeof(in_addr));
    pszIPAddress[0] = 0;
    inet_ntop4((unsigned char *)&in, pszIPAddress, 0xFF);

    return 1;
}
