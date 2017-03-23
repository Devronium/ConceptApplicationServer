//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#ifndef _WIN32
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <string.h>
 #include <sys/select.h>
 #include <sys/poll.h>
 #include <sys/time.h>

 #define INVALID_SOCKET    -1
#endif
#include <stdlib.h>

#ifdef _WIN32
    #define WITH_SELECT
    #pragma message ( "Building with SELECT" )
#else
#ifdef __linux__
    #define WITH_EPOLL
    #pragma message ( "Building with EPOLL" )
#else
#if defined(__MACH__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #define WITH_KQUEUE
    #pragma message ( "Building with KQUEUE" )
#else
    #pragma message ( "WARNING: Cannot determine operating system. Falling back to poll." )
    #define WITH_POLL
#endif
#endif
#endif

#ifdef WITH_KQUEUE
    #include <sys/event.h>
#endif
#ifdef WITH_EPOLL
    #include <sys/epoll.h>
#endif

#if defined(WITH_SELECT) || defined(WITH_POLL)
#define WITH_SELECT_POLL
class PollContainer {
private:
#ifdef WITH_SELECT
    fd_set chlist;
#endif
#ifdef WITH_KQUEUE
    int fd;
#endif
#ifdef WITH_POLL
    struct pollfd *chlist;
    int chlist_len;
    int chlist_pos;
#endif
public:
#ifdef WITH_KQUEUE
    PollContainer(int fd) {
        this->fd = fd;
    }
#endif
#ifdef WITH_POLL
    PollContainer() {
        this->chlist = NULL;
        this->chlist_len = 0;
        this->chlist_pos = 0;
    }
#endif
#ifdef WITH_SELECT
    PollContainer() {
        FD_ZERO(&chlist);
    }
#endif

    int Add(int efd) {
#ifdef WITH_KQUEUE
        struct kevent event;
        EV_SET(&event, efd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        return kevent(fd, &event, 1, NULL, 0, NULL);
#endif
#ifdef WITH_POLL
        if (chlist_pos <= chlist_len) {
            chlist_len += 64;
            chlist = (struct pollfd *)realloc(chlist, sizeof(struct pollfd) * chlist_len);
        }
        if (!chlist)
            return -2;
        chlist[chlist_pos].fd = efd;
        chlist[chlist_pos].events = POLLIN | POLLPRI;
        chlist[chlist_pos].revents = 0;
        chlist_pos++;
#endif
#ifdef WITH_SELECT
        FD_SET(efd, &chlist);
#endif
        return 0;
    }

    int Wait(INVOKE_CALL Invoke, void *RESULT, int timeout) {
#ifdef WITH_KQUEUE
        int maxevents = 1024;
        struct kevent *events = (struct kevent *)malloc(sizeof(struct kevent));
        if (!events)
            return -1;

        int nev = kevent(fd, NULL, 0, events, maxevents, NULL);
        for (INTEGER i = 0; i < nev; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 

        free(events);

        if (nev < 0)
            return nev;
#endif
#ifdef WITH_SELECT
        struct timeval tout;
        tout.tv_sec = 0;
        tout.tv_usec = 0;
        if (timeout > 0) {
            tout.tv_sec = timeout / 1000;
            tout.tv_usec = (timeout % 1000) * 1000;
        }
        fd_set fd_list;
        FD_ZERO(&fd_list);
#ifdef _WIN32
        for (int i = 0; i < chlist.fd_count; i++)
            FD_SET(chlist.fd_array[i], &fd_list);
#else
        FD_COPY(&chlist, &fd_list);
#endif

        INTEGER index = 0;
        if (select(FD_SETSIZE, &fd_list, 0, 0, &tout) > 0) {
            for (int i = 0; i < chlist.fd_count; i++) {
                if (FD_ISSET(chlist.fd_array[i], &fd_list))
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist.fd_array[i]); 
            }
        }
#endif
#ifdef WITH_POLL
        INTEGER index = 0;
        int q = poll(chlist, chlist_pos, timeout);
        if (q < 0)
            return q;
        for (int i = 0; i < q; i++) {
            if (chlist[i].revents && POLLIN) {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
            }
            chlist[i].revents = 0;
        }
#endif
        return 0;
    }

    int Remove(int efd) {
#ifdef WITH_KQUEUE
        struct kevent event;
        EV_SET(&event, efd, EVFILT_READ, EV_DELETE, 0, 0, 0);
        return kevent(fd, &event, 1, NULL, 0, NULL);
#endif
#ifdef WITH_POLL
        if ((chlist_pos) && (chlist)) {
            for (int i = 0; i < chlist_pos; i++) {
                if (chlist[i].fd == efd) {
                    chlist_pos--;
                    for (int j = i; j < chlist_pos; j++) {
                        chlist[i].fd = chlist[i + 1].fd;
                        chlist[i].events = chlist[i + 1].events;
                        chlist[i].revents = 0;
                    }
                    if (chlist_len - chlist_pos > 64) {
                        chlist_len -= 64;
                        chlist = (struct pollfd *)realloc(chlist, sizeof(struct pollfd) * chlist_len);
                    }
                    break;
                }
            }
        }
#endif
#ifdef WITH_SELECT
        FD_CLR(efd, &chlist);
#endif
        return 0;
    }

    ~PollContainer() {
#ifdef WITH_KQUEUE
        close(fd);
#endif
#ifdef WITH_POLL
        if (chlist)
            free(chlist);
#endif
    }
};
#endif

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PollOpen, 0)
#ifdef WITH_SELECT_POLL
    PollContainer *fd = new PollContainer();
    RETURN_NUMBER((SYS_INT)fd);
