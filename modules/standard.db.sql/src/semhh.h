/*====================================================================*/
/* Chaine    : tests Semaphores                                       */
/* Programme : semhh.h                                                */
/* But       : tests semaphores et multi-tasking                      */
/* Creation  : 25/06/00                                               */
/* Auteur    : Henri HENAULT (C) 2000-2005 H.H. & S                   */
/*             http://www.hhns.fr                                     */
/*--------------------------------------------------------------------*/
/* Historique                                                         */
/*--------------------------------------------------------------------*/
/* But       :                                                        */
/* date      :                                                        */
/* Auteur    :                                                        */
/*--------------------------------------------------------------------*/
/* But       :                                                        */
/* date      :                                                        */
/* Auteur    :                                                        */
/*====================================================================*/
#if !defined SEMHH_DONE
#     define  SEMHH_DONE
#include <string.h>
#include <stdio.h>

# if defined(_WIN32)
/*===============================================================================*/
/*===============================================================================*/
/*======================= WIN32 =================================================*/
/*===============================================================================*/
/*===============================================================================*/
 #      include <windows.h>
 #      include <process.h>
 #      include <signal.h>

 #    define SLEEP(n)    Sleep(1000 * n)
 #    define NAP(n)      Sleep(n)

typedef HANDLE   HHSEM;
typedef HANDLE   TSKID_t;

 #      define seminit(_s, _a)    _s = CreateSemaphore(NULL, _a, 0x7FFF, NULL)
 #      define semp(_a)                    \
    {                                      \
        WaitForSingleObject(_a, INFINITE); \
    }
 #      define semv(_a)                \
    {                                  \
        ReleaseSemaphore(_a, 1, NULL); \
    }

 #      define semdel(_a) \
    {                     \
        CloseHandle(_a);  \
    }

/*-------------------------------------------------------------------*/
/* tskid  : GetCurThrId  ...                                         */
/*-------------------------------------------------------------------*/
 #      define tskid()    (TSKID_t)GetCurrentThreadId()

static void abend(char *r, int e, char *s) {
    fprintf(stderr, "%s (x%08lx): ABEND Reason code %d, %s\n", tskid(), r, e, s);
    fflush(stderr);
    abort();
}

 #  define __SEMHH_DEFAULT_MAX_TASK    10
static TSKID_t *__SEMHH_TASK_TABLE = NULL;
static int     __SEMHH_TASK_NUM    = 0;
static int     __SEMHH_TASK_MAX    = __SEMHH_DEFAULT_MAX_TASK;

/*-------------------------------------------------------------------*/
/* strtsk : CreateThread ...                                         */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_START_TASK(void *f, void *arg) {
    HANDLE   wcid;
    unsigned chn;

    if (__SEMHH_TASK_TABLE == NULL) {
        int  lg = __SEMHH_TASK_MAX * sizeof(TSKID_t);
        void *p = malloc(lg);
        if (!p) abend("strtsk", -1, "alloc task table, not enough storage");
        __SEMHH_TASK_TABLE = (void **)p;
    } else
    if (__SEMHH_TASK_NUM == __SEMHH_TASK_MAX) {         /* task table is full, realloc ... */
        int  lg = __SEMHH_TASK_MAX * sizeof(TSKID_t);   /* old size */
        void *p;
        __SEMHH_TASK_MAX += __SEMHH_DEFAULT_MAX_TASK;   /* new size */
        p = malloc(__SEMHH_TASK_MAX * sizeof(TSKID_t)); /* new alloc */
        if (!p) abend("strtsk", -1, "re-alloc task table, not enough storage");
        memcpy(p, __SEMHH_TASK_TABLE, lg);              /* keep old values */
        free(__SEMHH_TASK_TABLE);                       /* free old table  */
        __SEMHH_TASK_TABLE = (void **)p;                /* set up new table */
    }
    wcid = (TSKID_t)CreateThread(NULL, 0, /*(void *)*/ (LPTHREAD_START_ROUTINE)f, (void *)arg, 0, NULL);
    __SEMHH_TASK_TABLE[__SEMHH_TASK_NUM] = wcid;
    ++__SEMHH_TASK_NUM;

    return wcid;
}

 #      define strtsk(f, arg) \
    __SEMHH_START_TASK(f, arg)

