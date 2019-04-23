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
    u_int64_t maxSegmentSize = 1024;
    std::string filePath = "../../test/";
    namespace po = boost::program_options;
    
    po::options_description visibleOptDesc("Allowed options");
    visibleOptDesc.add_options()("help,h", "print this message and exit")
                                ("prefix,p", po::value<std::string>(), "root prefix")
                                ("size,s", po::value<u_int64_t>(), "maximum chunk size, in bytes")
                                ("directory,d", po::value<std::string>(), "shared file directory");

    po::variables_map optVm;    
    store(parse_command_line(argc, argv, visibleOptDesc), optVm);

    if(optVm.count("help")){
        usage(visibleOptDesc);
    }

    if(optVm.count("prefix")){
        prefix = optVm["prefix"].as<std::string>();
    }

    if(optVm.count("size")){
        maxSegmentSize = optVm["size"].as<u_int64_t>();
    }

    if(optVm.count("directory")){
        filePath = optVm["directory"].as<std::string>();
        if(filePath.back() != '/'){
            filePath.append("/");
        }
    }

    Server server(prefix, maxSegmentSize, filePath);
    server.run();
    return 0;
}