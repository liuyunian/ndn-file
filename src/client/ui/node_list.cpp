#include "node_list.h"
#include "ui_node_list.h"
#include <src/client/ui/node_list.moc>

#include <QMessageBox>

NodeList::NodeList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NodeList),
    tabWidget(new QTabWidget(parent))
{
    ui->setupUi(this);

    this->setWindowTitle("File List");  
}

NodeList::~NodeList()
{
    delete ui;
}

void NodeList::on_AddNode_clicked(){
    QString nodeName = this->ui->NodeName->text();
    QString nodePrefix = this->ui->NodePrefix->text();
    if(nodeName.isEmpty() || nodePrefix.isEmpty()){
        QMessageBox::warning(NULL, "warning", "node name or prifix can't be empty");
        return;
    }

    auto node = std::make_unique<Node>(nodeName, nodePrefix.toStdString());
    m_nodeList.push_back(std::move(node));

    tabWidget->addTab(m_nodeList.back().get(), m_nodeList.back()->getNodeName());
    this->ui->verticalLayout->addWidget(tabWidget);

    this->ui->NodeName->clear();
    this->ui->NodePrefix->clear();
}


