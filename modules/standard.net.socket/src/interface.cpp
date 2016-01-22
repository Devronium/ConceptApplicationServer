#include "interface.h"

#ifdef WIN32
 #include <stdio.h>
 #include <Windows.h>
 #include <Iphlpapi.h>

int getInterfaces(std::vector<DataHolder> *holder) {
    PIP_ADAPTER_INFO AdapterInfo;
    DWORD            dwBufLen  = sizeof(AdapterInfo);
    char             *mac_addr = (char *)malloc(17);

    AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (AdapterInfo == NULL)
        return -1;

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(AdapterInfo);
        AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
        if (AdapterInfo == NULL)
            return -1;
    }

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        do {
            sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    pAdapterInfo->Address [0], pAdapterInfo->Address [1],
                    pAdapterInfo->Address [2], pAdapterInfo->Address [3],
                    pAdapterInfo->Address [4], pAdapterInfo->Address [5]);
            DataHolder d;

            d.ip          = pAdapterInfo->IpAddressList.IpAddress.String;
            d.mask        = pAdapterInfo->IpAddressList.IpMask.String;
            d.mac         = mac_addr;
            d.gateway     = pAdapterInfo->GatewayList.IpAddress.String;
            d.adapter     = pAdapterInfo->AdapterName;
            d.description = pAdapterInfo->Description;
            d.flags       = pAdapterInfo->DhcpEnabled;
            d.type        = pAdapterInfo->Type;
            holder->push_back(d);

            IP_ADDR_STRING *ips = pAdapterInfo->IpAddressList.Next;
            while (ips) {
                d.ip   = ips->IpAddress.String;
                d.mask = ips->IpMask.String;

                holder->push_back(d);
                ips = ips->Next;
            }

            pAdapterInfo = pAdapterInfo->Next;
        } while (pAdapterInfo);
    }
    free(AdapterInfo);
    return 0;
}

#else
 #ifdef linux
  #include <arpa/inet.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
  #include <ifaddrs.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <string.h>

void print_ip(struct ifaddrs *ifaddrs_ptr, void *addr_ptr, std::string *s) {
    if (addr_ptr) {
        char address [INET6_ADDRSTRLEN];
        inet_ntop(ifaddrs_ptr->ifa_addr->sa_family, addr_ptr, address, sizeof(address));
        *s = address;
    } else {
        *s = "";
    }
}

void *get_addr_ptr(struct sockaddr *sockaddr_ptr) {
    void *addr_ptr = 0;

    if (sockaddr_ptr->sa_family == AF_INET)
        addr_ptr = &((struct sockaddr_in *)sockaddr_ptr)->sin_addr;
    else
    if (sockaddr_ptr->sa_family == AF_INET6)
        addr_ptr = &((struct sockaddr_in6 *)sockaddr_ptr)->sin6_addr;
    return addr_ptr;
}

void print_internet_address(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    void *addr_ptr;

    if (!ifaddrs_ptr->ifa_addr)
        return;
    addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_addr);
    print_ip(ifaddrs_ptr, addr_ptr, &d->ip);
}

void print_netmask(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    void *addr_ptr;

    if (!ifaddrs_ptr->ifa_netmask) {
        d->mask = "";
        return;
    }
    addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_netmask);
    print_ip(ifaddrs_ptr, addr_ptr, &d->mask);
}

void print_internet_interface(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    char mac_addr [18];

    mac_addr [17] = 0;
    print_internet_address(ifaddrs_ptr, d);
    print_netmask(ifaddrs_ptr, d);

    /* P2P interface destination */
    if (ifaddrs_ptr->ifa_dstaddr) {
        void *addr_ptr;
        addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_dstaddr);
        print_ip(ifaddrs_ptr, addr_ptr, &d->gateway);
    }

    /* Interface broadcast address */
    if (ifaddrs_ptr->ifa_broadaddr) {
        void *addr_ptr;
        addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_broadaddr);
        print_ip(ifaddrs_ptr, addr_ptr, &d->gateway);
    }

    int family = ifaddrs_ptr->ifa_addr->sa_family;
    if (family == PF_PACKET) {
        struct sockaddr *sdl = (struct sockaddr *)(ifaddrs_ptr->ifa_addr);
        unsigned char   *ptr = (unsigned char *)sdl->sa_data;
        ptr += 10;
        sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
        d->mac = mac_addr;
    }
}

