#include <iostream>
#include <cassert>
#include <dirent.h>
#include <fstream>
#include <memory>
#include <sys/time.h>

#include "server.h"

#define MAXSIZE static_cast<u_int64_t>(10*1024)

Server::Server(std::string & prefix, u_int64_t maxSegmentSize, std::string & filePath) : 
    m_prefix(prefix),
    m_maxSegmentSize(maxSegmentSize),
    m_filePath(filePath){
    
    getFileList();

    for(const auto & fileInfor : m_fileList){
        m_store.insert({fileInfor.first, std::make_unique<std::vector<std::unique_ptr<ndn::Data>>>()});
    }
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
    std::string clientRequest = interestName.at(-1).toUri();

    if(clientRequest == "fileList"){
        std::string dataContent;
        getFileList_XML(dataContent);

        auto data = std::make_unique<ndn::Data>(interestName);
        data->setFreshnessPeriod(ndn::time::milliseconds(2000));
        data->setContent(reinterpret_cast<const uint8_t *>(&dataContent[0]), dataContent.size());
        m_keyChain.sign(*data, ndn::signingWithSha256());
        
        m_face.put(*data);
        // std::cout << "send data: " << data->getName() << std::endl;
    } 
    else{
        std::string requestFileName = interestName.at(-2).toUri();
        const auto segmentNo = static_cast<u_int64_t>(interestName.at(-1).toNumber());
        // std::cout << segmentNo << std::endl;
        if(segmentNo % MAXSIZE == 0){
            makeFileData(requestFileName, segmentNo);
        }

        // std::cout << "send data: " << m_store[requestFileName]->at(segmentNo % MAXSIZE)->getName() << std::endl;
        m_face.put(*(m_store[requestFileName]->at(segmentNo % MAXSIZE)));
    }
}

void Server::onRegisterFailed(const ndn::Name & prefix, const std::string & reason){
    std::cerr << "Prefix = " 
              << prefix 
              << "Registration Failure. Reason = " 
              << reason << std::endl;
}

void Server::makeFileData(const std::string & fileName, u_int64_t begin){
    auto file_Data = std::make_unique<std::vector<std::unique_ptr<ndn::Data>>>();
    std::vector<uint8_t> buffer(m_maxSegmentSize);

    std::string filePath(m_filePath);
    std::ifstream fin(filePath.append(fileName));
    assert(fin.is_open());
    fin.seekg(static_cast<std::streamoff>(begin*1024), std::ios::beg);

    if(((m_fileList[fileName]->fileSize) - (begin*1024)) > MAXSIZE*1024){
        // std::cout << "[" << begin << ", " << begin+MAXSIZE << ")" << std::endl;
        for(u_int64_t i = begin; i < begin+MAXSIZE; ++ i){
            fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            const auto nCharsRead = fin.gcount(); //读取个数
            if(nCharsRead > 0) {
                ndn::Name dataName = ndn::Name(m_prefix).append(fileName).appendNumber(i);
                auto data = std::make_unique<ndn::Data>(dataName);
                data->setFreshnessPeriod(ndn::time::milliseconds(2000));
                data->setContent(buffer.data(), static_cast<size_t>(nCharsRead));
                m_keyChain.sign(*data, ndn::signingWithSha256());
                file_Data->push_back(std::move(data));
            }
        }
    }
    else{
        while(fin.good()){
            fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            const auto nCharsRead = fin.gcount(); //读取个数
            if (nCharsRead > 0) {
                ndn::Name dataName = ndn::Name(m_prefix).append(fileName).appendNumber(begin + file_Data->size());
                auto data = std::make_unique<ndn::Data>(dataName);
                data->setFreshnessPeriod(ndn::time::milliseconds(2000));
                data->setContent(buffer.data(), static_cast<size_t>(nCharsRead));
                m_keyChain.sign(*data, ndn::signingWithSha256());
                file_Data->push_back(std::move(data));
            }
        }
        // std::cout << "[" << begin << ", " <<  begin + file_Data->size()<< ")" << std::endl;
    }

    fin.close();

    const auto iter = m_store.find(fileName);
    if(iter != m_store.end()){
        (*iter).second = std::move(file_Data);
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
        std::string fileName = ptr->d_name;

        struct stat buf;
        std::string filePath(m_filePath);
        stat(filePath.append(ptr->d_name).c_str(), &buf);

        auto fileInfor = std::make_unique<FileInfor>();
        fileInfor->fileSize = buf.st_size;

        // TODO FILE_TIME

        if(buf.st_size % m_maxSegmentSize  == 0){
            fileInfor->fileMaxSeq = buf.st_size / m_maxSegmentSize;
        }
        else{
            fileInfor->fileMaxSeq = buf.st_size / m_maxSegmentSize + 1;
        }

        m_fileList.insert({fileName, std::move(fileInfor)});
    }
    closedir(dir);
}

void Server::getFileList_XML(std::string & str_xml){
    str_xml += "<FileList>";
    for(auto & fileInfor : m_fileList){
        str_xml = str_xml + "<FileInfor><Name>" + fileInfor.first + "</Name>" + 
                    "<Time>Thu Feb 07 14:39:36 2019</Time>" + 
                    "<Size>" + std::to_string(fileInfor.second->fileSize) + "</Size>" + 
                    "<MaxSeq>" + std::to_string(fileInfor.second->fileMaxSeq) + "</MaxSeq></FileInfor>";
    }
    str_xml += "</FileList>";
}
