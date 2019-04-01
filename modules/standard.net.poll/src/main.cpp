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
    fd_set outlist;
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
        FD_ZERO(&outlist);
    }
#endif

    int Add(int efd, int mode) {
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
        switch (mode) {
            case 1:
                chlist[chlist_pos].events = POLLOUT;
                break;
            case 3:
                chlist[chlist_pos].events = POLLIN | POLLPRI | POLLOUT;
                break;
            default:
                chlist[chlist_pos].events = POLLIN | POLLPRI;
        }
        chlist[chlist_pos].revents = 0;
        chlist_pos++;
#endif
#ifdef WITH_SELECT
        switch (mode) {
            case 1:
                FD_SET(efd, &outlist);
                break;
            case 3:
                FD_SET(efd, &chlist);
                FD_SET(efd, &outlist);
                break;
            default:
                FD_SET(efd, &chlist);
        }
#endif
        return 0;
    }

    int Wait(INVOKE_CALL Invoke, void *RESULT, int timeout, void *OUT_SOCKETS) {
#ifdef WITH_KQUEUE
        int maxevents = 8192;
        struct kevent *events = (struct kevent *)malloc(sizeof(struct kevent) * maxevents);
        if (!events)
            return -1;

        int nev = kevent(fd, NULL, 0, events, maxevents, NULL);
        INTEGER index = 0;
        INTEGER out_index = 0;
        for (INTEGER i = 0; i < nev; i++) {
            if ((OUT_SOCKETS) && (events[i].filter == EVFILT_WRITE)) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, OUT_SOCKETS, out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 
            } else {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 
            }
        }

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
        fd_set fd_out_list;
        FD_ZERO(&fd_list);
        if (OUT_SOCKETS) {
            FD_ZERO(&fd_out_list);
        }
#ifdef _WIN32
        for (int i = 0; i < chlist.fd_count; i++) {
            FD_SET(chlist.fd_array[i], &fd_list);
        }
        if (OUT_SOCKETS) {
            for (int i = 0; i < outlist.fd_count; i++)
                FD_SET(outlist.fd_array[i], &fd_out_list);
        }
#else
        FD_COPY(&chlist, &fd_list);
        if (OUT_SOCKETS) {
            FD_COPY(&outlist, &fd_out_list);
        }
#endif

        INTEGER index = 0;
        INTEGER out_index = 0;
        int err;
        if (OUT_SOCKETS)
            err = select(FD_SETSIZE, &fd_list, &fd_out_list, 0, &tout);
        else
            err = select(FD_SETSIZE, &fd_list, 0, 0, &tout);
        if (err > 0) {
            for (int i = 0; i < chlist.fd_count; i++) {
                if (FD_ISSET(chlist.fd_array[i], &fd_list))
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist.fd_array[i]); 
            }
            if (OUT_SOCKETS) {
                for (int i = 0; i < outlist.fd_count; i++) {
                    if (FD_ISSET(outlist.fd_array[i], &fd_out_list))
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, OUT_SOCKETS, out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)outlist.fd_array[i]); 
                }
            }
        }
