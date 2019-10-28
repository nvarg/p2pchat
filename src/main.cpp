#include <sstream>
#include <iostream>

#include <boost/asio.hpp>

#include "chat.h"

using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    unsigned short port;
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
    p2p::Chat{port};
    return 0;
}
