#ifndef __MESSAGES_H
#define __MESSAGES_H

#include "AnsiString.h"
#include "ConceptClient.h"

#define FILE_MESSAGE    0x1000 // is important to be a multiple of AES Keysize (16 in our case)

#define BIG_MESSAGE     0x7FFFF // 50 000 bytes ... 0x2800 10 000 bytes
#define CHUNK_SIZE      0x1FFFF //0xC800 // 0x400  // 16k send buffer size
#define RBUF_SIZE       0xFFFFF

int get_message(CConceptClient *OWNER, TParameters *PARAM, PROGRESS_API notify_parent = 0);
int send_message(CConceptClient *OWNER, AnsiString SENDER_NAME, int MESSAGE_ID, AnsiString MESSAGE_TARGET, AnsiString& MESSAGE_DATA, PROGRESS_API notify_parent = 0);
void set_next_post_filename(char *filename);
void ResetMessages(CConceptClient *OWNER);
int wait_message(CConceptClient *OWNER, TParameters *PARAM, int MESSAGE_ID, PROGRESS_API notify_parent = 0);
int have_messages(CConceptClient *OWNER);
void InitUDP(CConceptClient *owner, int port);
AnsiString InitUDP2(CConceptClient *owner, char *host);
AnsiString InitUDP3(CConceptClient *owner, char *host);
AnsiString SwitchP2P(CConceptClient *owner, char *host);
AnsiString P2PInfo(CConceptClient *owner, char *host);
#endif //__MESSAGES_H
