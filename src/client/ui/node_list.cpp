#include "node_list.hpp"
#include "ui_node_list.h"
#include <src/client/ui/node_list.moc>

#include "node.h"
#include "ui_node.h"

NodeList::NodeList(std::vector<std::shared_ptr<Node>> & nodeList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NodeList),
    m_nodeList(nodeList)
{
    ui->setupUi(this);
    for(auto node : m_nodeList){
        this -> ui -> verticalLayout -> addWidget(node.get());
    }
}

NodeList::~NodeList()
{
    delete ui;
}
