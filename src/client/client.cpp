#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cassert>

#include <ndn-cxx/interest.hpp>

#include "client.h"
Client::Client(std::string & prefix, std::string & filePath) : 
    m_prefix(prefix),
    m_filePath(filePath){}

void Client::run(){
    ndn::Name interestName(m_prefix);
    sendInterest(interestName.append("size"));

    m_face.processEvents();
}

void Client::schedulePackets(){
    for(u_int64_t seg = 0; seg < m_maxSeq; seg ++){
        ndn::Name interestName(m_prefix);
        sendInterest(interestName.appendNumber(seg));
    }
}

void Client::sendInterest(ndn::Name & interestName){
    ndn::Interest interest(interestName);
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(ndn::time::milliseconds(5000));
    interest.setNonce(std::rand());

    try {
        m_face.expressInterest(interest,
        std::bind(&Client::onData, this, _2), 
        std::bind(&Client::onNack, this), 
        std::bind(&Client::onTimeOut, this));

        // std::cout << "send interest: " << interest.getName() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void Client::onData(const ndn::Data & data){
    ndn::Name dataName = data.getName();
    // std::cout << "receive data: " << dataName << std::endl;
    std::string dataType = dataName.at(-1).toUri();
    if(dataType == "size"){
        const ndn::Block & content = data.getContent();
        const char * infor = reinterpret_cast<const char*>(content.value());
        m_maxSeq = strtoul(infor, NULL, 10);

        if(m_maxSeq > 0){
            schedulePackets();
        }
        else{
            std::cerr << "no file" << std::endl;
        }
    }
    else{
        std::string filePath(m_filePath); 
        std::string fileName = dataName.at(-2).toUri();
        std::ofstream fout;
        if(dataType == "%00"){
            fout.open(filePath.append(fileName), std::ios::out | std::ios::trunc);
            
        }
        else{
            fout.open(filePath.append(fileName), std::ios::out | std::ios::app);
        }
        assert(fout.is_open());

        const ndn::Block & content = data.getContent();
        for (size_t i = 0; i < content.value_size(); ++i) {
            fout << (content.value())[i];
        }

        fout.clear();
        fout.close();
    }
}

void Client::onNack(){
    std::cerr << " receive Nack " << std::endl;
}

void Client::onTimeOut(){
    std::cerr << " time out " << std::endl;
}