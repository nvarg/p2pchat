#pragma once

#include "peer.h"

#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
namespace posix = boost::asio::posix;

namespace p2p
{
    class Chat
    {
        public:
            Chat(unsigned short port);
            ~Chat();
        private:
            awaitable<void> listener(tcp::acceptor);
            awaitable<void> inputStreamer(posix::stream_descriptor);
            bool isValidPeer(const std::shared_ptr<Peer>) const;

            std::istringstream stream;
            void initDispatchTable();
            void dispatch(const std::string& cmd);
            std::unordered_map<std::string, std::function<void()>> dispatchTable;

            std::map<unsigned, std::shared_ptr<Peer>> peerList;
            boost::asio::io_context ioc;
            tcp::endpoint listeningEndpoint;
    };

} // namespace p2p
