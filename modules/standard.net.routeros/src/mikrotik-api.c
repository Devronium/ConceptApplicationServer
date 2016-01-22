#ifndef _WIN32

/********************************************************************
* Some definitions
* Word = piece of API code
* Sentence = multiple words
* Block = multiple sentences (usually in response to a sentence request)
*

        int fdSock;
        int iLoginResult;
        struct Sentence stSentence;
        struct Block stBlock;

        fdSock = apiConnect("10.0.0.1", 8728);

        // attempt login
        iLoginResult = login(fdSock, "admin", "adminPassword");

        if (!iLoginResult)
        {
                apiDisconnect(fdSock);
                printf("Invalid username or password.\n");
                exit(1);
        }

        // initialize, fill and send sentence to the API
        initializeSentence(&stSentence);
        addWordToSentence(&stSentence, "/interface/getall");
        writeSentence(fdSock, &stSentence);

        // receive and print block from the API
        stBlock = readBlock(fdSock);
        printBlock(&stBlock);

        apiDisconnect(fdSock);

********************************************************************/

 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <string.h>
 #include <stdlib.h>
 #include "md5.h"
 #include "mikrotik-api.h"



/********************************************************************
* Connect to API
* Returns a socket descriptor
********************************************************************/
int apiConnect(char *szIPaddr, int iPort) {
    int fdSock;
    struct sockaddr_in address;
    int iConnectResult;
    int iLen;

    fdSock = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(szIPaddr);
    address.sin_port        = htons(iPort);
    iLen = sizeof(address);

    DEBUG ? printf("Connecting to %s\n", szIPaddr) : 0;

    iConnectResult = connect(fdSock, (struct sockaddr *)&address, iLen);

    if (iConnectResult == -1) {
        //perror ("Connection problem");
        //exit(1);
        return iConnectResult;
    } else {
        DEBUG ? printf("Successfully connected to %s\n", szIPaddr) : 0;
    }

    // determine endianness of this machine
    // iLittleEndian will be set to 1 if we are
    // on a little endian machine...otherwise
    // we are assumed to be on a big endian processor
    iLittleEndian = isLittleEndian();

    return fdSock;
}

/********************************************************************
* Disconnect from API
* Close the API socket
********************************************************************/
void apiDisconnect(int fdSock) {
    DEBUG ? printf("Closing socket\n") : 0;

    close(fdSock);
}

/********************************************************************
* Login to the API
* 1 is returned on successful login
* 0 is returned on unsuccessful login
********************************************************************/
int login(int fdSock, char *username, char *password) {
    struct Sentence stReadSentence;
    struct Sentence stWriteSentence;
    char            *szMD5Challenge;
    char            *szMD5ChallengeBinary;
    char            *szMD5PasswordToSend;
    char            *szLoginUsernameResponseToSend;
    char            *szLoginPasswordResponseToSend;
    md5_state_t     state;
    md5_byte_t      digest[16];
    char            cNull[1] = { 0 };

    writeWord(fdSock, "/login");
    writeWord(fdSock, "");

    stReadSentence = readSentence(fdSock);
    DEBUG ? printSentence(&stReadSentence) : 0;

    if (stReadSentence.iReturnValue != DONE) {
        //printf("error.\n");
        //exit(0);
        return 0;
    }

    // extract md5 string from the challenge sentence
    szMD5Challenge = strtok(stReadSentence.szSentence[1], "=");
    szMD5Challenge = strtok(NULL, "=");

    DEBUG ? printf("MD5 of challenge = %s\n", szMD5Challenge) : 0;

    // convert szMD5Challenge to binary
    szMD5ChallengeBinary = md5ToBinary(szMD5Challenge);

    // get md5 of the password + challenge concatenation
    md5_init(&state);
    md5_append(&state, cNull, 1);
    md5_append(&state, (const md5_byte_t *)password, strlen(password));
    md5_append(&state, (const md5_byte_t *)szMD5ChallengeBinary, 16);
    md5_finish(&state, digest);

    // convert this digest to a string representation of the hex values
    // digest is the binary format of what we want to send
    // szMD5PasswordToSend is the "string" hex format
    szMD5PasswordToSend = md5DigestToHexString(digest);

    clearSentence(&stReadSentence);

    DEBUG ? printf("szPasswordToSend = %s\n", szMD5PasswordToSend) : 0;

    // put together the login sentence
    initializeSentence(&stWriteSentence);

    addWordToSentence(&stWriteSentence, "/login");
    addWordToSentence(&stWriteSentence, "=name=");
    addPartWordToSentence(&stWriteSentence, username);
    addWordToSentence(&stWriteSentence, "=response=00");
    addPartWordToSentence(&stWriteSentence, szMD5PasswordToSend);

    free(szMD5ChallengeBinary);
    free(szMD5PasswordToSend);

    DEBUG ? printSentence(&stWriteSentence) : 0;
    writeSentence(fdSock, &stWriteSentence);

    stReadSentence = readSentence(fdSock);
    DEBUG ? printSentence(&stReadSentence) : 0;

    if (stReadSentence.iReturnValue == DONE) {
        clearSentence(&stReadSentence);
        return 1;
    } else {
        clearSentence(&stReadSentence);
        return 0;
    }
}

