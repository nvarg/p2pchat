#include "client.h"

namespace p2p
{

Client::Client(int port): port(port)
{
        dispatch_table.insert("help",
                []{
                });

        dispatch_table.insert("myip",
                []{
                });

        dispatch_table.insert("myport",
                []{
                });

        dispatch_table.insert("connect",
                []{
                });

        dispatch_table.insert("list",
                []{
                });

        dispatch_table.insert("terminate",
                []{
                });

        dispatch_table.insert("send",
                []{
                });

        dispatch_table.insert("exit",
                []{
                });
}

}

