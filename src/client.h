#pragma once
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

using DispatchTable = std::unordered_map<std::string,
      std::function<void(std::istringstream)>>;

namespace p2p
{
    class Client
    {
        public:
            Client(int port);
        private:
            DispatchTable dispatch_table;
            std::istringstream inputStream;
    };
} // p2p
