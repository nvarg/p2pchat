#include <sstream>
#include <iostream>

#include "chat.h"

int main(int argc, char** argv)
{
    int port;
    if (argc == 2)
        /* The command line interface is only defined
         * for "$0 <port number>"
         * */
    {
        std::istringstream stream{argv[1]};
        stream >> port;
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <port #>\n";
        return -1;
    }
    p2p::Chat chat{port};
    return 0;
}
