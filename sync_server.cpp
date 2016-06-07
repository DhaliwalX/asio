#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// create a string to send back to the client
std::string make_daytime_string()
{
    time_t now = time(0);
    return ctime(&now);
}

int main()
{
    try {
        boost::asio::io_service io; // REQUIRED

        // an ip::tcp::acceptor object needs to be created to listen for new
        // connections. It is initialised to listen on TCP port 13
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));

        // this is an iterative server, which means that it will handle one
        // connection at a time. Create a socket that represent the connection
        // to the client and wait for connection
        for (;;) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            std::string message = make_daytime_string();

            boost::system::error_code error;
            boost::asio::write(socket, boost::asio::buffer(message), error);
        }
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}


