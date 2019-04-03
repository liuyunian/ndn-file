#ifndef CLIENT_H_
#define CLIENT_H_
#include <string>
#include <fstream>

#include <ndn-cxx/face.hpp>

class Client{
public:
    Client(std::string & prefix);

    void run();

private:
    void onData(const ndn::Data & data);
    void onNack();
    void onTimeOut();

    void schedulePackets();
    void sendInterest(ndn::Name & interestName);

private:
    std::string m_prefix;
    uint32_t m_maxSeq = 0;
    std::ofstream m_fout;

    ndn::Face m_face;
};
#endif