/********************************************************************
* Encode message length and write it out to the socket
********************************************************************/
void writeLen(int fdSock, int iLen) {
    char *cEncodedLength;      // encoded length to send to the api socket
    char *cLength;             // exactly what is in memory at &iLen integer

    //cLength = calloc(sizeof(int), 1);
    //cEncodedLength = calloc(sizeof(int), 1);
    int cLength_container        = 0;
    int cEncodedLength_container = 0;

    cLength        = &cLength_container;        //calloc(sizeof(int), 1);
    cEncodedLength = &cEncodedLength_container; //calloc(sizeof(int), 1);


    // set cLength address to be same as iLen
    cLength = (char *)&iLen;

    DEBUG ? printf("length of word is %d\n", iLen) : 0;

    // write 1 byte
    if (iLen < 0x80) {
        cEncodedLength[0] = (char)iLen;
        write(fdSock, cEncodedLength, 1);
    }
    // write 2 bytes
    else if (iLen < 0x4000) {
        DEBUG ? printf("iLen < 0x4000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[1] | 0x80;
            cEncodedLength[1] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[2] | 0x80;
            cEncodedLength[1] = cLength[3];
        }

        write(fdSock, cEncodedLength, 2);
    }
    // write 3 bytes
    else if (iLen < 0x200000) {
        DEBUG ? printf("iLen < 0x200000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[2] | 0xc0;
            cEncodedLength[1] = cLength[1];
            cEncodedLength[2] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[1] | 0xc0;
            cEncodedLength[1] = cLength[2];
            cEncodedLength[2] = cLength[3];
        }

        write(fdSock, cEncodedLength, 3);
    }
    // write 4 bytes
    // this code SHOULD work, but is untested...
    else if (iLen < 0x10000000) {
        DEBUG ? printf("iLen < 0x10000000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[3] | 0xe0;
            cEncodedLength[1] = cLength[2];
            cEncodedLength[2] = cLength[1];
            cEncodedLength[3] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[0] | 0xe0;
            cEncodedLength[1] = cLength[1];
            cEncodedLength[2] = cLength[2];
            cEncodedLength[3] = cLength[3];
        }

        write(fdSock, cEncodedLength, 4);
    } else {  // this should never happen
        printf("length of word is %d\n", iLen);
        printf("word is too long.\n");
        //exit(1);
        return;
    }
}

/********************************************************************
* Write a word to the socket
********************************************************************/
void writeWord(int fdSock, char *szWord) {
    DEBUG ? printf("Word to write is %s\n", szWord) : 0;
    writeLen(fdSock, strlen(szWord));
    write(fdSock, szWord, strlen(szWord));
}

/********************************************************************
* Write a sentence (multiple words) to the socket
********************************************************************/
void writeSentence(int fdSock, struct Sentence *stWriteSentence) {
    int iIndex;

    if (stWriteSentence->iLength == 0) {
        return;
    }

    DEBUG ? printf("Writing sentence\n") : 0;
    DEBUG ? printSentence(stWriteSentence) : 0;

    for (iIndex = 0; iIndex < stWriteSentence->iLength; iIndex++) {
        writeWord(fdSock, stWriteSentence->szSentence[iIndex]);
    }

    writeWord(fdSock, "");
}

/********************************************************************
* Read a message length from the socket
*
* 80 = 10000000 (2 character encoded length)
* C0 = 11000000 (3 character encoded length)
* E0 = 11100000 (4 character encoded length)
*
* Message length is returned
********************************************************************/

/*int readLen(int fdSock)
   {
        char cFirstChar; // first character read from socket
        char *cLength;   // length of next message to read...will be cast to int at the end
        int *iLen;       // calculated length of next message (Cast to int)

        cLength = calloc(sizeof(int), 1);

        DEBUG ? printf("start readLen()\n") : 0;

        read(fdSock, &cFirstChar, 1);

        DEBUG ? printf("byte1 = %#x\n", cFirstChar) : 0;

        // read 4 bytes
        // this code SHOULD work, but is untested...
        if ((cFirstChar & 0xE0) == 0xE0)
        {
                DEBUG ? printf("4-byte encoded length\n") : 0;

                if (iLittleEndian)
                {
                        cLength[3] = cFirstChar;
                        cLength[3] &= 0x1f;        // mask out the 1st 3 bits
                        read(fdSock, &cLength[2], 1);
                        read(fdSock, &cLength[1], 1);
                        read(fdSock, &cLength[0], 1);
                }
                else
                {
                        cLength[0] = cFirstChar;
                        cLength[0] &= 0x1f;        // mask out the 1st 3 bits
                        read(fdSock, &cLength[1], 1);
                        read(fdSock, &cLength[2], 1);
                        read(fdSock, &cLength[3], 1);
                }

                iLen = (int *)cLength;
        }

        // read 3 bytes
        else if ((cFirstChar & 0xC0) == 0xC0)
        {
                DEBUG ? printf("3-byte encoded length\n") : 0;

                if (iLittleEndian)
                {
                        cLength[2] = cFirstChar;
                        cLength[2] &= 0x3f;        // mask out the 1st 2 bits
                        read(fdSock, &cLength[1], 1);
                        read(fdSock, &cLength[0], 1);
                }
                else
                {
                        cLength[1] = cFirstChar;
                        cLength[1] &= 0x3f;        // mask out the 1st 2 bits
                        read(fdSock, &cLength[2], 1);
                        read(fdSock, &cLength[3], 1);
                }

                iLen = (int *)cLength;
        }

        // read 2 bytes
        else if ((cFirstChar & 0x80) == 0x80)
        {
                DEBUG ? printf("2-byte encoded length\n") : 0;

                if (iLittleEndian)
                {
                        cLength[1] = cFirstChar;
                        cLength[1] &= 0x7f;        // mask out the 1st bit
                        read(fdSock, &cLength[0], 1);
                }
                else
                {
                        cLength[2] = cFirstChar;
                        cLength[2] &= 0x7f;        // mask out the 1st bit
                        read(fdSock, &cLength[3], 1);
                }

                iLen = (int *)cLength;
        }

        // assume 1-byte encoded length...same on both LE and BE systems
        else
        {
                DEBUG ? printf("1-byte encoded length\n") : 0;
                iLen = malloc(sizeof(int));
 * iLen = (int)cFirstChar;
        }

        return *iLen;
   }
 */



/********************************************************************
* Read a word from the socket
* The word that was read is returned as a string
********************************************************************/

