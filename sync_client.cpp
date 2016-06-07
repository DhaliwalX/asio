// to implement a client application with TCP
#include <iostream>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
    try {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 0;
        }

        // required for asynchronous I/O
        boost::asio::io_service io;
        
        // Now we need to turn the server name that was specified as
        // a parameter to the application, into a TCP endpoint. To do this
        // we ude an ip::tcp::resolver object.
        tcp::resolver resolver(io);

        // a resolver takes a query obejct and turns it into a list of
        // endpoints. We construct a query using the name of the server,
        // specified in argv[1], amd the name of the service, in this case
        // "daytime".
        tcp::resolver::query query(argv[1], "daytime");
        
        // The list of endpoints is returned using an iterator of type
        //              ip::tcp::resolver::iterator
        //
        // NOTE: The default constructed ip::tcp::resolver::iterator object
        //       can be used as an end iterator
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Now we create and connect the socket. The list of endpoints obtained
        // above may contain both IPv4 and IPv6 endpointes, so we need to try each
        // of them untl we find one that works. This keeps the client program
        // independent of a specific IP version. The boost::asio::connect() function
        // does this for us automatically.
        tcp::socket socket(io);
        boost::asio::connect(socket, endpoint_iterator);

        // The connection is open now, All we need to do now is read the response
        // from the daytime service

        // Using array to hold the recieved data. 
        for (;;) {
            std::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            if (error == boost::asio::error::eof)
                break;  // connection closed by peer
            else if (error)
                throw boost::system::system_error(error); // some other error

        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

