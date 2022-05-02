#include <iostream>
#include "httplib.h"
#include <map>
#include <string>
#include "SimplePocoHandler.h"

// internal storage
std::map<std::string, std::set<std::string>> env;

auto main() -> int
{
    httplib::Server svr;
    SimplePocoHandler handler("localhost", 5672);
    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
    AMQP::Channel channel(&connection);
    channel.declareQueue("hello");

    channel.consume("hello", AMQP::noack).onReceived([](const AMQP::Message &message, [[maybe_unused]] uint64_t deliveryTag, [[maybe_unused]] bool redelivered) {
        std::string msg;
        for (size_t i = 0; i < message.bodySize(); ++i)
            msg += message.body()[i];
        std::cout << " [x] Received [" << msg << "]," << message.bodySize() << std::endl;
        auto pos = msg.find(':');
        if (pos != std::string::npos)
        {
            std::string id = msg.substr(0, pos);
            std::string val = msg.substr(pos + 1);

            if (env.find(id) != env.end())
            {
                env[id].insert(val);
            } else {
                env[id] = {val};
            }

            std::cout << "id:" << id;
            for(auto& v : env[id])
                std::cout << " " << v;
            std::cout << std::endl;
        }
    });

    svr.Get("/all", [&]([[maybe_unused]] const httplib::Request &req, httplib::Response &res) {
        std::string result;

        for (auto &kv : env)
        {
            result += kv.first + ":";
            for (auto &val : kv.second)
            {
                result += val + " ";
            }
            result += "\n";
        }
        res.set_content(result, "text/plain");
    });

    std::thread rabbit_loop([&]() {
        std::cout << "Processing rabbit msgs" << std::endl;
        while (true)
            handler.loop();
    });

    std::cout << "Listening on port 8082" << std::endl;
    svr.listen("0.0.0.0", 8082);

    rabbit_loop.join();
    return 1;
}