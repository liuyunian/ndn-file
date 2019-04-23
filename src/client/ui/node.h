#ifndef NODE_HPP
#define NODE_HPP

#include <QGroupBox>
#include <QStandardItemModel>
#include <string>
#include <map>
#include <memory>
#include "client.h"

namespace Ui {
class Node;
}

class Node : public QGroupBox
{
    Q_OBJECT

public:
    Node(const std::string & name, const std::string & prefix, QWidget *parent = 0);

    ~Node();

    // @brief 获取节点名称
    inline std::string getNodeName(){return m_name;}

    // @brief 获取节点监听的前缀
    inline std::string getNodePrefix(){return m_prefix;}

private slots:
    void on_displayFileListInfor(QString);

    void ClickDownloadButton();

private:
    Ui::Node *ui;
    QStandardItemModel * m_fileListModel;

    std::string m_name;
    std::string m_prefix;
    std::map<std::string, uint32_t> m_fileList;
    
    // Client * m_client;
    std::unique_ptr<Client> m_client;
};

#endif // NODE_HPP
