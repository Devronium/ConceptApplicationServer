#ifndef SHMANAGER_H
#define SHMANAGER_H

#define MAX_KEY_SIZE    0xFFF

//#define ARM_ADJUST_SIZE(x) (x)%4 ? (x)+4-(x)%4 : x

// emulate P and V semaphores
bool SHLock(char *key);
void SHUnLock();
void SHLockWait(char *key);

void *SHPoolAlloc(char *key, int size, int nReadOnly = 0);
void *SHPoolOpen(char *key, int nReadOnly = 0);
void *SHAlloc(int size);
bool SHIsPooled();
bool SHIsCreated();
void SHPoolFree(void *pBuf);
#endif

