#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // required for time
#include <boost/program_options.hpp>

// so when we use asio's asynchronous functionality means having callback
// function that will be called when the asynchronous operation finishes.
void print(const boost::system::error_code& e)
{
    std::cout << "Hello, World!\n" << std::endl;
}

int main(int argc, char *argv[])
{
    int time;
    boost::program_options::options_description desc("Asynchronous timer");
    boost::program_options::positional_options_description p;
    boost::asio::io_service io; // required to achieve I/O

    // timer: deadline_timer
   
    desc.add_options()
        ("help,h", "print help message")
        ("time,t", boost::program_options::value<int>(&time)->default_value(0),
            "time in seconds")
        ("debug,d", "print debug information");
    p.add("time", -1);
    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(desc)
                .positional(p).run(), vm);
    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    if (vm.count("help")) {
        std::cout << desc;
        return 0;
    }
    if (vm.count("time")) {
        time = vm["time"].as<int>();
    }

    if (vm.count("debug")) {
        std::cout << "Timer will wait for " << time << " seconds\n";
    }
    
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(time));
 
    // yup this is the asynchronous call
    t.async_wait(&print);

    // now boost says that we must call io_service::run() member function on the 
    // io. The asio library provides a guarantee that callback handlers will only
    // be called from the threads that are currently calling io_service;;run()/
    // Therefor unless the io_service::run() function is called the callback for
    // asynchronous wait completion will never be invoked.
    io.run();

    // It is important to remember to give the io_srevice some work to do before
    // calling io_service::run(). For example, id we had omitted the above
    // statement `t.async_wait()`, the io_service would not have had any work to
    // do so io.run() would have immediately returned.
    return 0;
}

