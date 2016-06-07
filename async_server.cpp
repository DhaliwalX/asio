#include <iostream>
#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using tcp = boost::asio::ip::tcp;

std::string make_daytime_string()
{
    time_t t = time(NULL);
    return ctime(&t);
}

class async_timer {
public:
    async_timer(boost::asio::io_service &io)
        : timer_(io, boost::posix_time::milliseconds(200)),
        count_(0), bytes_transferred_(0), requests_(0),
        update_(false), previous_request_(0), previous_transfer_(0)
    {
        timer_.async_wait(boost::bind(&async_timer::print, this));
    }

    void print()
    {
        double transfer_rate = (double)(bytes_transferred_ - previous_transfer_)
                                        / (double)(0.2);
        double requests_per_sec = (double)(requests_ - previous_request_)
                                        / (double)(0.2);

        std::cout << "\rR: " << requests_per_sec << "/s, "
            << "Transfer Rate: " << transfer_rate << " bytes/sec";
        std::cout.flush();
        timer_.expires_at(timer_.expires_at() + boost::posix_time::milliseconds(200));
        timer_.async_wait(boost::bind(&async_timer::print, this));
        count_++;
        previous_transfer_ = bytes_transferred_;
        previous_request_ = requests_;
    }

    void bytes_transfered(size_t count)
    {
        bytes_transferred_ += count;
        requests_++;
    }

private:
    boost::asio::deadline_timer timer_;
    size_t bytes_transferred_;
    size_t previous_request_;
    size_t previous_transfer_;
    bool update_;
    size_t requests_;
    size_t count_;
};

class tcp_connection
    : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_service &io)
    {
        return pointer(new tcp_connection(io));
    }

    tcp::socket &socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = make_daytime_string();
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

private:
    tcp_connection(boost::asio::io_service &io)
        : socket_(io)
    {}

    void handle_write(const boost::system::error_code &e,
            size_t )
    {
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server {
public:
    tcp_server(boost::asio::io_service &io)
        : acceptor_(io, tcp::endpoint(tcp::v4(), 13)), timer_(io)
    {
        start_accept();
    }

    void start_accept()
    {
        tcp_connection::pointer new_connection = 
            tcp_connection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
                boost::bind(&tcp_server::handle_accept, this, new_connection,
                    boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection,
            const boost::system::error_code &error)
    {
        if (!error)
        {
            new_connection->start();
            timer_.bytes_transfered(make_daytime_string().size());
        }
        
        start_accept();
    }

private:
    tcp::acceptor acceptor_;
    async_timer timer_;
};


int main(int argc, char *argv[])
{
    try {
        boost::asio::io_service io;

        tcp_server server(io);
        io.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