/*char *readWord(int fdSock)
   {
        int iLen = readLen(fdSock);
        int iBytesToRead = 0;
        int iBytesRead = 0;
        char *szWord;
        char *szRetWord;
        char *szTmpWord;

        DEBUG ? printf("readWord iLen=%x\n", iLen) : 0;

        if (iLen > 0)
        {
                // allocate memory for strings
                szRetWord = calloc(sizeof(char), iLen + 1);
                szTmpWord = calloc(sizeof(char), 1024 + 1);

                while (iLen != 0)
                {
                        // determine number of bytes to read this time around
                        // lesser of 1024 or the number of byes left to read
                        // in this word
                        iBytesToRead = iLen > 1024 ? 1024 : iLen;

                        // read iBytesToRead from the socket
                        iBytesRead = read(fdSock, szTmpWord, iBytesToRead);
            if (iBytesRead <= 0)
                return 0;

                        // terminate szTmpWord
                        szTmpWord[iBytesRead] = 0;

                        // concatenate szTmpWord to szRetWord
                        strcat(szRetWord, szTmpWord);

                        // subtract the number of bytes we just read from iLen
                        iLen -= iBytesRead;
                }

                // deallocate szTmpWord
                free(szTmpWord);

                DEBUG ? printf("word = %s\n", szRetWord) : 0;
                return szRetWord;
        }
        else
        {
                return NULL;
        }
   }
 */


/********************************************************************
* Read a sentence from the socket
* A Sentence struct is returned
********************************************************************/

/*struct Sentence readSentence(int fdSock)
   {
        struct Sentence stReturnSentence;
        char *szWord;
        int i=0;
        int iReturnLength=0;

        DEBUG ? printf("readSentence\n") : 0;

        initializeSentence(&stReturnSentence);

        while (szWord = readWord(fdSock))
        {
                addWordToSentence(&stReturnSentence, szWord);

                // check to see if we can get a return value from the API
                if (strstr(szWord, "!done") != NULL)
                {
                        DEBUG ? printf("return sentence contains !done\n") : 0;
                        stReturnSentence.iReturnValue = DONE;
                }
                else if (strstr(szWord, "!trap") != NULL)
                {
                        DEBUG ? printf("return sentence contains !trap\n") : 0;
                        stReturnSentence.iReturnValue = TRAP;
                }
                else if (strstr(szWord, "!fatal") != NULL)
                {
                        DEBUG ? printf("return sentence contains !fatal\n") : 0;
                        stReturnSentence.iReturnValue = FATAL;
                }

        }

        // if any errors, get the next sentence
        if (stReturnSentence.iReturnValue == TRAP || stReturnSentence.iReturnValue == FATAL)
        {
                readSentence(fdSock);
        }

        if (DEBUG)
        {
                for (i=0; i<stReturnSentence.iLength; i++)
                {
                        printf("stReturnSentence.szSentence[%d] = %s\n", i, stReturnSentence.szSentence[i]);
                }
        }

        return stReturnSentence;
   }
 */


/********************************************************************
* Read sentence block from the socket...keeps reading sentences
* until it encounters !done, !trap or !fatal from the socket
********************************************************************/
struct Block readBlock(int fdSock) {
    struct Sentence stSentence;
    struct Block    stBlock;

    initializeBlock(&stBlock);

    DEBUG ? printf("readBlock\n") : 0;

    do {
        stSentence = readSentence(fdSock);
        DEBUG ? printf("readSentence succeeded.\n") : 0;

        addSentenceToBlock(&stBlock, &stSentence);
        DEBUG ? printf("addSentenceToBlock succeeded\n") : 0;
    } while (stSentence.iReturnValue == 0);


    DEBUG ? printf("readBlock completed successfully\n") : 0;

    return stBlock;
}

/********************************************************************
* Initialize a new block
* Set iLength to 0.
********************************************************************/
void initializeBlock(struct Block *stBlock) {
    DEBUG ? printf("initializeBlock\n") : 0;

    stBlock->iLength = 0;
}

/********************************************************************
* Print a block.
* Output a Block with printf.
********************************************************************/
void printBlock(struct Block *stBlock) {
    int i;

    DEBUG ? printf("printBlock\n") : 0;
    DEBUG ? printf("block iLength = %d\n", stBlock->iLength) : 0;

    for (i = 0; i < stBlock->iLength; i++) {
        printSentence(stBlock->stSentence[i]);
    }
}

/********************************************************************
* Add a sentence to a block
* Allocate memory and add a sentence to a Block.
********************************************************************/
void addSentenceToBlock(struct Block *stBlock, struct Sentence *stSentence) {
    int iNewLength;

    iNewLength = stBlock->iLength + 1;

    DEBUG ? printf("addSentenceToBlock iNewLength=%d\n", iNewLength) : 0;

    // allocate mem for the new Sentence position
    if (stBlock->iLength == 0) {
        stBlock->stSentence = malloc(1 * sizeof stBlock->stSentence);
    } else {
        stBlock->stSentence = realloc(stBlock->stSentence, iNewLength * sizeof stBlock->stSentence + 1);
    }


    // allocate mem for the full sentence struct
    stBlock->stSentence[stBlock->iLength] = malloc(sizeof *stSentence);

    // copy actual sentence struct to the block position
    memcpy(stBlock->stSentence[stBlock->iLength], stSentence, sizeof *stSentence);

    // update iLength
    stBlock->iLength = iNewLength;

    DEBUG ? printf("addSentenceToBlock stBlock->iLength=%d\n", stBlock->iLength) : 0;
}

/********************************************************************
* Initialize a new sentence
********************************************************************/
void initializeSentence(struct Sentence *stSentence) {
    DEBUG ? printf("initializeSentence\n") : 0;

    stSentence->iLength      = 0;
    stSentence->iReturnValue = 0;
}

/********************************************************************
* Add a word to a sentence struct
********************************************************************/
void addWordToSentence(struct Sentence *stSentence, char *szWordToAdd) {
    int iNewLength;

    iNewLength = stSentence->iLength + 1;

    // allocate mem for the new word position
    if (stSentence->iLength == 0) {
        stSentence->szSentence = malloc(1 * sizeof stSentence->szSentence);
    } else {
        stSentence->szSentence = realloc(stSentence->szSentence, iNewLength * sizeof stSentence->szSentence + 1);
    }


    // allocate mem for the full word string
    stSentence->szSentence[stSentence->iLength] = malloc(strlen(szWordToAdd) + 1);

    // copy word string to the sentence
    strcpy(stSentence->szSentence[stSentence->iLength], szWordToAdd);

    // update iLength
    stSentence->iLength = iNewLength;
}

