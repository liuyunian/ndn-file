#ifndef SERVER_H_
#define SERVER_H_
#include <string>
#include <map>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>


class Server{
public:
    Server(std::string & prefix, size_t maxSegmentSize, std::string & filePath);

    void run();

private:
    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name & prefix, const std::string & reason);

    // @brief 获取目录下的文件列表
    void getFileList();

    // @brief 将文件夹下的文件封装成Data包
    void populateStore();

    // @brief 将文件列表转换成XML格式的字符
    void getFileList_XML(std::string & str_xml);

    struct FileInfor{
        std::string fileName;
        off_t fileSize;
        uint32_t fileMaxSeq;
    };

private:
    std::string m_prefix;
    size_t m_maxSegmentSize;
    std::string m_filePath;
    // std::vector<std::string> m_fileList;
    std::vector<std::shared_ptr<FileInfor>> m_fileList;
    std::shared_ptr<ndn::Data> m_fileListData;
    std::map<std::string, std::shared_ptr<std::vector<std::shared_ptr<ndn::Data>>>> m_store;

    ndn::Face m_face;
    ndn::KeyChain m_keyChain;
};

#endif