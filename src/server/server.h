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
    Server(std::string & prefix, u_int64_t maxSegmentSize, std::string & filePath);

    void run();

private:
    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name & prefix, const std::string & reason);

    // // @brief 处理文件夹下的所有文件
    // void populateStore();

    // @brief 读取文件内容并封装成Data包
    void makeFileData(const std::string & fileName, u_int64_t begin);
 
    // @brief 获取目录下的文件列表
    void getFileList();

    // @brief 将文件列表转换成XML格式的字符
    void getFileList_XML(std::string & str_xml);

    struct FileInfor{
        off_t fileSize;
        uint32_t fileMaxSeq;

        // u_int64_t readProgress = 0;
    };

private:
    std::string m_prefix;
    u_int64_t m_maxSegmentSize;
    std::string m_filePath;
    // std::vector<std::shared_ptr<FileInfor>> m_fileList;
    std::map<std::string, std::shared_ptr<FileInfor>> m_fileList;
    std::shared_ptr<ndn::Data> m_fileListData;
    std::map<std::string, std::shared_ptr<std::vector<std::shared_ptr<ndn::Data>>>> m_store;

    ndn::Face m_face;
    ndn::KeyChain m_keyChain;
};

#endif