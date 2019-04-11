#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cassert>

#include <ndn-cxx/interest.hpp>

#include "client.h"
#include <src/client/client.moc>

Client::Client(const std::string & prefix) : 
    m_prefix(prefix){}

Client::Client(const std::string & prefix, const std::string & name, const std::string & path, uint32_t maxSeq) :
    m_prefix(prefix),
    m_fileName(name),
    m_downloadPath(path),
    m_maxSeq(maxSeq){
        if(m_downloadPath.back() != '/'){
            m_downloadPath.append("/");
        }
    }

void Client::requestFileList(){
    ndn::Name interestName(m_prefix);
    sendInterest(interestName.append("fileList"));

    m_face.processEvents();
}

void Client::requestFile(){
    if(m_maxSeq > 0){
        schedulePackets();
    }
    else{
        std::cerr << "no file" << std::endl;
        return;
    }
    m_face.processEvents();
}

void Client::schedulePackets(){
    for(u_int64_t seg = 0; seg < m_maxSeq; seg ++){
        ndn::Name interestName(m_prefix);
        sendInterest(interestName.append(m_fileName).appendNumber(seg));
    }
}

void Client::sendInterest(ndn::Name & interestName){
    ndn::Interest interest(interestName);
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(ndn::time::milliseconds(2000));
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
    if(dataType == "fileList"){
        const ndn::Block & content = data.getContent();
        std::string fileListInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        // std::cout << fileListInfor << std::endl;
        emit displayFileListInfor(QString::fromStdString(fileListInfor)); 
    }
    else{
        std::string filePath(m_downloadPath); 
        std::string fileName = dataName.at(-2).toUri();
        filePath.append(fileName);

        std::ofstream fout;
        if(dataType == "%00"){
            fout.open(filePath, std::ios::out | std::ios::trunc);  
        }
        else{
            fout.open(filePath, std::ios::out | std::ios::app);
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