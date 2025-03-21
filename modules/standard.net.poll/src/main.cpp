//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#define POLL_MAX_EVENTS     1024

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

#ifdef WITH_SELECT
    #ifndef FD_COPY
        #define FD_COPY(dest,src) memcpy((dest),(src),sizeof *(dest))
    #endif
#endif

#ifdef WITH_KQUEUE
    #include <sys/event.h>
#endif
#ifdef WITH_EPOLL
    #include <sys/epoll.h>
#endif

#ifdef WITH_SELECT
    struct io_fd {
        int fd;
        char mode;
    };
#endif

#if defined(WITH_SELECT) || defined(WITH_POLL)
#define WITH_SELECT_POLL
class PollContainer {
private:
#ifdef WITH_SELECT
    struct io_fd *chlist;
    int chlist_len;
    int chlist_pos;
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
#if defined(WITH_POLL) || defined(WITH_SELECT)
    PollContainer() {
        this->chlist = NULL;
        this->chlist_len = 0;
        this->chlist_pos = 0;
    }
#endif

    int Add(int efd, int mode) {
#ifdef WITH_POLL
        if (chlist_pos <= chlist_len) {
            chlist_len += 64;
            chlist = (struct pollfd *)realloc(chlist, sizeof(struct pollfd) * chlist_len);
        }
        if (!chlist)
            return -2;
        chlist[chlist_pos].fd = efd;
        switch (mode & 3) {
            case 1:
                chlist[chlist_pos].events = POLLOUT;
                break;
            case 3:
                chlist[chlist_pos].events = POLLIN | POLLPRI | POLLOUT | POLLHUP | POLLRDHUP;
                break;
            default:
                chlist[chlist_pos].events = POLLIN | POLLPRI | POLLHUP | POLLRDHUP;
        }
        chlist[chlist_pos].revents = 0;
        chlist_pos++;
#endif
#ifdef WITH_SELECT
        if (chlist_pos <= chlist_len) {
            chlist_len += 64;
            chlist = (struct io_fd *)realloc(chlist, sizeof(struct io_fd) * chlist_len);
        }
        for (int i = 0; i < chlist_pos; i ++) {
            if (chlist[i].fd == efd) {
                chlist[i].mode = mode & 3;
                return 0;
            }
        }
        if (!chlist)
            return -2;
        chlist[chlist_pos].fd = efd;
        chlist[chlist_pos].mode = mode & 3;
        chlist_pos ++;
#endif
        return 0;
    }

    int Update(int efd, int mode) {
#ifdef WITH_POLL
        if (!chlist)
            return -2;
        for (int i = 0; i < chlist_pos; i ++) {
            if (chlist[i].fd == efd) {
                switch (mode & 3) {
                    case 1:
                        chlist[i].events = POLLOUT;
                        break;
                    case 3:
                        chlist[i].events = POLLIN | POLLPRI | POLLOUT | POLLHUP | POLLRDHUP;
                        break;
                    default:
                        chlist[i].events = POLLIN | POLLPRI | POLLHUP | POLLRDHUP;
                }
                return 0;
            }
        }
#endif
#ifdef WITH_SELECT
        if (!chlist)
            return -2;
        for (int i = 0; i < chlist_pos; i ++) {
            if (chlist[i].fd == efd) {
                chlist[i].mode = mode & 3;
                return 0;
            }
        }
#endif
        return -1;
    }

    int Wait(INVOKE_CALL Invoke, void *RESULT, int timeout, void *OUT_SOCKETS) {
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
        fd_set fd_excepts;
        FD_ZERO(&fd_list);
        FD_ZERO(&fd_excepts);
        if (OUT_SOCKETS) {
            FD_ZERO(&fd_out_list);
        }
#ifdef _WIN32
        for (int i = 0; i < chlist_pos; i++) {
            if (chlist[i].fd >= 0) {
                FD_SET(chlist[i].fd, &fd_list);
                FD_SET(chlist[i].fd, &fd_excepts);
            }
        }
        if (OUT_SOCKETS) {
            for (int i = 0; i < chlist_pos; i++) {
                if ((chlist[i].fd >= 0) && (chlist[i].mode & 1))
                    FD_SET(chlist[i].fd, &fd_out_list);
            }
        }
#endif

        INTEGER index = 0;
        INTEGER out_index = 0;
        int err;
        if (OUT_SOCKETS)
            err = select(FD_SETSIZE, &fd_list, &fd_out_list, &fd_excepts, &tout);
        else
            err = select(FD_SETSIZE, &fd_list, 0, &fd_excepts, &tout);
        if (err > 0) {
            for (int i = 0; i < chlist_pos; i++) {
                if (FD_ISSET(chlist[i].fd, &fd_list))
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
            }
            if (OUT_SOCKETS) {
                for (int i = 0; i < chlist_pos; i++) {
                    if (FD_ISSET(chlist[i].fd, &fd_out_list))
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, OUT_SOCKETS, out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
                }
            }
        } else
        if (err < 0) {
            for (int i = 0; i < chlist_pos; i++) {
                if (FD_ISSET(chlist[i].fd, &fd_excepts))
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)chlist[i].fd); 
            }
        }
        return err;
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
        return q;
