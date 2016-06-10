#include <iostream>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <iostream>
#include <string>
#define MAX_COUNT -1

namespace po = boost::program_options;
int main(int argc, char *argv[])
{
    bool verbose = false, debug = false;
    std::vector<std::string> queries;
    po::options_description desc ("Valid options");

    desc.add_options()
        ("help,h", "Produce help message")
        ("query,q", po::value<std::vector<std::string>>()->composing(), "Queries")
        ("verbose,v", "Print a lot of output")
        ("debug,d", "Print debugging information");

    po::positional_options_description p;
    p.add("query", MAX_COUNT);

    po::variables_map vm;
    try {
        /* parse the command line options */
        po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(), vm);
        po::notify(vm);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    if (vm.count("help")) {
        std::cout << desc;
        exit(0);
    }

    if (vm.count("query")) {
        queries = vm["query"].as<std::vector<std::string>>();
    }

    if (vm.count("verbose")) {
        verbose = true;
    }

    if (vm.count("debug")) {
        debug = true;
    }

    if (debug) {
        std::cout << "Options passed were: " << std::endl;
        for (const auto &i : vm) {
            std::cout << i.first << std::endl;
        }
    }

    if (debug) {
        std::cout << "Queries are: " << std::endl;
        for (const auto &i : queries) {
            std::cout << i.substr(i.find(":") + 1) << " on " 
                << i.substr(0, i.find(":")) << std::endl;
        }
    }

    using boost::asio::ip::tcp;

    boost::asio::io_service io;
    tcp::resolver resolver(io);

    std::string address;
    std::string port;
    int j = 1;
    for (const auto &i : queries) {
        if (verbose || debug)
            std::cout << "[" << j << "]: (";
        address = i.substr(0, i.find(":"));
        port = i.substr(i.find(":") + 1);
        if (verbose || debug)
            std::cout << i << ")\n";
        tcp::resolver::query query(address, port);
        tcp::resolver::iterator it; 
        try {
            it = resolver.resolve(query);
        } catch (std::exception &e) {
            std::cerr << "[" << j++ << "] " << e.what() << std::endl;
            continue;
        }
        // default constructed resolver::iterator is end marker
        tcp::resolver::iterator end;

        while (it != end) {
            tcp::endpoint point = *it++;
            std::cout << "[" << j << "] " << point.address() << ":" 
                << point.port() <<  std::endl;
        }
        j++;
    }

    
    return 0;
}

