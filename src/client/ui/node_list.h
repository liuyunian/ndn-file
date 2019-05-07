#ifndef NODE_LIST_H
#define NODE_LIST_H

#include <QMainWindow>
#include <QTabWidget>

#include "node.h"

namespace Ui {
class NodeList;
}

class NodeList : public QMainWindow
{
    Q_OBJECT

public:
    explicit NodeList(QWidget *parent = 0);
    ~NodeList();

private slots:
    void on_AddNode_clicked();

private:
    Ui::NodeList *ui;
    QTabWidget * tabWidget;

    std::vector<std::unique_ptr<Node>> m_nodeList;
};

#endif // NODE_LIST_H
