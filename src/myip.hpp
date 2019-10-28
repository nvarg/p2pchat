#include <string>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <ifaddrs.h>
#include <iterator>
#include <fstream>

std::string myip()
{
    std::ifstream routeFile("/proc/net/route", std::ios_base::in);
    if (!routeFile.good()) return "";

    std::string line;
    std::vector<std::string> tokens;
    std::string defaultInterface;
    while(std::getline(routeFile, line))
    {
        std::istringstream stream(line);
        std::copy(std::istream_iterator<std::string>(stream),
                std::istream_iterator<std::string>(),
                std::back_inserter<std::vector<std::string> >(tokens));

        // the default interface is the one having the second
        // field, Destination, set to "00000000"
        if ((tokens.size() >= 2) && (tokens[1] == std::string("00000000")))
        {
            defaultInterface = tokens[0];
            break;
        }

        tokens.clear();
    }

    routeFile.close();

    typedef struct ifaddrs ifaddrs_t;
    ifaddrs_t* ifap;
    int status = getifaddrs(&ifap);

    for(const auto* current = ifap; (current = current->ifa_next);)
    {
        if (defaultInterface == current->ifa_name
                && current->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *addr_in = (struct sockaddr_in *)current->ifa_addr;
            return std::string(inet_ntoa(addr_in->sin_addr));
        }
    }

    return "";

}
