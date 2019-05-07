#ifndef NODE_H
#define NODE_H

#include <QGroupBox>
#include <QStandardItemModel>
#include <QProgressBar>

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
    Node(const QString & name, const std::string & prefix, QWidget *parent = 0);

    ~Node();

    // @brief 获取节点名称
    inline QString getNodeName(){return m_name;}

    // @brief 获取节点监听的前缀
    inline std::string getNodePrefix(){return m_prefix;}

private slots:
    void on_displayFileListInfor(QString);

    void on_clickDownloadButton();

    void on_displayProgress(QString, int);

    void on_fileCompleted(QString);

private:
    Ui::Node *ui;
    QStandardItemModel * m_fileListModel;

    QString m_name;
    std::string m_prefix;

    std::map<QString, uint64_t> m_fileInforStore;
    std::map<QString, std::unique_ptr<QProgressBar>> m_progressBarStore;
    std::map<QString, std::unique_ptr<Client>> m_clientStore;
};

#endif // NODE_H