#endif
        return 0;
    }

    int Remove(int efd) {
#if defined(WITH_POLL) || defined(WITH_SELECT)
        if ((chlist_pos) && (chlist)) {
            for (int i = 0; i < chlist_pos; i++) {
                if (chlist[i].fd == efd) {
                    chlist_pos--;
                    for (int j = i; j < chlist_pos; j++) {
                        chlist[j].fd = chlist[j + 1].fd;
#ifdef WITH_POLL
                        chlist[j].events = chlist[j + 1].events;
                        chlist[j].revents = 0;
#else
                        chlist[j].mode = chlist[j + 1].mode;
#endif
                    }
                    if (chlist_len - chlist_pos > 64) {
                        chlist_len -= 64;
#ifdef WITH_SELECT
                        chlist = (struct io_fd *)realloc(chlist, sizeof(struct io_fd) * chlist_len);
#else
                        chlist = (struct pollfd *)realloc(chlist, sizeof(struct pollfd) * chlist_len);
#endif
                    }
                    break;
                }
            }
        }
#endif
        return 0;
    }

    ~PollContainer() {
#ifdef WITH_KQUEUE
        close(fd);
#endif
#if defined(WITH_POLL) || defined(WITH_SELECT)
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
    DEFINE_SCONSTANT("POLL_ET",     "4");
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
    if (fd < 0) {
        RETURN_NUMBER(-2);
        return 0;
    }
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
        int poll_et = 0;
#ifdef WITH_KQUEUE
        struct kevent events[2];
        int num_events = 1;

        if (mode & 4)
            poll_et = EV_CLEAR;

        switch (mode & 3) {
            case 1:
                EV_SET(&events[0], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                break;
            case 3:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                EV_SET(&events[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                num_events = 2;
                break;
            default:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
        }
        err = kevent(efd, events, num_events, NULL, 0, NULL);
#else
        struct epoll_event event;
        // supress valgrind warning
        event.data.u64 = 0;
        event.data.fd = fd;

        if (mode & 4)
            poll_et = EPOLLET;

        switch (mode & 3) {
            case 1:
                event.events = EPOLLOUT | poll_et;
                break;
            case 3:
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | poll_et;
                break;
            default:
                event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | poll_et;
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
        err = efd->Update(fd, mode);
        // efd->Remove(fd);
        // err = efd->Add(fd, mode);
    }
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollUpdate, 0);
    int efd = PARAM_INT(0);
    if ((fd > 0) && (efd > 0)) {
        int poll_et = 0;

#ifdef WITH_KQUEUE
        struct kevent events[2];
        int num_events = 1;
        if (mode & 4)
            poll_et = EV_CLEAR;
        // Adds the event to the kqueue. Re-adding an existing event will modify the parameters of the original event, and not result in a duplicate entry.
        switch (mode & 3) {
            case 1:
                EV_SET(&events[0], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                break;
            case 3:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                EV_SET(&events[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
                num_events = 2;
                break;
            default:
                EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE | poll_et, 0, 0, 0);
        }
        err = kevent(efd, events, num_events, NULL, 0, NULL);
#else
        struct epoll_event event;
        // supress valgrind warning
        event.data.u64 = 0;
        event.data.fd = fd;

        if (mode & 4)
            poll_et = EPOLLET;

        switch (mode & 3) {
            case 1:
                event.events = EPOLLOUT | poll_et;
                break;
            case 3:
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | poll_et;
                break;
            default:
                event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | poll_et;
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
    if (fd < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PollWait, 1, 4)
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
    int err;
    if (PARAMETERS_COUNT > 2)
        err = efd->Wait(Invoke, RESULT, timeout, PARAMETER(2));
    else
        err = efd->Wait(Invoke, RESULT, timeout, NULL);
    if (PARAMETERS_COUNT > 3) {
        SET_NUMBER(3, err);
    }
#else
#if defined(WITH_EPOLL) || defined(WITH_KQUEUE)
    T_NUMBER(PollWait, 0);
    int efd = PARAM_INT(0);
    if (efd > 0) {
        int maxevents = POLL_MAX_EVENTS;
        CREATE_ARRAY(RESULT);
#ifdef WITH_KQUEUE
        struct kevent *events = (struct kevent *)malloc(sizeof(struct kevent) * maxevents);
        if (!events) {
            RETURN_NUMBER(0);
            return (void *)"PollWait: Out of memory";
        }

        struct timespec timeout_spec;
        if (timeout > 0) {
            timeout_spec.tv_sec = timeout / 1000;
            timeout_spec.tv_nsec = (timeout % 1000) * 1000;
        }
        int nev = kevent(efd, NULL, 0, events, maxevents, (timeout > 0) ? &timeout_spec : NULL);
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, nev);
        }
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
            if(PARAMETERS_COUNT > 3) {
                SET_NUMBER(3, nfds);
            }
            INTEGER index = 0;
            INTEGER out_index = 0;
            for (int i = 0; i < nfds; i++) {
                if (PARAMETERS_COUNT > 2) {
                    if (events[i].events & EPOLLOUT)
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), out_index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);

                    if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLERR) || (!(events[i].events & EPOLLOUT)))
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);
                } else {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index ++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)events[i].data.fd);
                }
            }
            free(events);
        } else {
            if (PARAMETERS_COUNT > 3) {
                SET_NUMBER(3, 0);
            }
            RETURN_NUMBER(0);
            return (void *)"PollWait: Out of memory";
        }
#endif
    } else {
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, 0);
        }
        RETURN_NUMBER(0);
    }
#endif
#endif
END_IMPL
//=====================================================================================//