/*-------------------------------------------------------------------*/
/* deltskbn : delete a task by num                                     */
/* deltskbi : delete a task by id                                      */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_DEL_TASK_BY_NUM(int n) {
    TSKID_t *p  = __SEMHH_TASK_TABLE + n,
            cid = *p;
    int m       = __SEMHH_TASK_NUM - n - 1;

    if (m < 0) abend("waitsk", n, "DEL_TASK_BY_NUM, bad n");
    if (m > 0) memcpy(p, p + 1, m * sizeof(TSKID_t));
    --__SEMHH_TASK_NUM;
    return cid;
}

static TSKID_t   __SEMHH_DEL_TASK_BY_ID(TSKID_t cid) {
    int n;

    for (n = 0; n < __SEMHH_TASK_NUM; ++n)
        if (cid == __SEMHH_TASK_TABLE[n]) return __SEMHH_DEL_TASK_BY_NUM(n);
    return 0;
}

/*-------------------------------------------------------------------*/
/* waitsk : WaitObject, GetExitCode, Close                           */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_WAIT_SINGLE_TASK(TSKID_t cid, int *prc) {
    if (cid == 0) return __SEMHH_WAIT_SINGLE_TASK(cid, prc);
    if (__SEMHH_TASK_NUM <= 0) {
        abend("waitsk", -3, "WaitM, task_num = 0");
    }
    if (WaitForSingleObject(cid, INFINITE) < 0) {
        abend("waitsk", GetLastError(), "WaitSingle");
    }
    __SEMHH_DEL_TASK_BY_ID(cid);
    GetExitCodeThread(cid, (DWORD *)prc);
    CloseHandle(cid);
    return cid;
}

 #      define waitsk(_cid, _rc)    __SEMHH_WAIT_SINGLE_TASK((TSKID_t)_cid, &_rc)

/*-------------------------------------------------------------------*/
/* waitany: WaitMultipleObject, GetExitCode, Close                           */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_WAIT_MULTIPLE(int *prc) {
    int     i = 0, n;
    TSKID_t cid;

    if (__SEMHH_TASK_NUM <= 0)
        abend("waitsk", -3, "WaitM, task_num = 0");
    if (__SEMHH_TASK_NUM <= 64) {
        n = WaitForMultipleObjects(__SEMHH_TASK_NUM, __SEMHH_TASK_TABLE, 0, INFINITE);
        goto retour;
    }
    for ( ; ; ) {      /* wait multiple */
        n = 0;
        for (i = 0; i < __SEMHH_TASK_NUM; i += 64) {
            int wn = __SEMHH_TASK_NUM - i < 64 ? __SEMHH_TASK_NUM - i : 64;
            n = WaitForMultipleObjects(wn, __SEMHH_TASK_TABLE + i, 0, 0);
            if ((0 <= n) && (n < wn)) {
                goto retour;
            }
            Sleep(25);
        }
    }
retour:
    cid = __SEMHH_DEL_TASK_BY_NUM(n + i);
    GetExitCodeThread(cid, (DWORD *)prc);
    CloseHandle(cid);
    return cid;
}

 #      define waitm(_rc)      __SEMHH_WAIT_MULTIPLE(&_rc)
 #      define waitany(_rc)    waitm(rc)


/*-------------------------------------------------------------------*/
/* waitall: waitany task_num times ...                               */
/*-------------------------------------------------------------------*/
static int      __SEMHH_WAIT_ALL() {
    int n = 0, rc;

    for (n = 0; __SEMHH_TASK_NUM > 0; ++n)
        waitm(rc);
    return n;
}

 #      define waitall()    __SEMHH_WAIT_ALL()

/*-------------------------------------------------------------------*/
/* killtsk : TerminateThread  (mal defini, danger)                    */
/*-------------------------------------------------------------------*/
 #      define killtsk(cid)    TerminateThread((TSKID_t)cid, 127)

/*-------------------------------------------------------------------*/
/* exitsk : ExitThread   ...                                         */
/*-------------------------------------------------------------------*/
 #      define exitsk(rc)    ExitThread(rc)


