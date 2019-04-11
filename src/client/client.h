#ifndef CLIENT_H_
#define CLIENT_H_
#include <string>

#include <ndn-cxx/face.hpp>
#include <QObject>
#include <QString>

class Client : public QObject{
    Q_OBJECT //如果要用信号与槽机制必须要加这句话

public:
    Client(const std::string & prefix);

    Client(const std::string & prefix, const std::string & name, const std::string & path, uint32_t maxSeq);

    // @brief 请求服务器文件列表
    void requestFileList();

    // @brief 请求指定的文件
    void requestFile();

private:
    void onData(const ndn::Data & data);
    void onNack();
    void onTimeOut();

    void schedulePackets();
    void sendInterest(ndn::Name & interestName);

signals:
    void displayFileListInfor(QString);

private:
    std::string m_prefix;
    std::string m_downloadPath;
    std::string m_fileName;
    uint32_t m_maxSeq = 0;

    ndn::Face m_face;
};
#endif