#else
    #ifdef WITH_KQUEUE
        int kfd = kqueue();
        RETURN_NUMBER((SYS_INT)kfd);
    #endif
    #ifdef WITH_EPOLL
        int fd = epoll_create1(0);
        RETURN_NUMBER(fd);
    #endif
#endif
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PollClose, 1)
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollClose, 0);
    PollContainer *fd = (PollContainer *)(SYS_INT)PARAM(0);
    delete fd;
    SET_NUMBER(0, 0);
#endif
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollClose, 0);
    int fd = PARAM_INT(0);
    if (fd > 0) {
        close(fd);
        SET_NUMBER(0, 0);
    }
#endif
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PollAdd, 2)
    T_NUMBER(PollAdd, 1);
    int fd = PARAM_INT(1);
    int err = -1;
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollAdd, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    if (fd > 0)
        err = efd->Add(fd);
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollAdd, 0);
    int efd = PARAM_INT(0);
    if ((fd > 0) && (efd > 0)) {
#ifdef WITH_KQUEUE
        struct kevent event;
        EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        err = kevent(efd, &event, 1, NULL, 0, NULL);
#else
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLPRI | EPOLLHUP | EPOLLRDHUP;// | EPOLLET;
        err = epoll_ctl (efd, EPOLL_CTL_ADD, fd, &event);
#endif
    }
#endif
#endif
    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PollRemove, 2)
    T_NUMBER(PollRemove, 1);
    int fd = PARAM_INT(1);
    int err = -1;
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollRemove, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    if (fd > 0)
        err = efd->Remove(fd);
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollRemove, 0);
    int efd = PARAM_INT(0);
    if ((fd > 0) && (efd > 0)) {
#ifdef WITH_KQUEUE
        struct kevent event;
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
        err = kevent(efd, &event, 1, NULL, 0, NULL);
#else
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLPRI | EPOLLHUP | EPOLLRDHUP;// | EPOLLET;
        err = epoll_ctl (efd, EPOLL_CTL_DEL, fd, &event);
#endif
    }
#endif
#endif
    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PollWait, 1, 2)
    int timeout = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(PollWait, 1);
        timeout = PARAM_INT(1);
    }

#ifdef WITH_SELECT_POLL
    T_HANDLE(PollWait, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    CREATE_ARRAY(RESULT);
    efd->Wait(Invoke, RESULT, timeout);
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollWait, 0);
    int efd = PARAM_INT(0);
    if (efd > 0) {
        int maxevents = 1024;
        CREATE_ARRAY(RESULT);
#ifdef WITH_KQUEUE
        struct kevent *events = (struct kevent *)malloc(sizeof(struct kevent));
        if (!events) {
            RETURN_NUMBER(0);
            return (void *)"PollWait: Out of memory";
        }

        int nev = kevent(efd, NULL, 0, events, maxevents, NULL);
        for (INTEGER i = 0; i < nev; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 
        free(events);
#else
        struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event));
        if (events) {
            CREATE_ARRAY(RESULT);
            int nfds = epoll_wait(efd, events, maxevents, timeout);
            INTEGER index = 0;
            for (int i = 0; i < nfds; i++)
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);
            free(events);
        } else {
            RETURN_NUMBER(0);
            return (void *)"PollWait: Out of memory";
        }
#endif
    } else {
        RETURN_NUMBER(0);
    }
#endif
#endif
END_IMPL
//=====================================================================================//
