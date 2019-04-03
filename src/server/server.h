#ifndef SERVER_H_
#define SERVER_H_
#include <string>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>


class Server{
public:
    Server(std::string & prefix, size_t maxSegmentSize, std::istream& is);

    void run();

private:
    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name & prefix, const std::string & reason);

    void populateStore(std::istream& is);

private:
    std::string m_prefix;
    size_t m_maxSegmentSize;
    std::vector<std::shared_ptr<ndn::Data>> m_store;

    ndn::Face m_face;
    ndn::KeyChain m_keyChain;
};

#endif