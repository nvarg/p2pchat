#include "chat.h"
#include "peer.h"
#include "myip.hpp"

#include <memory>
#include <algorithm>
#include <experimental/coroutine>
#include <boost/asio.hpp>
#include <boost/throw_exception.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;

namespace posix = boost::asio::posix;

namespace p2p
{
    Chat::Chat(unsigned short port): listeningEndpoint({boost::asio::ip::address::from_string(myip()), port}), ioc(1)
    {
        initDispatchTable();

        try {
            co_spawn(ioc, [&, port] {
                    return listener({ioc, {tcp::v4(), port}});
                    }, detached);

            co_spawn(ioc, [&]{
                    return inputStreamer({ioc, ::dup(STDIN_FILENO)});
                    }, detached);

            boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&](auto,auto){ ioc.stop(); });
            ioc.run();
        } catch (std::exception& e)
        {
            std::fprintf(stderr, "Exception %s\n", e.what());
        }
    }

    Chat::~Chat()
    {
    }

    awaitable<void> Chat::listener(tcp::acceptor acceptor)
    {
        for (;;)
        {
            auto peer = std::make_shared<Peer>(co_await acceptor.async_accept(use_awaitable));
            std::printf("Incoming connection from %s.\n", peer->to_string().c_str());
            peer->start();
            peerList.emplace(peerList.size(), std::move(peer));
        }
    }

    awaitable<void> Chat::inputStreamer(posix::stream_descriptor in)
    {
        std::array<char, 256> read_buf;
        std::string cmd;
        for(;;)
        {
            std::size_t n = co_await in.async_read_some(
                    boost::asio::buffer(read_buf), use_awaitable);
            cmd = std::string(read_buf.data(), n);
            dispatch(cmd);
        }
    }

    bool Chat::isValidPeer(const std::shared_ptr<Peer> peer) const
    {
            std::printf("test");
            auto endpoint = peer->remote();
            std::printf("%hu\t%hu", endpoint.port(), peer->local().port());
            if(std::find_if(peerList.begin(), peerList.end(), [&endpoint](const auto& p){
                        return p.second->is_open() && (endpoint == p.second->remote());
                        }) != peerList.end())
            {
                std::fprintf(stderr, "Could not add %s because it is already a peer.\n",
                        endpoint.address().to_string().c_str());
                return false;
            } else if (endpoint == listeningEndpoint)
            {
                std::fprintf(stderr, "Could not add %s because it is the local endpoint.\n",
                        endpoint.address().to_string().c_str());
                return false;
            }
            return true;
    }

    void Chat::dispatch(const std::string& cmd)
    {
        if(!stream)
        {
            stream.clear();
            stream.ignore(1024, '\n');
        }
        stream.str(cmd);
        std::string arg0;
        stream >> arg0;
        if(dispatchTable.find(arg0) != dispatchTable.end())
        {
            dispatchTable[arg0]();
        } else
        {
            std::fprintf(stderr, "Error: %s is not a command\n", arg0.c_str());
        }

    }

    void Chat::initDispatchTable()
    {
        dispatchTable.insert({"help",
                [&]{
                    std::printf("%s\n", std::string(R"(help                             Display information of all available commands
myip                             Display the user's real ip address
myport                           Display the port number the connection was established on
connect <ip address> <port no>   Connect to a peer using a specific port
list                             List all the connected peers
send <connection id> <message>   Send message(limit 100 characters) to peer using an unique ID
terminate                        Terminate the connection with peer on a specific port
exit                             Terminate all the connection with peers on all ports)").c_str());
                }});

        dispatchTable.insert({"myip",
                [&]{
                    std::printf("The listening connection is on %s\n",
                            listeningEndpoint.address().to_string().c_str());
                }});

        dispatchTable.insert({"myport",
                [&]{
                    std::printf("The listening port number is %hu\n", listeningEndpoint.port());
                }});

        dispatchTable.insert({"connect",
                [&]{
                    std::string address;
                    unsigned short port;
                    stream >> address;
                    if(!(stream >> port))
                    {
                        std::fputs("That is not a valid port number!", stderr);
                        stream.clear();
                        stream.ignore(1024, '\n');
                    }
                    tcp::endpoint endpoint{boost::asio::ip::address::from_string(address), port};
                    if(endpoint != listeningEndpoint)
                    {
                        tcp::socket socket(ioc);
                        socket.connect(endpoint);
                        auto peer = std::make_shared<Peer>(std::move(socket));
                        peer->start();
                        peerList.emplace(peerList.size(), std::move(peer));
                    }
                }});

        dispatchTable.insert({"list",
                [&]{ std::printf("id: IP address\tPort No.\n");
                for (const auto& a : peerList)
                {
                    if(a.second->is_open())
                    {
                    const auto endpoint = a.second->remote();
                    std::printf("%d: %s\t%hu\n", a.first,
                            endpoint.address().to_string().c_str(),
                            endpoint.port());
                    }
                }
                }});

        dispatchTable.insert({"terminate",
                [&]{
                    unsigned id;
                    stream >> id;
                    if(peerList.find(id) != peerList.end() && peerList[id]->is_open())
                    {
                        peerList[id]->close();
                    } else
                    {
                        std::fprintf(stderr, "No peer has the id %u\n", id);
                    }
                }});

        dispatchTable.insert({"send",
                [&]{
                    unsigned id;
                    std::string msg;
                    stream >> id;
                    std::getline(stream, msg);
                    if(peerList.find(id) != peerList.end() && peerList[id]->is_open())
                        peerList[id]->send(msg);
                    else
                        std::fprintf(stderr, "No peer has the id %u\n", id);
                }});

        dispatchTable.insert({"exit",
                [&]{
                    exit(SIGTERM);
                }});
    }
}

