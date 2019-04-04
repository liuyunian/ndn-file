#ifndef CLIENT_H_
#define CLIENT_H_
#include <string>

#include <ndn-cxx/face.hpp>

class Client{
public:
    Client(std::string & prefix, std::string & filePath);

    void run();

private:
    void onData(const ndn::Data & data);
    void onNack();
    void onTimeOut();

    void schedulePackets();
    void sendInterest(ndn::Name & interestName);

private:
    std::string m_prefix;
    std::string m_filePath;
    uint32_t m_maxSeq = 0;

    ndn::Face m_face;
};
#endif