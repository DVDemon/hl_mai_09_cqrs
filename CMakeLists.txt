cmake_minimum_required(VERSION 3.2)

project(amqp_publisher C CXX)

SET (COMMAND "command")
SET (QUERY "query")

# https://github.com/CopernicaMarketingSoftware/AMQP-CPP

find_package(Threads)
find_package(OpenSSL)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
link_directories("/usr/local/lib")

add_executable(${COMMAND} command.cpp
                                 SimplePocoHandler.cpp)
add_executable(${QUERY} query.cpp
                                 SimplePocoHandler.cpp)

target_include_directories(${COMMAND} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${COMMAND} PRIVATE -Wall -Wextra -pedantic -Werror )
target_link_libraries(${COMMAND} PUBLIC 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${OpenSSL_LIBRARIES}
                             ${Poco_LIBRARIES}
                             amqpcpp
                             OpenSSL::SSL
                             OpenSSL::Crypto)
set_target_properties(${COMMAND} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${COMMAND} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)


target_include_directories(${QUERY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${QUERY} PRIVATE -Wall -Wextra -pedantic -Werror )
target_link_libraries(${QUERY} PUBLIC 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${OpenSSL_LIBRARIES}
                             ${Poco_LIBRARIES}
                             amqpcpp
                             OpenSSL::SSL
                             OpenSSL::Crypto)
set_target_properties(${QUERY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${QUERY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)


