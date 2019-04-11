#include "collect_information.h"
#include "ui_collect_information.h"
#include <src/client/ui/collect_information.moc>

#include "node_list.hpp"
#include "node.h"
#include <iostream>

CollectInformation::CollectInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollectInformation)
{
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize); //设置为固定的布局,鼠标不能进行拖拽
}

CollectInformation::~CollectInformation()
{
    delete ui;
}

void CollectInformation::on_AddNodeInfor_clicked(){
    QString nodeName = this->ui->NodeName->text();
    QString nodePrefix = this->ui->NodePrefix->text();
    if(nodeName.isEmpty() || nodePrefix.isEmpty()){
        this->ui->NodeInforDisplay->append("node name or prifix can't be empty");
        return;
    }

    std::shared_ptr<Node> node = std::make_shared<Node>(nodeName.toStdString(), nodePrefix.toStdString());
    m_nodeList.push_back(node);

    this->ui->NodeInforDisplay->append(nodeName + ": " + nodePrefix);
    this->ui->NodeName->clear();
    this->ui->NodePrefix->clear();
}

void CollectInformation::on_ButtonBox_accepted(){ //点击确定按钮
   if(m_nodeList.empty()){ //没有输入任何节点信息就点击了确认按钮
       this->ui->NodeInforDisplay->append("please add some node information!");
   }
   else{
        this->close(); //关闭收集节点信息的窗口

        NodeList * nlist = new NodeList(m_nodeList); //TODO: 这里的处理欠妥当
        nlist->show();
   }
}

void CollectInformation::on_ButtonBox_rejected(){ //点击取消按钮
    this->close();
}
