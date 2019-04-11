#include "node.h"
#include "ui_node.h"
#include <src/client/ui/node.moc>

#include <QDomDocument>
#include <QPushButton>
#include <QFileDialog>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include "client.h"

Node::Node(const std::string & name, const std::string & prefix, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Node),
    m_fileListModel(new QStandardItemModel()),
    m_name(name),
    m_prefix(prefix),
    m_client(nullptr)

{
    ui->setupUi(this);
    this->setTitle(QString::fromStdString(name + ":" + prefix));

    m_fileListModel->setColumnCount(4);
    m_fileListModel->setHorizontalHeaderLabels(QStringList() << "File Name" << "Time" << "Size" << "Download");
    ui->fileListTable->setModel(m_fileListModel);
    ui->fileListTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->fileListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应

    m_client = new Client(m_prefix);

    connect(m_client, SIGNAL(displayFileListInfor(QString)),
            this, SLOT(on_displayFileListInfor(QString)),
            Qt::QueuedConnection);

    std::thread rFLThread(&Client::requestFileList, m_client);
    rFLThread.detach();
}

Node::~Node()
{
    delete ui;
    if(m_client != nullptr){
        delete m_client;
    }
}

void Node::on_displayFileListInfor(QString fileListInfor){
    delete m_client;

    QDomDocument doc;
    if(!doc.setContent(fileListInfor))
    {
        std::cerr << "read xml string error" << std::endl;
        return;
    }

    QDomNode node = doc.firstChild();// .firstChild(); //第二级子节点
    QDomNodeList fileInforEntrys = node.childNodes();
    m_fileListModel->setRowCount(fileInforEntrys.count());
    for(int i = 0; i < fileInforEntrys.count(); ++ i){
        QDomNode fileInforItem = fileInforEntrys.at(i);
        QDomElement fileName = fileInforItem.namedItem("Name").toElement();
        m_fileListModel->setItem(i, 0, new QStandardItem(fileName.text()));

        QDomElement fileTime = fileInforItem.namedItem("Time").toElement();
        m_fileListModel->setItem(i, 1, new QStandardItem(fileTime.text()));

        QDomElement fileSize = fileInforItem.namedItem("Size").toElement();
        m_fileListModel->setItem(i, 2, new QStandardItem(fileSize.text()));

        QDomElement maxSeq = fileInforItem.namedItem("MaxSeq").toElement();
        m_fileList.insert(std::pair<std::string, uint32_t>(fileName.text().toStdString(), maxSeq.text().toUInt()));

        QPushButton * download_btn = new QPushButton("download");
        connect(download_btn, SIGNAL(clicked(bool)), this, SLOT(ClickDownloadButton()));
        download_btn->setProperty("fileName", fileName.text());  //为按钮设置fileName属性
        ui->fileListTable->setIndexWidget(m_fileListModel->index(i, 3), download_btn);
    }
}

void Node::ClickDownloadButton(){
    QPushButton *btn = (QPushButton *)sender();   //产生事件的对象

    std::string fileName = btn->property("fileName").toString().toStdString();

    uint32_t maxSeq;
    auto iter = m_fileList.find(fileName);
    if(iter != m_fileList.end()){
        maxSeq = iter->second;
    }

    QFileDialog * fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Choose Download Directory");
    fileDialog->setFileMode( QFileDialog::DirectoryOnly );
    std::string filePath;
    if ( fileDialog->exec() == QDialog::Accepted)
    {
        filePath = fileDialog->selectedFiles().front().toStdString();
    }
    else
    {
        return;
    }

    m_client = new Client(m_prefix, fileName, filePath, maxSeq);
    std::thread rFThread(&Client::requestFile, m_client);
    rFThread.detach();
}



