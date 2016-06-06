#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
// since this file uses time so we've to include Boost.Date_Time header
// file for manipulating times

// required for program_options
#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
    // create a description object that will take the options
    boost::program_options::options_description desc("A simple timer: Usage");
    int time; // time on seconds;
    boost::program_options::positional_options_description p;
    // add the options
    desc.add_options()
        ("help,h", "show help message")
        ("time,t", boost::program_options::value<int>(&time)->default_value(0),
            "time in seconds for timer to run");

    p.add("time", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(desc)
                .positional(p).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    // all programs that use asio need to have at least one io_service object
    // This class service provides access to I/O functionality.
    boost::asio::io_service io; // required for I/O functionality, as "told" above

    // Next we declare an object of type boost::asio::deadline_timer. The core
    // asio classes that provide I/O functionality always take a reference to an
    // io_service as their first constructor argument. The second argument to
    // the constructor sets the timer to expire 5 seconds from now.
    boost::asio::deadline_timer t(io /* io_service object */,
                                    boost::posix_time::seconds(time));
    // In this simple example we perform a blocking wait on the timer. That is
    // the call to deadline_timer::wait() will not return until timer has
    // expired, 5 seconds after it was created (i.e. not from when the wait start
    t.wait();
    // now if the `t` has already expired then it will return immediately.

    std::cout << "Hello, World\n";
    return 0;
}

