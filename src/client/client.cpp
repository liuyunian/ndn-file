#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cassert>

#include <ndn-cxx/interest.hpp>

#include "client.h"
#include <src/client/client.moc>

#define BATCH_NUM static_cast<u_int64_t>(1024) // 批量发送1024个interest
#define MAX_CACHE static_cast<u_int64_t>(10*1024) //10MB

Client::Client(const std::string & prefix) : 
    m_prefix(prefix){
        m_pool = ThreadPool::getInstance();
    }

Client::Client(const std::string & prefix, const std::string & name, const std::string & path, uint64_t maxSeq) :
    m_prefix(prefix),
    m_fileName(name),
    m_downloadPath(path),
    m_maxSeq(maxSeq){
        if(m_downloadPath.back() != '/'){
            m_downloadPath.append("/");
        }

        m_pool = ThreadPool::getInstance();

        m_dataCache = std::make_shared<std::vector<std::shared_ptr<const ndn::Data>>>();
    }

void Client::requestFileList(){
    m_pool->enqueue([this]{
        ndn::Name interestName(m_prefix);
        sendInterest(interestName.append("fileList"));
        m_face.processEvents();
    });
}

void Client::requestFile(){
    m_pool->enqueue([this]{
        if(m_maxSeq > 0 && m_maxSeq <= BATCH_NUM){
            batchSendInterest(0, m_maxSeq);
        }
        else if(m_maxSeq > BATCH_NUM){
            m_sentSeq = BATCH_NUM;
            batchSendInterest(0, BATCH_NUM);
        }
        else{
            std::cerr << "no file" << std::endl;
            return;
        }
    });
}

void Client::batchSendInterest(u_int64_t start, u_int64_t end){
    // std::cout << "[" << start << ", " << end << ")" << std::endl;
    for(u_int64_t seg = start; seg < end; seg ++){
        ndn::Name interestName(m_prefix);
        sendInterest(interestName.append(m_fileName).appendNumber(seg));
    }
    m_face.processEvents();
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
    if(dataName.at(-1).toUri() == "fileList"){
        const ndn::Block & content = data.getContent();
        std::string fileListInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        emit displayFileListInfor(QString::fromStdString(fileListInfor)); 
    }
    else{
        auto dataPtr = data.shared_from_this();
        m_dataCache->push_back(dataPtr);

        const auto receiveSeg = static_cast<u_int64_t>(dataName.at(-1).toNumber());
        if((receiveSeg+1) == m_maxSeq || (receiveSeg+1) % MAX_CACHE == 0){
            // std::cout << receiveSeg << std::endl;
            std::shared_ptr<std::vector<std::shared_ptr<const ndn::Data>>> m_dataCache_copy(m_dataCache);
            m_dataCache = std::make_shared<std::vector<std::shared_ptr<const ndn::Data>>>();

            m_pool->enqueue([this, m_dataCache_copy]{
                std::string filePath(m_downloadPath); 
                // std::string fileName = dataName.at(-2).toUri();
                filePath.append(m_fileName);
                std::ofstream fout;
                if(m_dataCache_copy->front()->getName().at(-1).toNumber() == 0){
                    fout.open(filePath, std::ios::out | std::ios::trunc);  
                }
                else{
                    fout.open(filePath, std::ios::out | std::ios::app);
                }
                assert(fout.is_open());

                for(auto iter = m_dataCache_copy->begin(); iter != m_dataCache_copy->end(); ++ iter){
                    const ndn::Block & content = (*iter)->getContent();
                    for (size_t i = 0; i < content.value_size(); ++i) {
                        fout << (content.value())[i];
                    }
                }

                fout.clear();
                fout.close();
            });
        }

        if((m_maxSeq > BATCH_NUM) && ((receiveSeg+1) % BATCH_NUM == 0)){
            m_pool->enqueue([this]{
                if((m_maxSeq-m_sentSeq) > BATCH_NUM){
                    m_sentSeq = m_sentSeq+BATCH_NUM;
                    batchSendInterest(m_sentSeq-BATCH_NUM, m_sentSeq);
                }
                else{
                    batchSendInterest(m_sentSeq, m_maxSeq);
                }
            });
        }
    }
}

void Client::onNack(){
    std::cerr << " receive Nack " << std::endl;
}

void Client::onTimeOut(){
    std::cerr << " time out " << std::endl;
}