#include <iostream>
#include <boost/program_options.hpp>

#include "client.h"

void usage(const boost::program_options::options_description &options){
    std::cout << "Usage: sudo build/client [options] <prefix>" << std::endl;
    std::cout << options;
    exit(0);
}

int main(int argc, char * argv[]){
    std::string prefix;
    std::string filePath = "download/";

    namespace po = boost::program_options;
    
    po::options_description visibleOptDesc("Allowed options");
    visibleOptDesc.add_options()("help,h", "print this message and exit")
                                ("prefix,p", po::value<std::string>(), "root prefix")
                                ("directory,d", po::value<std::string>(), "File save path");

    po::variables_map optVm;    
    store(parse_command_line(argc, argv, visibleOptDesc), optVm);

    if(optVm.count("help")){
        usage(visibleOptDesc);
    }
    
    if(optVm.count("prefix")){
        prefix = optVm["prefix"].as<std::string>();
    }

    if(optVm.count("directory")){
        filePath = optVm["directory"].as<std::string>();
        if(filePath.back() != '/'){
            filePath.append("/");
        }
    }

    Client client(prefix, filePath);
    client.run();
    return 0;
}