#include <iostream>
#include <cassert>

#include "server.h"

Server::Server(std::string & prefix, size_t maxSegmentSize, std::istream& is) : 
    m_prefix(prefix),
    m_maxSegmentSize(maxSegmentSize){
    
    populateStore(std::cin);
}

void Server::run(){
    std::cout << "SERVER IS LISTEN: " << m_prefix << std::endl;

    try {
        m_face.setInterestFilter(
            ndn::Name(m_prefix),
            bind(&Server::onInterest, this, _2),
            nullptr,
            bind(&Server::onRegisterFailed, this, _1, _2)
        );

        m_face.processEvents();     
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    } 
}

void Server::onInterest(const ndn::Interest & interest){
    ndn::Name interestName = interest.getName();
    std::string clientRequest = interestName.at(-1).toUri();

    if(clientRequest == "size"){
        m_face.put(*(m_store.back()));
    }
    else{
        const auto segmentNo = static_cast<size_t>(interestName.at(-1).toNumber());
        if (segmentNo < m_store.size()-1){
            m_face.put(*(m_store[segmentNo]));
        }
    }
}

void Server::onRegisterFailed(const ndn::Name & prefix, const std::string & reason)
{
    std::cerr << "Prefix = " 
              << prefix 
              << "Registration Failure. Reason = " 
              << reason << std::endl;
}

void Server::populateStore(std::istream& is){
    assert(m_store.empty()); //当存储的不为空时报错

    std::vector<uint8_t> buffer(m_maxSegmentSize);
    while(is.good()){
        is.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        const auto nCharsRead = is.gcount();

        if (nCharsRead > 0) {
            auto data = std::make_shared<ndn::Data>(ndn::Name(m_prefix).appendNumber(m_store.size()));
            data->setFreshnessPeriod(ndn::time::milliseconds(2000));
            data->setContent(buffer.data(), static_cast<size_t>(nCharsRead));
             m_store.push_back(data);
        }
    }

    auto data = std::make_shared<ndn::Data>(ndn::Name(m_prefix).append("size"));
    data->setFreshnessPeriod(ndn::time::milliseconds(2000));
    std::string size = std::to_string(m_store.size());
    data->setContent(reinterpret_cast<const uint8_t *>(&size[0]), size.size());
    m_store.push_back(data);

    for (const auto & data : m_store) {
        m_keyChain.sign(*data, ndn::signingWithSha256());
    }
}
