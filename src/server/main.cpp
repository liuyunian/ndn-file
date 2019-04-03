#include <iostream>
#include <boost/program_options.hpp>

#include "server.h"

void usage(const boost::program_options::options_description &options){
    std::cout << "Usage: sudo build/server [options] <prefix> \n";
    std::cout << options;
    exit(0);
}

int main(int argc, char * argv[]){
    
    std::string prefix = "/localhost";
    size_t maxSegmentSize = 1024;
    namespace po = boost::program_options;
    
    po::options_description visibleOptDesc("Allowed options");
    visibleOptDesc.add_options()("help,h", "print this message and exit")
                                ("prefix,p", po::value<std::string>(), "root prefix")
                                ("size,s", po::value<size_t>(), "maximum chunk size, in bytes");

    po::variables_map optVm;    
    store(parse_command_line(argc, argv, visibleOptDesc), optVm);

    if(optVm.count("help")){
        usage(visibleOptDesc);
    }

    if(optVm.count("prefix")){
        prefix = optVm["prefix"].as<std::string>();
    }

    if(optVm.count("size")){
        maxSegmentSize = optVm["size"].as<size_t>();
    }

    Server server(prefix, maxSegmentSize, std::cin);
    server.run();
    return 0;
}