# else
/*===============================================================================*/
/*===============================================================================*/
/*======================= UNIX ==================================================*/
/*=======================         Tested on following platforms :  ==============*/
/*=======================          AIX-4.3-PowerPC                 ==============*/
/*=======================          SunOS-5.8-Sparc                 ==============*/
/*=======================          Linux-386  RedHat 7.2           ==============*/
/*=======================          Linux-386  RedHat 9             ==============*/
/*=======================          Linux-386  Fedora Core 2        ==============*/
/*=======================         zLinux-S390  32 bits             ==============*/
/*=======================         zLinux-S390  64 bits             ==============*/
/*===============================================================================*/
/*===============================================================================*/

 #     include <pthread.h>
 #     include <signal.h>
 #     include <errno.h>
 #     include <stdlib.h>
 #    define SLEEP(n)    sleep(n)
 #    define NAP(n)      usleep(1000 * n)

typedef struct {
    int             v;
    pthread_mutex_t mutx;
    pthread_cond_t  cond;
}  HHSEM;

typedef pthread_t   TSKID_t;

typedef struct s_Task_Table {
    TSKID_t  id;
    int      rc;
    unsigned fl;
} __SEMHH_T_TASK;

/*-------------------------------------------------------------------*/
/* tskid  : pthread_self ...                                         */
/*-------------------------------------------------------------------*/
 #      define tskid()    pthread_self()

static void abend(char *r, int e, char *s) {
    fprintf(stderr, "%s (x%08lx): ABEND Reason code %d, %s\n", r, tskid(), e, s);
    fflush(stderr);
    abort();
}

/*-------------------------------------------------------------------*/
/* setpri : undefined    ...                                         */
/*-------------------------------------------------------------------*/
 #     define setpri(cid, n)    /* nop */


/*-------------------------------------------------------------------*/
/* semp, semv, seminit   ...                                         */
/*-------------------------------------------------------------------*/
 #     define semp(s)                                \
    {                                                \
        pthread_mutex_lock(&(s.mutx));               \
        if (--(s.v) < 0) {                           \
            int wprt = s.v;                          \
            pthread_cond_wait(&(s.cond), &(s.mutx)); \
        }                                            \
        pthread_mutex_unlock(&(s.mutx));             \
    }

 #     define semv(s)                        \
    {                                        \
        pthread_mutex_lock(&(s.mutx));       \
        if ((s.v)++ < 0)  {                  \
            pthread_mutex_unlock(&(s.mutx)); \
            pthread_cond_signal(&(s.cond));  \
        }                                    \
        else {                               \
            pthread_mutex_unlock(&(s.mutx)); \
        }                                    \
    }

 #     define seminit(s, u)                  \
    {                                        \
        s.v = u;                             \
        pthread_mutex_init(&(s.mutx), NULL); \
        pthread_cond_init(&(s.cond), NULL);  \
    }

 #     define semdel(s)                   \
    {                                     \
        pthread_mutex_destroy(&(s.mutx)); \
        pthread_cond_destroy(&(s.cond));  \
    }

 #  define __SEMHH_DEFAULT_MAX_TASK    10
 #  define __SEMHH_TASK_RUNNING        0
 #  define __SEMHH_TASK_ENDED          1
static __SEMHH_T_TASK *__SEMHH_TASK_TABLE = NULL;
static int            __SEMHH_TASK_NUM    = 0;
static HHSEM          __SEMHH_TASK_TABLE_MUTEX;
static HHSEM          __SEMHH_END_TASK;
static int            __SEMHH_TASK_MAX = __SEMHH_DEFAULT_MAX_TASK;