#endif
#ifdef WITH_POLL
        INTEGER index = 0;
        INTEGER out_index = 0;
        int q = poll(chlist, chlist_pos, timeout);
        if (q < 0)
            return q;
        for (int i = 0; i < q; i++) {
            if ((OUT_SOCKETS) && (chlist[i].revents & POLLOUT)) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, OUT_SOCKETS, out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
            }
            if (chlist[i].revents & POLLIN) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
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
        FD_CLR(efd, &outlist);
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
    DEFINE_SCONSTANT("POLL_WRITE",  "1");
    DEFINE_SCONSTANT("POLL_READ",   "2");
    DEFINE_SCONSTANT("POLL_RW",     "3");
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PollAdd, 2, 3)
    T_NUMBER(PollAdd, 1);
    int fd = PARAM_INT(1);
    int err = -1;
    int mode = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(PollAdd, 2);
        mode = PARAM_INT(2);
    }
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollAdd, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    if (fd > 0)
        err = efd->Add(fd, mode);
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollAdd, 0);
    int efd = PARAM_INT(0);
    if ((fd > 0) && (efd > 0)) {
#ifdef WITH_KQUEUE
        struct kevent events[2];
        int num_events = 1;
        switch (mode) {
            case 1:
                EV_SET(&events[0], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
                break;
            case 3:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
                EV_SET(&events[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
                num_events = 2;
                break;
            default:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        }
        err = kevent(efd, events, num_events, NULL, 0, NULL);
#else
        struct epoll_event event;
        // supress valgrind warning
        event.data.u64 = 0;
        event.data.fd = fd;
        switch (mode) {
            case 1:
                event.events = EPOLLOUT;
                break;
            case 3:
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
                break;
            default:
                event.events = EPOLLIN | /* EPOLLPRI |*/ EPOLLHUP | EPOLLRDHUP;// | EPOLLET;
        }
        err = epoll_ctl (efd, EPOLL_CTL_ADD, fd, &event);
#endif
    }
#endif
#endif
    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PollUpdate, 2, 3)
    T_NUMBER(PollUpdate, 1);
    int fd = PARAM_INT(1);
    int err = -1;
    int mode = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(PollUpdate, 2);
        mode = PARAM_INT(2);
    }
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollUpdate, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    if (fd > 0) {
        efd->Remove(fd);
        err = efd->Add(fd, mode);
    }
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollUpdate, 0);
    int efd = PARAM_INT(0);
    if ((fd > 0) && (efd > 0)) {
#ifdef WITH_KQUEUE
        struct kevent events[2];
        int num_events = 1;
        // Adds the event to the kqueue. Re-adding an existing event will modify the parameters of the original event, and not result in a duplicate entry.
        switch (mode) {
            case 1:
                EV_SET(&events[0], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
                break;
            case 3:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
                EV_SET(&events[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
                num_events = 2;
                break;
            default:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        }
        err = kevent(efd, events, num_events, NULL, 0, NULL);
#else
        struct epoll_event event;
        // supress valgrind warning
        event.data.u64 = 0;
        event.data.fd = fd;
        switch (mode) {
            case 1:
                event.events = EPOLLOUT;
                break;
            case 3:
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
                break;
            default:
                event.events = EPOLLIN | /* EPOLLPRI |*/ EPOLLHUP | EPOLLRDHUP;// | EPOLLET;
        }
        err = epoll_ctl (efd, EPOLL_CTL_MOD, fd, &event);
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
        // supress valgrind warning
        event.data.u64 = 0;
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PollWait, 1, 3)
    int timeout = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(PollWait, 1);
        timeout = PARAM_INT(1);
    }

    if (PARAMETERS_COUNT > 2) {
        CREATE_ARRAY(PARAMETER(2));
    }
#ifdef WITH_SELECT_POLL
    T_HANDLE(PollWait, 0);
    PollContainer *efd = (PollContainer *)(SYS_INT)PARAM(0);
    CREATE_ARRAY(RESULT);
    if (PARAMETERS_COUNT > 2)
        efd->Wait(Invoke, RESULT, timeout, PARAMETER(2));
    else
        efd->Wait(Invoke, RESULT, timeout, NULL);
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollWait, 0);
    int efd = PARAM_INT(0);
    if (efd > 0) {
        int maxevents = 8196;
        CREATE_ARRAY(RESULT);
#ifdef WITH_KQUEUE
        struct kevent *events = (struct kevent *)malloc(sizeof(struct kevent) * maxevents);
        if (!events) {
            RETURN_NUMBER(0);
            return (void *)"PollWait: Out of memory";
        }

        int nev = kevent(efd, NULL, 0, events, maxevents, NULL);
        INTEGER out_index = 0;
        INTEGER index = 0;
        for (INTEGER i = 0; i < nev; i++) {
            if (PARAMETERS_COUNT > 2) {
                if (events[i].filter == EVFILT_WRITE) {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident);
                } else {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 
                }
            } else {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].ident); 
            }
        }
        free(events);
#else
        struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * maxevents);
        if (events) {
            CREATE_ARRAY(RESULT);
            int nfds = epoll_wait(efd, events, maxevents, timeout);
            INTEGER index = 0;
            INTEGER out_index = 0;
            for (int i = 0; i < nfds; i++) {
                if (PARAMETERS_COUNT > 2) {
                    if (events[i].events & EPOLLOUT)
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);

                    if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLERR))
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);
                } else {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);
                }
            }
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