/********************************************************************
* Add a partial word to a sentence struct...useful for concatenation
********************************************************************/
void addPartWordToSentence(struct Sentence *stSentence, char *szWordToAdd) {
    int iIndex;

    iIndex = stSentence->iLength - 1;

    // reallocate memory for the new partial word
    stSentence->szSentence[iIndex] = realloc(stSentence->szSentence[iIndex], strlen(stSentence->szSentence[iIndex]) + strlen(szWordToAdd) + 1);

    // concatenate the partial word to the existing sentence
    strcat(stSentence->szSentence[iIndex], szWordToAdd);
}

/********************************************************************
* Print a Sentence struct
********************************************************************/
void printSentence(struct Sentence *stSentence) {
    int i;

    DEBUG ? printf("Sentence iLength = %d\n", stSentence->iLength) : 0;
    DEBUG ? printf("Sentence iReturnValue = %d\n", stSentence->iReturnValue) : 0;

    printf("Sentence iLength = %d\n", stSentence->iLength);
    printf("Sentence iReturnValue = %d\n", stSentence->iReturnValue);

    for (i = 0; i < stSentence->iLength; i++) {
        printf(">>> %s\n", stSentence->szSentence[i]);
    }

    printf("\n");
}

/********************************************************************
* MD5 helper function to convert an md5 hex char representation to
* binary representation.
********************************************************************/
char *md5ToBinary(char *szHex) {
    int  di;
    char cBinWork[3];
    char *szReturn;

    // allocate 16 + 1 bytes for our return string
    szReturn = malloc((16 + 1) * sizeof *szReturn);

    // 32 bytes in szHex?
    if (strlen(szHex) != 32) {
        return NULL;
    }

    for (di = 0; di < 32; di += 2) {
        cBinWork[0] = szHex[di];
        cBinWork[1] = szHex[di + 1];
        cBinWork[2] = 0;

        DEBUG ? printf("cBinWork = %s\n", cBinWork) : 0;

        szReturn[di / 2] = hexStringToChar(cBinWork);
    }

    return szReturn;
}

/********************************************************************
* MD5 helper function to calculate and return hex representation
* of an MD5 digest stored in binary.
********************************************************************/
char *md5DigestToHexString(md5_byte_t *binaryDigest) {
    int  di;
    char *szReturn;

    // allocate 32 + 1 bytes for our return string
    szReturn = malloc((32 + 1) * sizeof *szReturn);


    for (di = 0; di < 16; ++di) {
        sprintf(szReturn + di * 2, "%02x", binaryDigest[di]);
    }

    return szReturn;
}

/********************************************************************
* Quick and dirty function to convert hex string to char...
* the toConvert string MUST BE 2 characters + null terminated.
********************************************************************/
char hexStringToChar(char *cToConvert) {
    char         cConverted;
    unsigned int iAccumulated = 0;
    char         cString0[2]  = { cToConvert[0], 0 };
    char         cString1[2]  = { cToConvert[1], 0 };

    // look @ first char in the 16^1 place
    if ((cToConvert[0] == 'f') || (cToConvert[0] == 'F')) {
        iAccumulated += 16 * 15;
    } else if ((cToConvert[0] == 'e') || (cToConvert[0] == 'E')) {
        iAccumulated += 16 * 14;
    } else if ((cToConvert[0] == 'd') || (cToConvert[0] == 'D')) {
        iAccumulated += 16 * 13;
    } else if ((cToConvert[0] == 'c') || (cToConvert[0] == 'C')) {
        iAccumulated += 16 * 12;
    } else if ((cToConvert[0] == 'b') || (cToConvert[0] == 'B')) {
        iAccumulated += 16 * 11;
    } else if ((cToConvert[0] == 'a') || (cToConvert[0] == 'A')) {
        iAccumulated += 16 * 10;
    } else {
        iAccumulated += 16 * atoi(cString0);
    }


    // now look @ the second car in the 16^0 place
    if ((cToConvert[1] == 'f') || (cToConvert[1] == 'F')) {
        iAccumulated += 15;
    } else if ((cToConvert[1] == 'e') || (cToConvert[1] == 'E')) {
        iAccumulated += 14;
    } else if ((cToConvert[1] == 'd') || (cToConvert[1] == 'D')) {
        iAccumulated += 13;
    } else if ((cToConvert[1] == 'c') || (cToConvert[1] == 'C')) {
        iAccumulated += 12;
    } else if ((cToConvert[1] == 'b') || (cToConvert[1] == 'B')) {
        iAccumulated += 11;
    } else if ((cToConvert[1] == 'a') || (cToConvert[1] == 'A')) {
        iAccumulated += 10;
    } else {
        iAccumulated += atoi(cString1);
    }

    DEBUG ? printf("%d\n", iAccumulated) : 0;
    return (char)iAccumulated;
}

/********************************************************************
* Test whether or not this system is little endian at RUNTIME
* Courtesy: http://download.osgeo.org/grass/grass6_progman/endian_8c_source.html
********************************************************************/
int isLittleEndian(void) {
    union {
        int  testWord;
        char testByte[sizeof(int)];
    } endianTest;

    endianTest.testWord = 1;

    if (endianTest.testByte[0] == 1)
        return 1;               /* true: little endian */

    return 0;                   /* false: big endian */
}

#else

/********************************************************************
* Some definitions
* Word = piece of API code
* Sentence = multiple words
* Block = multiple sentences (usually in response to a sentence request)
*

        int fdSock;
        int iLoginResult;
        struct Sentence stSentence;
        struct Block stBlock;

        fdSock = apiConnect("10.0.0.1", 8728);

        // attempt login
        iLoginResult = login(fdSock, "admin", "adminPassword");

        if (!iLoginResult)
        {
                apiDisconnect(fdSock);
                printf("Invalid username or password.\n");
                exit(1);
        }

        // initialize, fill and send sentence to the API
        initializeSentence(&stSentence);
        addWordToSentence(&stSentence, "/interface/getall");
        writeSentence(fdSock, &stSentence);

        // receive and print block from the API
        stBlock = readBlock(fdSock);
        printBlock(&stBlock);

        apiDisconnect(fdSock);

********************************************************************/

