#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;

namespace p2p
{
    class Peer : public std::enable_shared_from_this<Peer>
    {
        public:
            Peer(tcp::socket);
            ~Peer();

            void send(const std::string& msg);
            const tcp::endpoint remote() const;
            const tcp::endpoint local() const;
            void start();
            void close();
            bool is_open() const;
            const std::string to_string() const;
        private:
            awaitable<void> send();
            awaitable<void> recv();
            tcp::socket socket;
            boost::asio::steady_timer timer;
            std::string msg;
    };
} // p2p
