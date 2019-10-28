#include "peer.h"

#include <cstdlib>
#include <memory>
#include <cstdio>
#include <experimental/coroutine>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;
namespace posix = boost::asio::posix;

namespace p2p
{
    Peer::Peer(tcp::socket socket): socket(std::move(socket)),
        timer(this->socket.get_executor())
    {
        timer.expires_at(std::chrono::steady_clock::time_point::max());
    }

    Peer::~Peer()
    {
        close();
    }

    void Peer::start()
    {
        try {
            co_spawn(socket.get_executor(), [self = shared_from_this()]{
                    return self->send();
                    }, detached);

            co_spawn(socket.get_executor(), [self = shared_from_this()]{
                    return self->recv();
                    }, detached);
        } catch(std::exception& e)
        {
            std::fprintf(stderr, "Exception: %s\n", e.what());
        }
    }

    void Peer::close()
    {
        if(is_open())
        {
            std::printf("Peer %s terminated the connection.\n",
                socket.remote_endpoint().address().to_string().c_str());
            socket.close();
            timer.cancel();
        }
    }

    bool Peer::is_open() const
    {
        return socket.is_open();
    }

    const std::string Peer::to_string() const
    {
        std::ostringstream out;
        out << remote().address().to_string() << ':' << remote().port();
        return out.str();
    }

    void Peer::send(const std::string& msg)
    {
        this->msg = msg;
        timer.cancel_one(); // signals reader
    }

    const tcp::endpoint Peer::local() const
    {
        return socket.local_endpoint();
    }

    const tcp::endpoint Peer::remote() const
    {
        return socket.remote_endpoint();
        /*
        boost::system::error_code ec;
        auto endpoint = socket.remote_endpoint(ec);
        return endpoint;
        */
    }

    awaitable<void> Peer::send()
    {
        try { while(is_open())
        {
            if (msg.empty())
            {
                boost::system::error_code ec;
                co_await timer.async_wait(redirect_error(use_awaitable, ec));
            }
            else
            {
                co_await boost::asio::async_write(socket,
                        boost::asio::buffer(msg), use_awaitable);
                msg.clear();
            }
        }} catch (std::exception& e)
        {
            close();
        }
    }

    awaitable<void> Peer::recv()
    {
        try
        {
            for (std::array<char, 100> read_msg;;)
            {
                std::size_t n = co_await socket.async_read_some(
                        boost::asio::buffer(read_msg), use_awaitable);

                std::printf(std::string(R"IN(
Message recieved from %s
Sender's Port: %hu
Message: %s
)IN").c_str(),
                        socket.remote_endpoint().address().to_string().c_str(),
                        socket.remote_endpoint().port(),
                        std::string(read_msg.data(), n).c_str());
            }
        } catch (std::exception& e)
        {
            close();
        }
    }
}
