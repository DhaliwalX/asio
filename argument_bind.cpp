#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

// Timer will fire once a second.
// This program shows how to pass additional arguments to the handler
// function

void print(const boost::system::error_code &e,
        boost::asio::deadline_timer *t, int *count)
{
    // this tutorial uses a counter to stop running when
    // the timer fires for the sixth time. However there is no explicit
    // call to ask the io_service to stop.
    if (*count < 5)
    {
        std::cout << *count << std::endl;
        ++(*count);

        // Next we move the expiry time for the timer along by one second
        // from the previous expiry time. By calculating the new expiry time
        // relative to the old, we can ensure that the timer does no drift away
        // from the whole-second mark due to any delays in processing the 
        // handler
        t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
        
        // then we start a new asynchronous wait on the timer. 
        // Now boost::bind() function is used to associate the extra parameters
        // with callback handler. The deadline_timer::async_wait() function
        // expects a handler function with the signature
        //            
        //              void(const boost::system::error_code&)
        //
        //
        t->async_wait(boost::bind(print,
                    boost::asio::placeholders::error, t, count));
    }
}


int main(int argc, char *argv[])
{
    int time;
    boost::program_options::options_description desc("Asynchronous timer");
    boost::program_options::positional_options_description p;
    boost::asio::io_service io; // required to achieve I/O
    int count = 0;

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
    t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));

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