// C implementation of Mikrotik's API rewritten for Winsock2 (for windows)
// Updated 28 August 2011 by hel

 #include <stdio.h>
 #include <winsock2.h>
 #pragma comment(lib, "ws2_32.lib")
 #include <windows.h>
 #include <string.h>
 #include <stdlib.h>
 #include "md5.h"
 #include "mikrotik-api.h"


/********************************************************************
* Connect to API
* Returns a socket descriptor
********************************************************************/
int apiConnect(char *szIPaddr, int iPort) {
    int fdSock;
    struct sockaddr_in address;
    int iConnectResult;
    int iLen;

    WORD    versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;

    WSAStartup(versionWanted, &wsaData);

    fdSock = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(szIPaddr);
    address.sin_port        = htons(iPort);
    iLen = sizeof(address);

    DEBUG ? printf("Connecting to %s\n", szIPaddr) : 0;

    iConnectResult = connect(fdSock, (struct sockaddr *)&address, iLen);

    if (iConnectResult == -1) {
        //perror ("Connection problem");
        //exit(1);
        return iConnectResult;
    } else {
        DEBUG ? printf("Successfully connected to %s\n", szIPaddr) : 0;
    }

    // determine endianness of this machine
    // iLittleEndian will be set to 1 if we are
    // on a little endian machine...otherwise
    // we are assumed to be on a big endian processor
    iLittleEndian = isLittleEndian();

    return fdSock;
}

/********************************************************************
* Disconnect from API
* Close the API socket
********************************************************************/
void apiDisconnect(int fdSock) {
    DEBUG ? printf("Closing socket\n") : 0;

    closesocket(fdSock);
}

/********************************************************************
* Login to the API
* 1 is returned on successful login
* 0 is returned on unsuccessful login
********************************************************************/
int login(int fdSock, char *username, char *password) {
    struct Sentence stReadSentence;
    struct Sentence stWriteSentence;
    char            *szMD5Challenge;
    char            *szMD5ChallengeBinary;
    char            *szMD5PasswordToSend;
    char            *szLoginUsernameResponseToSend;
    char            *szLoginPasswordResponseToSend;
    md5_state_t     state;
    md5_byte_t      digest[16];
    char            cNull[1] = { 0 };


    writeWord(fdSock, "/login");
    writeWord(fdSock, "");

    stReadSentence = readSentence(fdSock);
    DEBUG ? printSentence(&stReadSentence) : 0;

    if (stReadSentence.iReturnValue != DONE) {
        //printf("error.\n");
        //exit(0);
        return 0;
    }

    // extract md5 string from the challenge sentence
    szMD5Challenge = strtok(stReadSentence.szSentence[1], "=");
    szMD5Challenge = strtok(NULL, "=");

    DEBUG ? printf("MD5 of challenge = %s\n", szMD5Challenge) : 0;

    // convert szMD5Challenge to binary
    szMD5ChallengeBinary = md5ToBinary(szMD5Challenge);

    // get md5 of the password + challenge concatenation
    md5_init(&state);
    md5_append(&state, cNull, 1);
    md5_append(&state, (const md5_byte_t *)password, strlen(password));
    md5_append(&state, (const md5_byte_t *)szMD5ChallengeBinary, 16);
    md5_finish(&state, digest);

    // convert this digest to a string representation of the hex values
    // digest is the binary format of what we want to send
    // szMD5PasswordToSend is the "string" hex format
    szMD5PasswordToSend = md5DigestToHexString(digest);

    DEBUG ? printf("szPasswordToSend = %s\n", szMD5PasswordToSend) : 0;

    // put together the login sentence
    initializeSentence(&stWriteSentence);

    addWordToSentence(&stWriteSentence, "/login");
    addWordToSentence(&stWriteSentence, "=name=");
    addPartWordToSentence(&stWriteSentence, username);
    addWordToSentence(&stWriteSentence, "=response=00");
    addPartWordToSentence(&stWriteSentence, szMD5PasswordToSend);

    free(szMD5ChallengeBinary);
    free(szMD5PasswordToSend);
    DEBUG ? printSentence(&stWriteSentence) : 0;
    writeSentence(fdSock, &stWriteSentence);


    stReadSentence = readSentence(fdSock);
    DEBUG ? printSentence(&stReadSentence) : 0;

    if (stReadSentence.iReturnValue == DONE) {
        return 1;
    } else {
        return 0;
    }
}

/********************************************************************
* Encode message length and write it out to the socket
********************************************************************/
void writeLen(int fdSock, int iLen) {
    char *cEncodedLength;      // encoded length to send to the api socket
    char *cLength;             // exactly what is in memory at &iLen integer

    int cLength_container        = 0;
    int cEncodedLength_container = 0;

    cLength        = &cLength_container;        //calloc(sizeof(int), 1);
    cEncodedLength = &cEncodedLength_container; //calloc(sizeof(int), 1);

    // set cLength address to be same as iLen
    cLength = (char *)&iLen;

    DEBUG ? printf("length of word is %d\n", iLen) : 0;

    // write 1 byte
    if (iLen < 0x80) {
        cEncodedLength[0] = (char)iLen;
        send(fdSock, cEncodedLength, 1, 0);
    }
    // write 2 bytes
    else if (iLen < 0x4000) {
        DEBUG ? printf("iLen < 0x4000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[1] | 0x80;
            cEncodedLength[1] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[2] | 0x80;
            cEncodedLength[1] = cLength[3];
        }

        send(fdSock, cEncodedLength, 2, 0);
    }
    // write 3 bytes
    else if (iLen < 0x200000) {
        DEBUG ? printf("iLen < 0x200000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[2] | 0xc0;
            cEncodedLength[1] = cLength[1];
            cEncodedLength[2] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[1] | 0xc0;
            cEncodedLength[1] = cLength[2];
            cEncodedLength[2] = cLength[3];
        }

        send(fdSock, cEncodedLength, 3, 0);
    }
    // write 4 bytes
    // this code SHOULD work, but is untested...
    else if (iLen < 0x10000000) {
        DEBUG ? printf("iLen < 0x10000000.\n") : 0;

        if (iLittleEndian) {
            cEncodedLength[0] = cLength[3] | 0xe0;
            cEncodedLength[1] = cLength[2];
            cEncodedLength[2] = cLength[1];
            cEncodedLength[3] = cLength[0];
        } else {
            cEncodedLength[0] = cLength[0] | 0xe0;
            cEncodedLength[1] = cLength[1];
            cEncodedLength[2] = cLength[2];
            cEncodedLength[3] = cLength[3];
        }

        send(fdSock, cEncodedLength, 4, 0);
    } else {  // this should never happen
        printf("length of word is %d\n", iLen);
        printf("word is too long.\n");
        //exit(1);
        return;
    }
}