/*-------------------------------------------------------------------*/
/* strtsk   : start a new subtask                                    */
/*     if 1st call, alloc a default TASK_TABLE                       */
/*     else, if not enough room, reallocate TASK_TABLE               */
/*     pthread_create -> cid                                         */
/*     setup new entry in TASK_TABLE                                 */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_START_TASK(void *f, void *arg) {
    TSKID_t chn;
    int     n;

    if (__SEMHH_TASK_TABLE == NULL) {
        int  lg = __SEMHH_TASK_MAX * sizeof(__SEMHH_T_TASK);
        void *p = malloc(lg);
        if (!p) abend("strtsk", -1, "alloc task table, not enough storage");
        __SEMHH_TASK_TABLE = (__SEMHH_T_TASK *)p;
        seminit(__SEMHH_TASK_TABLE_MUTEX, 1);
        seminit(__SEMHH_END_TASK, 0);
    }        /* and __SEMHHH_TASK_NUM == 0 */

    semp(__SEMHH_TASK_TABLE_MUTEX);

    if (__SEMHH_TASK_NUM == __SEMHH_TASK_MAX) {              /* task table is full, realloc ... */
        int  lg = __SEMHH_TASK_MAX * sizeof(__SEMHH_T_TASK); /* old size */
        int  lgnew;
        void *p;
        __SEMHH_TASK_MAX += __SEMHH_DEFAULT_MAX_TASK;                  /* new size */
        lgnew             = __SEMHH_TASK_MAX * sizeof(__SEMHH_T_TASK); /* new alloc */
        p = malloc(lgnew);                                             /* new alloc */
        if (!p) abend("strtsk", -1, "re-alloc task table, not enough storage");
        memcpy(p, __SEMHH_TASK_TABLE, lg);                             /* keep old values */
        free(__SEMHH_TASK_TABLE);                                      /* free old table  */
        __SEMHH_TASK_TABLE = (__SEMHH_T_TASK *)p;                      /* set up new table */
    }
    if (pthread_create((TSKID_t *)&chn, NULL, (void *(*)(void *))f, arg)) {
        int e = errno;
        abend("strtsk", e, "pthread_create failed");
    }
    n = __SEMHH_TASK_NUM++;
    __SEMHH_TASK_TABLE[n].id = chn;
    __SEMHH_TASK_TABLE[n].rc = 0;
    __SEMHH_TASK_TABLE[n].fl = __SEMHH_TASK_RUNNING;
/*printf("\t\tstrtsk : new cid x%08lx #%d inserted @%p\n", chn, n, __SEMHH_TASK_TABLE + n ); fflush(stdout);*/
    semv(__SEMHH_TASK_TABLE_MUTEX);
    return chn;
}

 #     define strtsk(_f, _arg) \
    __SEMHH_START_TASK((void *)_f, (void *)_arg)

/*-------------------------------------------------------------------*/
/* deltskbn : delete a task by num                                     */
/* deltskbi : delete a task by id                                      */
/*-------------------------------------------------------------------*/
static TSKID_t   __SEMHH_DEL_TASK_BY_NUM(int n) {
    __SEMHH_T_TASK *p  = __SEMHH_TASK_TABLE + n;
    TSKID_t        cid = p->id;
    int            m;

    m = __SEMHH_TASK_NUM - n - 1;
    if (m < 0) abend("del_tsk_by_num", n, "__SEMHH_TASK_NUM - n - 1 < 0");
    if (m > 0)
        memcpy(p, p + 1, m * sizeof(__SEMHH_T_TASK));
/*printf("\t\tdeltsk : cid x%08lx #%d removed from table @%p\n", cid, n, p ); fflush(stdout);*/
    --__SEMHH_TASK_NUM;
    return cid;
}

/*----------------------------------------------------------*/
/* waitany : wait  for any task end                         */
/*     wait for __SEMHH_END_TASK  posted,                   */
/*     find 1st flag 'TASK_ENDED' in TASK_TABLE             */
/*     grab retcode from TASK_TABLE and setup user rc       */
/*     remove cid entry from TASK_TABLE                     */
/*     pthread_join (dummy retcode ptr)                     */
/*     return cid                                           */
/*----------------------------------------------------------*/
static TSKID_t   __SEMHH_WAIT_MULTIPLE(int *prc) {
    int     n, rc, flg;
    TSKID_t cid;

    semp(__SEMHH_END_TASK);

    semp(__SEMHH_TASK_TABLE_MUTEX);
    for (n = 0; n < __SEMHH_TASK_NUM; ++n)
        if (__SEMHH_TASK_TABLE[n].fl == __SEMHH_TASK_ENDED) goto found_it;
    abend("wait multiple", __SEMHH_TASK_NUM, "no flag task_ended found in tsk_table");

found_it:
    *prc = rc = __SEMHH_TASK_TABLE[n].rc;
    cid  = __SEMHH_TASK_TABLE[n].id;
    __SEMHH_DEL_TASK_BY_NUM(n);
    semv(__SEMHH_TASK_TABLE_MUTEX);

    pthread_join(cid, (void **)&rc); /* on est sur de rien avec rc, on l'ignore : resultat deja dans prc */
    return cid;
}

 #      define waitm(_rc)      __SEMHH_WAIT_MULTIPLE(&_rc)
 #      define waitany(_rc)    waitm(rc)

