#include <iostream>
#include <cassert>
#include <dirent.h>
#include <fstream>

#include "server.h"

Server::Server(std::string & prefix, size_t maxSegmentSize, std::string & filePath) : 
    m_prefix(prefix),
    m_maxSegmentSize(maxSegmentSize),
    m_filePath(filePath){
    
    getFileList();
    populateStore();
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

    // std::cout << "receive interest: " << interestName << std::endl;
    std::string requestFileName = interestName.at(-2).toUri();
    std::string clientRequest = interestName.at(-1).toUri();

    auto iter = m_store.find(requestFileName);
    if(iter != m_store.end()){
        std::vector<std::shared_ptr<ndn::Data>> fileContent = *(iter->second);
        if(clientRequest == "size"){
            m_face.put(*(fileContent.back()));
            // std::cout << "send data: " << fileContent.back()->getName() << std::endl;
        }    
        else{
            const auto segmentNo = static_cast<size_t>(interestName.at(-1).toNumber());
            if (segmentNo < fileContent.size()-1){
                m_face.put(*(fileContent[segmentNo]));
                // std::cout << "send data: " << fileContent[segmentNo]->getName() << std::endl;
            }
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

void Server::populateStore(){
    assert(m_store.empty()); //当存储的不为空时报错

    std::vector<uint8_t> buffer(m_maxSegmentSize);

    for(auto & fileName : m_fileList){
        std::string filePath(m_filePath);
        std::ifstream fin(filePath.append(fileName));
        assert(fin.is_open());
        auto file_Data = std::make_shared<std::vector<std::shared_ptr<ndn::Data>>>();

        while(fin.good()){
            fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            const auto nCharsRead = fin.gcount(); //读取个数
            if (nCharsRead > 0) {
                ndn::Name dataName = ndn::Name(m_prefix).append(fileName).appendNumber(file_Data->size());
                auto data = std::make_shared<ndn::Data>(dataName);
                data->setFreshnessPeriod(ndn::time::milliseconds(5000));
                data->setContent(buffer.data(), static_cast<size_t>(nCharsRead));
                m_keyChain.sign(*data, ndn::signingWithSha256());
                file_Data->push_back(data);
            }
        }
        fin.close();

        auto data = std::make_shared<ndn::Data>(ndn::Name(m_prefix).append(fileName).append("size"));
        data->setFreshnessPeriod(ndn::time::milliseconds(5000));
        std::string size = std::to_string(file_Data->size());
        data->setContent(reinterpret_cast<const uint8_t *>(&size[0]), size.size());
        m_keyChain.sign(*data, ndn::signingWithSha256());
        file_Data->push_back(data);

        m_store.insert(std::pair<std::string, std::shared_ptr<std::vector<std::shared_ptr<ndn::Data>>>>(fileName, file_Data));
    }
}

void Server::getFileList(){
    struct dirent * ptr;    
    DIR * dir = opendir(m_filePath.c_str());
    while((ptr = readdir(dir)) != NULL)
    {
        //跳过'.'和'..'两个目录
        if(ptr->d_name[0] == '.')
            continue;
        m_fileList.push_back(ptr->d_name);
    }
 
    closedir(dir);
}
