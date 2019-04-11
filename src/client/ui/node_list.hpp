#ifndef NODE_LIST_H_
#define NODE_LIST_H_

#include <QDialog>
#include <memory>

#include "node.h"

namespace Ui {
class NodeList;
}

class NodeList : public QDialog
{
    Q_OBJECT

public:
    explicit NodeList(std::vector<std::shared_ptr<Node>> & nodeList, QWidget *parent = 0);
    ~NodeList();

private:
    Ui::NodeList *ui;
    
    std::vector<std::shared_ptr<Node>> & m_nodeList;
};

#endif // NODE_LIST_H_