/*----------------------------------------------------------*/
/* waitsk  : wait  for a specific task end                  */
/*     wait for __SEMHH_END_TASK  posted,                   */
/*     find cid in TASK_TABLE, or abend                     */
/*     grab retcode from TASK_TABLE and setup user rc       */
/*     remove cid entry from TASK_TABLE                     */
/*     pthread_join (dummy retcode ptr)                     */
/*     return cid                                           */
/*----------------------------------------------------------*/
static TSKID_t   __SEMHH_WAIT_SINGLE_TASK(TSKID_t cid, int *prc) {
    int n, rc, flg;

    if (cid == 0) return __SEMHH_WAIT_MULTIPLE(prc);
    semp(__SEMHH_END_TASK);

    semp(__SEMHH_TASK_TABLE_MUTEX);

    for (n = 0; n < __SEMHH_TASK_NUM; ++n) {
        __SEMHH_T_TASK *p = __SEMHH_TASK_TABLE + n;
/*printf("\t\twaitsk : cid x%08lx :: table[%d] @%p, =x%08lx ...\n", cid, n, p ); fflush(stdout);*/
        if (cid == p->id) goto found_it;
    }
    abend("waitsk single", __SEMHH_TASK_NUM, "cid not found in  tsk_table");

found_it:
    *prc = rc = __SEMHH_TASK_TABLE[n].rc;
    __SEMHH_DEL_TASK_BY_NUM(n);

    semv(__SEMHH_TASK_TABLE_MUTEX);
    pthread_join(cid, (void **)&rc);   /* on est sur de rien avec rc, on l'ignore : resultat deja dans prc */
    return cid;
}

 #  define waitsk(_cid, _rc)    __SEMHH_WAIT_SINGLE_TASK((TSKID_t)_cid, &_rc)

/*----------------------------------------------------------*/
/* waitall : waitsk for each cid ...                        */
/*----------------------------------------------------------*/
static int       __SEMHH_WAIT_ALL() {
    int n, nt, rc;

    semp(__SEMHH_TASK_TABLE_MUTEX);
    nt = __SEMHH_TASK_NUM;
    semv(__SEMHH_TASK_TABLE_MUTEX);
    for (n = 0; n < nt; ++n) {
        TSKID_t cid = waitany(rc);
    }
    return n;
}

 #      define waitall()    __SEMHH_WAIT_ALL()

/*----------------------------------------------------------*/
/* exitsk : terminate a thread                              */
/*     get current cid                                      */
/*     find cid in TASK_TABLE, or abend                     */
/*     setup retcode and flag=TASK_ENDED in TASK_TABLE      */
/*     pthread_exit (&rc)                                   */
/*----------------------------------------------------------*/
static TSKID_t   __SEMHH_EXIT_TASK(int rc) {
    int     n, wrc = rc;
    TSKID_t cid = pthread_self();

    semp(__SEMHH_TASK_TABLE_MUTEX);

    for (n = 0; n < __SEMHH_TASK_NUM; ++n) {
        if (cid == __SEMHH_TASK_TABLE[n].id) goto found_it;
    }
    abend("exitsk", __SEMHH_TASK_NUM, "cid not found in task_table");

found_it:
    __SEMHH_TASK_TABLE[n].rc = rc;
    __SEMHH_TASK_TABLE[n].fl = __SEMHH_TASK_ENDED;

    semv(__SEMHH_TASK_TABLE_MUTEX);
    semv(__SEMHH_END_TASK);
    pthread_exit(&wrc);
}

 #      define exitsk(_rc)    __SEMHH_EXIT_TASK(_rc)

/*-------------------------------------------------------------------*/
/* killtsk = pthread_kill                                             */
/*-------------------------------------------------------------------*/
 #      define killtsk(_cid) \
    pthread_kill(_cid, SIGUSR1)
# endif /* UNIX */
#endif  /* undef  SEMHH_DONE   */