void print_ifaddrs(struct ifaddrs *ifaddrs_ptr, std::vector<DataHolder> *holder) {
    while (ifaddrs_ptr) {
        DataHolder d;
        d.adapter     = ifaddrs_ptr->ifa_name;
        d.description = ifaddrs_ptr->ifa_name;
        d.flags       = ifaddrs_ptr->ifa_flags;
        if (ifaddrs_ptr->ifa_addr) {
            d.type = ifaddrs_ptr->ifa_addr->sa_family;

            if ((ifaddrs_ptr->ifa_addr->sa_family == AF_INET) || (ifaddrs_ptr->ifa_addr->sa_family == AF_INET6) || (ifaddrs_ptr->ifa_addr->sa_family == PF_PACKET)) {
                print_internet_interface(ifaddrs_ptr, &d);
                holder->push_back(d);
            }
        }
        ifaddrs_ptr = ifaddrs_ptr->ifa_next;
    }
}

 #else
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <ifaddrs.h>
  #include <net/if.h>
  #include <net/if_dl.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <net/if_types.h>
  #include <string.h>

void print_ip(struct ifaddrs *ifaddrs_ptr, void *addr_ptr, std::string *s) {
    if (addr_ptr) {
        char address [INET6_ADDRSTRLEN];
        inet_ntop(ifaddrs_ptr->ifa_addr->sa_family, addr_ptr, address, sizeof(address));
        *s = address;
    } else {
        *s = "";
    }
}

void *get_addr_ptr(struct sockaddr *sockaddr_ptr) {
    void *addr_ptr = 0;

    if (sockaddr_ptr->sa_family == AF_INET)
        addr_ptr = &((struct sockaddr_in *)sockaddr_ptr)->sin_addr;
    else
    if (sockaddr_ptr->sa_family == AF_INET6)
        addr_ptr = &((struct sockaddr_in6 *)sockaddr_ptr)->sin6_addr;
    return addr_ptr;
}

void print_internet_address(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    void *addr_ptr;

    if (!ifaddrs_ptr->ifa_addr)
        return;
    addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_addr);
    print_ip(ifaddrs_ptr, addr_ptr, &d->ip);
}

void print_netmask(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    void *addr_ptr;

    if (!ifaddrs_ptr->ifa_netmask) {
        d->mask = "";
        return;
    }
    addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_netmask);
    print_ip(ifaddrs_ptr, addr_ptr, &d->mask);
}

void print_mac_address(const unsigned char *ptr, DataHolder *d) {
    int mac_addr_offset;

    char mac_addr [18];

    mac_addr [17] = 0;
    sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
    d->mac = mac_addr;
}

void print_af_link(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    struct sockaddr_dl *sdl;

    sdl = (struct sockaddr_dl *)ifaddrs_ptr->ifa_addr;
    if (sdl->sdl_type == IFT_ETHER)
        print_mac_address((unsigned char *)LLADDR(sdl), d);
    else
    if (sdl->sdl_type == IFT_LOOP)
        d->mac = "00:00:00:00:00:00";
    else
    if (sdl->sdl_type == IFT_USB)
        d->mac = "usb";
    else
        d->mac = "";
}

void print_internet_interface(struct ifaddrs *ifaddrs_ptr, DataHolder *d) {
    print_internet_address(ifaddrs_ptr, d);
    print_netmask(ifaddrs_ptr, d);

    /* P2P interface destination */
    if (ifaddrs_ptr->ifa_dstaddr) {
        void *addr_ptr;
        addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_dstaddr);
        print_ip(ifaddrs_ptr, addr_ptr, &d->gateway);
    }

    /* Interface broadcast address */
    if (ifaddrs_ptr->ifa_broadaddr) {
        void *addr_ptr;
        addr_ptr = get_addr_ptr(ifaddrs_ptr->ifa_broadaddr);
        print_ip(ifaddrs_ptr, addr_ptr, &d->gateway);
    }
}

void print_ifaddrs(struct ifaddrs *ifaddrs_ptr, std::vector<DataHolder> *holder) {
    while (ifaddrs_ptr) {
        DataHolder d;
        d.adapter     = ifaddrs_ptr->ifa_name;
        d.description = ifaddrs_ptr->ifa_name;
        d.flags       = ifaddrs_ptr->ifa_flags;
        d.type        = ifaddrs_ptr->ifa_addr->sa_family;

        if ((ifaddrs_ptr->ifa_addr->sa_family == AF_INET) || (ifaddrs_ptr->ifa_addr->sa_family == AF_INET6)) {
            print_internet_interface(ifaddrs_ptr, &d);
            holder->push_back(d);
        } else
        if (ifaddrs_ptr->ifa_addr->sa_family == AF_LINK) {
            print_af_link(ifaddrs_ptr, &d);
            holder->push_back(d);
        }
        ifaddrs_ptr = ifaddrs_ptr->ifa_next;
    }
}
 #endif

int getInterfaces(std::vector<DataHolder> *holder) {
    struct ifaddrs *ifaddrs_ptr;
    int            status = getifaddrs(&ifaddrs_ptr);

    if (status == -1)
        return -1;

    print_ifaddrs(ifaddrs_ptr, holder);

    freeifaddrs(ifaddrs_ptr);
    return 0;
}
#endif
