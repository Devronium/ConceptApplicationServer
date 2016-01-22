#ifndef __INTERFACE_H
#define __INTERFACE_H

#include <string>
#include <vector>

struct DataHolder {
    std::string ip;
    std::string mask;
    std::string mac;
    std::string gateway;
    std::string adapter;
    std::string description;
    int         flags;
    int         type;
};

int getInterfaces(std::vector<DataHolder> *holder);
#endif