/********************************************************************
* Write a word to the socket
********************************************************************/
void writeWord(int fdSock, char *szWord) {
    DEBUG ? printf("Word to write is %s\n", szWord) : 0;
    writeLen(fdSock, strlen(szWord));
    send(fdSock, szWord, strlen(szWord), 0);
}

/********************************************************************
* Write a sentence (multiple words) to the socket
********************************************************************/
void writeSentence(int fdSock, struct Sentence *stWriteSentence) {
    int iIndex;

    if (stWriteSentence->iLength == 0) {
        return;
    }

    DEBUG ? printf("Writing sentence\n") : 0;
    DEBUG ? printSentence(stWriteSentence) : 0;

    for (iIndex = 0; iIndex < stWriteSentence->iLength; iIndex++) {
        writeWord(fdSock, stWriteSentence->szSentence[iIndex]);
    }

    writeWord(fdSock, "");
}

/********************************************************************
* Read sentence block from the socket...keeps reading sentences
* until it encounters !done, !trap or !fatal from the socket
********************************************************************/
struct Block readBlock(int fdSock) {
    struct Sentence stSentence;
    struct Block    stBlock;

    initializeBlock(&stBlock);

    DEBUG ? printf("readBlock\n") : 0;

    do {
        stSentence = readSentence(fdSock);
        DEBUG ? printf("readSentence succeeded.\n") : 0;

        addSentenceToBlock(&stBlock, &stSentence);
        DEBUG ? printf("addSentenceToBlock succeeded\n") : 0;
    } while (stSentence.iReturnValue == 0);


    DEBUG ? printf("readBlock completed successfully\n") : 0;

    return stBlock;
}

/********************************************************************
* Initialize a new block
* Set iLength to 0.
********************************************************************/
void initializeBlock(struct Block *stBlock) {
    DEBUG ? printf("initializeBlock\n") : 0;

    stBlock->iLength = 0;
}

/********************************************************************
* Print a block.
* Output a Block with printf.
********************************************************************/
void printBlock(struct Block *stBlock) {
    int i;

    DEBUG ? printf("printBlock\n") : 0;
    DEBUG ? printf("block iLength = %d\n", stBlock->iLength) : 0;

    for (i = 0; i < stBlock->iLength; i++) {
        printSentence(stBlock->stSentence[i]);
    }
}

/********************************************************************
* Add a sentence to a block
* Allocate memory and add a sentence to a Block.
********************************************************************/
void addSentenceToBlock(struct Block *stBlock, struct Sentence *stSentence) {
    int iNewLength;

    iNewLength = stBlock->iLength + 1;

    DEBUG ? printf("addSentenceToBlock iNewLength=%d\n", iNewLength) : 0;

    // allocate mem for the new Sentence position
    if (stBlock->iLength == 0) {
        stBlock->stSentence = malloc(1 * sizeof stBlock->stSentence);
    } else {
        stBlock->stSentence = realloc(stBlock->stSentence, iNewLength * sizeof stBlock->stSentence + 1);
    }


    // allocate mem for the full sentence struct
    stBlock->stSentence[stBlock->iLength] = malloc(sizeof *stSentence);

    // copy actual sentence struct to the block position
    memcpy(stBlock->stSentence[stBlock->iLength], stSentence, sizeof *stSentence);

    // update iLength
    stBlock->iLength = iNewLength;

    DEBUG ? printf("addSentenceToBlock stBlock->iLength=%d\n", stBlock->iLength) : 0;
}

/********************************************************************
* Initialize a new sentence
********************************************************************/
void initializeSentence(struct Sentence *stSentence) {
    DEBUG ? printf("initializeSentence\n") : 0;

    stSentence->iLength      = 0;
    stSentence->iReturnValue = 0;
}

/********************************************************************
* Add a word to a sentence struct
********************************************************************/
void addWordToSentence(struct Sentence *stSentence, char *szWordToAdd) {
    int iNewLength;

    iNewLength = stSentence->iLength + 1;

    // allocate mem for the new word position
    if (stSentence->iLength == 0) {
        stSentence->szSentence = malloc(1 * sizeof stSentence->szSentence);
    } else {
        stSentence->szSentence = realloc(stSentence->szSentence, iNewLength * sizeof stSentence->szSentence + 1);
    }


    // allocate mem for the full word string
    stSentence->szSentence[stSentence->iLength] = malloc(strlen(szWordToAdd) + 1);

    // copy word string to the sentence
    strcpy(stSentence->szSentence[stSentence->iLength], szWordToAdd);

    // update iLength
    stSentence->iLength = iNewLength;
}

/********************************************************************
* Add a partial word to a sentence struct...useful for concatenation
********************************************************************/
void addPartWordToSentence(struct Sentence *stSentence, char *szWordToAdd) {
    int iIndex;

    iIndex = stSentence->iLength - 1;

    // reallocate memory for the new partial word
    stSentence->szSentence[iIndex] = realloc(stSentence->szSentence[iIndex], strlen(stSentence->szSentence[iIndex]) + strlen(szWordToAdd) + 1);

    // concatenate the partial word to the existing sentence
    strcat(stSentence->szSentence[iIndex], szWordToAdd);
}

