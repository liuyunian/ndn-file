#ifndef CLIENT_H_
#define CLIENT_H_
#include <string>

#include <ndn-cxx/face.hpp>
#include <QObject>
#include <QString>

#include "threadpool.h"

class Client : public QObject{
    Q_OBJECT

public:
    Client(const std::string & prefix);

    Client(const std::string & prefix, const std::string & name, const std::string & path, uint64_t maxSeq);

    // @brief 请求服务器文件列表
    void requestFileList();

    // @brief 请求指定的文件
    void requestFile();

private:
    void onData(const ndn::Data & data);
    void onNack();
    void onTimeOut(const ndn::Interest & interest);

    void batchSendInterest(u_int64_t start, u_int64_t end);
    void sendInterest(ndn::Name & interestName);

    void writeToFile();

signals:
    void displayFileListInfor(QString);

    void displayProgress(QString, int);

    void fileCompleted(QString);

private:
    std::string m_prefix;
    std::string m_downloadPath;
    std::string m_fileName;
    uint64_t m_maxSeq = 0;
    bool m_done = false;
    
    std::map<uint64_t, std::shared_ptr<const ndn::Data>> m_dataCache;
    ThreadPool * m_pool;

    ndn::Face m_face;
};
#endif