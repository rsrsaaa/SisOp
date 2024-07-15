#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>
#include <netinet/in.h>

#define DISCOVER_PORT 53000
#define STATUS_PORT 52000

struct managementTable
{
    std::string name = "";
    std::string ip = "";
    std::string mac = "";
    std::string status = "";
    int port = 0;
};

extern managementTable table[3];

void clearscreen();

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg)   \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif // GLOBAL_HPP
