#include <iostream>
#include "httplib.h"
#include <map>
#include <string>
#include "SimplePocoHandler.h"

// internal storage
std::map<std::string, std::string> env;

auto main() -> int
{
    httplib::Server svr;
    SimplePocoHandler handler("localhost", 5672);
    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
    
    svr.Get(R"(/command/(\d+))", [&](const httplib::Request &req, httplib::Response &res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");

        if (req.has_param("value"))
        {
            std::string id = numbers;
            std::string val = req.get_param_value("value");

            // save to command model
            env[id] = val;

            // send event to query model
            AMQP::Channel channel(&connection);    
            channel.declareQueue("hello");
            std::string msg = id+":"+val;
            channel.publish("", "hello", msg.c_str());
                                    
            res.set_content(id + ":" + val, "text/plain");
        }
        else
        {
            res.set_content("No value", "text/plain");
        }
    });

    std::thread rabbit_loop([&]() {
            std::cout << "Processing rabbit msgs" << std::endl;
            while (true)
                 handler.loop();
    });

    std::cout << "Listening on port 8081" << std::endl;
    svr.listen("0.0.0.0", 8081);

    rabbit_loop.join();
    return 1;
}