/********************************************************************
* Print a Sentence struct
********************************************************************/
void printSentence(struct Sentence *stSentence) {
    int i;

    DEBUG ? printf("Sentence iLength = %d\n", stSentence->iLength) : 0;
    DEBUG ? printf("Sentence iReturnValue = %d\n", stSentence->iReturnValue) : 0;

    printf("Sentence iLength = %d\n", stSentence->iLength);
    printf("Sentence iReturnValue = %d\n", stSentence->iReturnValue);

    for (i = 0; i < stSentence->iLength; i++) {
        printf(">>> %s\n", stSentence->szSentence[i]);
    }

    printf("\n");
}

/********************************************************************
* MD5 helper function to convert an md5 hex char representation to
* binary representation.
********************************************************************/
char *md5ToBinary(char *szHex) {
    int  di;
    char cBinWork[3];
    char *szReturn;

    // allocate 16 + 1 bytes for our return string
    szReturn = malloc((16 + 1) * sizeof *szReturn);

    // 32 bytes in szHex?
    if (strlen(szHex) != 32) {
        return NULL;
    }

    for (di = 0; di < 32; di += 2) {
        cBinWork[0] = szHex[di];
        cBinWork[1] = szHex[di + 1];
        cBinWork[2] = 0;

        DEBUG ? printf("cBinWork = %s\n", cBinWork) : 0;

        szReturn[di / 2] = hexStringToChar(cBinWork);
    }

    return szReturn;
}

/********************************************************************
* MD5 helper function to calculate and return hex representation
* of an MD5 digest stored in binary.
********************************************************************/
char *md5DigestToHexString(md5_byte_t *binaryDigest) {
    int  di;
    char *szReturn;

    // allocate 32 + 1 bytes for our return string
    szReturn = malloc((32 + 1) * sizeof *szReturn);


    for (di = 0; di < 16; ++di) {
        sprintf(szReturn + di * 2, "%02x", binaryDigest[di]);
    }

    return szReturn;
}

/********************************************************************
* Quick and dirty function to convert hex string to char...
* the toConvert string MUST BE 2 characters + null terminated.
********************************************************************/
char hexStringToChar(char *cToConvert) {
    char         cConverted;
    unsigned int iAccumulated = 0;
    char         cString0[2]  = { cToConvert[0], 0 };
    char         cString1[2]  = { cToConvert[1], 0 };

    // look @ first char in the 16^1 place
    if ((cToConvert[0] == 'f') || (cToConvert[0] == 'F')) {
        iAccumulated += 16 * 15;
    } else if ((cToConvert[0] == 'e') || (cToConvert[0] == 'E')) {
        iAccumulated += 16 * 14;
    } else if ((cToConvert[0] == 'd') || (cToConvert[0] == 'D')) {
        iAccumulated += 16 * 13;
    } else if ((cToConvert[0] == 'c') || (cToConvert[0] == 'C')) {
        iAccumulated += 16 * 12;
    } else if ((cToConvert[0] == 'b') || (cToConvert[0] == 'B')) {
        iAccumulated += 16 * 11;
    } else if ((cToConvert[0] == 'a') || (cToConvert[0] == 'A')) {
        iAccumulated += 16 * 10;
    } else {
        iAccumulated += 16 * atoi(cString0);
    }


    // now look @ the second car in the 16^0 place
    if ((cToConvert[1] == 'f') || (cToConvert[1] == 'F')) {
        iAccumulated += 15;
    } else if ((cToConvert[1] == 'e') || (cToConvert[1] == 'E')) {
        iAccumulated += 14;
    } else if ((cToConvert[1] == 'd') || (cToConvert[1] == 'D')) {
        iAccumulated += 13;
    } else if ((cToConvert[1] == 'c') || (cToConvert[1] == 'C')) {
        iAccumulated += 12;
    } else if ((cToConvert[1] == 'b') || (cToConvert[1] == 'B')) {
        iAccumulated += 11;
    } else if ((cToConvert[1] == 'a') || (cToConvert[1] == 'A')) {
        iAccumulated += 10;
    } else {
        iAccumulated += atoi(cString1);
    }

    DEBUG ? printf("%d\n", iAccumulated) : 0;
    return (char)iAccumulated;
}

/********************************************************************
* Test whether or not this system is little endian at RUNTIME
* Courtesy: http://download.osgeo.org/grass/grass6_progman/endian_8c_source.html
********************************************************************/
int isLittleEndian(void) {
    union {
        int  testWord;
        char testByte[sizeof(int)];
    } endianTest;

    endianTest.testWord = 1;

    if (endianTest.testByte[0] == 1)
        return 1;               /* true: little endian */

    return 0;                   /* false: big endian */
}
#endif

/********************************************************************
* Clear an existing sentence
********************************************************************/
void clearSentence(struct Sentence *stSentence) {
    int i;

    DEBUG ? printf("initializeSentence\n") : 0;
    for (i = 0; i < stSentence->iLength; i++) {
        if (stSentence->szSentence[i])
            free(stSentence->szSentence[i]);
    }

    free(stSentence->szSentence);
    initializeSentence(stSentence);
}

/********************************************************************
* Clear an existing block
* Free all sentences in the Block struct and set iLength to 0.
********************************************************************/
void clearBlock(struct Block *stBlock) {
    int i;

    DEBUG ? printf("clearBlock\n") : 0;
    for (i = 0; i < stBlock->iLength; i++) {
        if (stBlock->stSentence[i]) {
            clearSentence(stBlock->stSentence[i]);
            free(stBlock->stSentence[i]);
        }
    }
    free(stBlock->stSentence);
    initializeBlock(&stBlock);
}

