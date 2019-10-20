#include "chat.h"

#include <memory>
#include <iostream>
#include <experimental/coroutine>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using boost::asio::experimental::awaitable;
using boost::asio::experimental::co_spawn;
using boost::asio::experimental::detached;
using boost::asio::experimental::redirect_error;
namespace this_coro = boost::asio::experimental::this_coro;


namespace p2p
{
    Chat::Chat(int): port(port)
    {
        auto listener = [&](tcp::acceptor acceptor) -> awaitable<void>{

            auto token = co_await this_coro::token();
            for (;;)
            {
                clients.insert(idx++, std::make_unique<tcp::socket>(co_await acceptor.async_accept(token)))
            }

        };


        try {
            boost::asio::io_context ioc();

            // listener
            co_spawn(ioc,
                [&]{ return listener(tcp::acceptor(ioc, {tcp::v4(), port})); },
                detatched);

            // stdin handler
       // input dispatch
       //     co_spawn(ioc,
       //         [&]{ return ; },
       //         detatched);

            boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
            signals.async_wait([&](auto,auto){ ioc.stop(); });
            ioc.run();
        } catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << '\n';
        }
    }
}