/********************************************************************
* Read a message length from the socket
*
* 80 = 10000000 (2 character encoded length)
* C0 = 11000000 (3 character encoded length)
* E0 = 11100000 (4 character encoded length)
*
* Message length is returned
********************************************************************/
int readLen(int fdSock) {
    char cFirstChar;     // first character read from socket
    char *cLength;       // length of next message to read...will be cast to int at the end
    int  *iLen;          // calculated length of next message (Cast to int)
    int  res;
    int  c_buf;

    cLength = &c_buf;    //calloc(sizeof(int), 1);

    DEBUG ? printf("start readLen()\n") : 0;

    recv(fdSock, &cFirstChar, 1, 0);

    DEBUG ? printf("byte1 = %#x\n", cFirstChar) : 0;

    // read 4 bytes
    // this code SHOULD work, but is untested...
    if ((cFirstChar & 0xE0) == 0xE0) {
        DEBUG ? printf("4-byte encoded length\n") : 0;

        if (iLittleEndian) {
            cLength[3]  = cFirstChar;
            cLength[3] &= 0x1f;                    // mask out the 1st 3 bits
            recv(fdSock, &cLength[2], 1, 0);
            recv(fdSock, &cLength[1], 1, 0);
            recv(fdSock, &cLength[0], 1, 0);
        } else {
            cLength[0]  = cFirstChar;
            cLength[0] &= 0x1f;                    // mask out the 1st 3 bits
            recv(fdSock, &cLength[1], 1, 0);
            recv(fdSock, &cLength[2], 1, 0);
            recv(fdSock, &cLength[3], 1, 0);
        }

        iLen = (int *)cLength;
    }
    // read 3 bytes
    else if ((cFirstChar & 0xC0) == 0xC0) {
        DEBUG ? printf("3-byte encoded length\n") : 0;

        if (iLittleEndian) {
            cLength[2]  = cFirstChar;
            cLength[2] &= 0x3f;                    // mask out the 1st 2 bits
            recv(fdSock, &cLength[1], 1, 0);
            recv(fdSock, &cLength[0], 1, 0);
        } else {
            cLength[1]  = cFirstChar;
            cLength[1] &= 0x3f;                    // mask out the 1st 2 bits
            recv(fdSock, &cLength[2], 1, 0);
            recv(fdSock, &cLength[3], 1, 0);
        }

        iLen = (int *)cLength;
    }
    // read 2 bytes
    else if ((cFirstChar & 0x80) == 0x80) {
        DEBUG ? printf("2-byte encoded length\n") : 0;

        if (iLittleEndian) {
            cLength[1]  = cFirstChar;
            cLength[1] &= 0x7f;                    // mask out the 1st bit
            recv(fdSock, &cLength[0], 1, 0);
        } else {
            cLength[2]  = cFirstChar;
            cLength[2] &= 0x7f;                    // mask out the 1st bit
            recv(fdSock, &cLength[3], 1, 0);
        }

        iLen = (int *)cLength;
    }
    // assume 1-byte encoded length...same on both LE and BE systems
    else {
        DEBUG ? printf("1-byte encoded length\n") : 0;
        return (int)cFirstChar;
        //iLen = malloc(sizeof(int));
        //*iLen = (int)cFirstChar;
    }

    return *iLen;
}

/********************************************************************
* Read a word from the socket
* The word that was read is returned as a string
********************************************************************/
char *readWord(int fdSock) {
    int  iLen         = readLen(fdSock);
    int  iBytesToRead = 0;
    int  iBytesRead   = 0;
    char *szWord;
    char *szRetWord;
    char *szTmpWord;

    DEBUG ? printf("readWord iLen=%x\n", iLen) : 0;

    if (iLen > 0) {
        // allocate memory for strings
        szRetWord = calloc(sizeof(char), iLen + 1);
        szTmpWord = calloc(sizeof(char), 1024 + 1);

        while (iLen != 0) {
            // determine number of bytes to read this time around
            // lesser of 1024 or the number of byes left to read
            // in this word
            iBytesToRead = iLen > 1024 ? 1024 : iLen;

            // read iBytesToRead from the socket
            iBytesRead = recv(fdSock, szTmpWord, iBytesToRead, 0);
            if (iBytesRead <= 0) {
                free(szTmpWord);
                free(szRetWord);
                return 0;
            }

            // terminate szTmpWord
            szTmpWord[iBytesRead] = 0;

            // concatenate szTmpWord to szRetWord
            strcat(szRetWord, szTmpWord);

            // subtract the number of bytes we just read from iLen
            iLen -= iBytesRead;
        }

        // deallocate szTmpWord
        free(szTmpWord);

        DEBUG ? printf("word = %s\n", szRetWord) : 0;
        return szRetWord;
    } else {
        return NULL;
    }
}

/********************************************************************
* Read a sentence from the socket
* A Sentence struct is returned
********************************************************************/
struct Sentence readSentence(int fdSock) {
    struct Sentence stReturnSentence;
    char            *szWord;
    int             i             = 0;
    int             iReturnLength = 0;

    DEBUG ? printf("readSentence\n") : 0;

    initializeSentence(&stReturnSentence);

    while (szWord = readWord(fdSock)) {
        addWordToSentence(&stReturnSentence, szWord);

        // check to see if we can get a return value from the API
        if (strstr(szWord, "!done") != NULL) {
            DEBUG ? printf("return sentence contains !done\n") : 0;
            stReturnSentence.iReturnValue = DONE;
        } else if (strstr(szWord, "!trap") != NULL) {
            DEBUG ? printf("return sentence contains !trap\n") : 0;
            stReturnSentence.iReturnValue = TRAP;
        } else if (strstr(szWord, "!fatal") != NULL) {
            DEBUG ? printf("return sentence contains !fatal\n") : 0;
            stReturnSentence.iReturnValue = FATAL;
        }
        free(szWord);
    }

    // if any errors, get the next sentence
    if ((stReturnSentence.iReturnValue == TRAP) || (stReturnSentence.iReturnValue == FATAL)) {
        readSentence(fdSock);
    }

    if (DEBUG) {
        for (i = 0; i < stReturnSentence.iLength; i++) {
            printf("stReturnSentence.szSentence[%d] = %s\n", i, stReturnSentence.szSentence[i]);
        }
    }

    return stReturnSentence;
}
