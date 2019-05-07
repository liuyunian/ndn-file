#include "node.h"
#include "ui_node.h"
#include <src/client/ui/node.moc>

#include <QDomDocument>
#include <QPushButton>
#include <QFileDialog>

#include <iostream>
#include <stdlib.h>
#include "client.h"

Node::Node(const QString & name, const std::string & prefix, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Node),
    m_fileListModel(new QStandardItemModel()),
    m_name(name),
    m_prefix(prefix)
{
    ui->setupUi(this);

    this->setTitle("The Node Prefix is: " + QString::fromStdString(prefix));

    m_fileListModel->setColumnCount(5);
    m_fileListModel->setHorizontalHeaderLabels(QStringList() << "File Name" << "Last Modify Time" << "Size" << "State" << "Download");
    ui->fileListTable->setModel(m_fileListModel);
    ui->fileListTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->fileListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应

    auto client = std::make_unique<Client>(m_prefix);

    connect(client.get(), SIGNAL(displayFileListInfor(QString)),
            this, SLOT(on_displayFileListInfor(QString)),
            Qt::QueuedConnection);

    client->requestFileList();

    m_clientStore.insert({QString("fileList"), std::move(client)});
}

Node::~Node()
{
    delete ui;
    delete m_fileListModel;
}

void Node::on_displayFileListInfor(QString fileListInfor){
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
        QDomElement fileName = fileInforItem.namedItem("Name").toElement(); // 文件名
        m_fileListModel->setItem(i, 0, new QStandardItem(fileName.text()));

        QDomElement fileTime = fileInforItem.namedItem("Time").toElement(); // 文件最后修改时间
        m_fileListModel->setItem(i, 1, new QStandardItem(fileTime.text()));

        QDomElement fileSize = fileInforItem.namedItem("Size").toElement(); //文件大小
        bool ok = false;
        ulong size = fileSize.text().toULong(&ok);
        if(ok){
            QString size_display;
            if(size >= 1000000){
                size_display = QString::number(static_cast<double>(size)/1000000).append("MB");
            }
            else if(size >= 1000 && size < 1000000){
                size_display = QString::number(static_cast<double>(size)/1000).append("KB");
            }
            else{
                size_display = QString::number(size).append("B");
            }
            
            m_fileListModel->setItem(i, 2, new QStandardItem(size_display));
        }
        else{
            m_fileListModel->setItem(i, 2, new QStandardItem("NULL"));
        }

        QDomElement maxSeq = fileInforItem.namedItem("MaxSeq").toElement(); // 文件的分块的最大序号
        m_fileInforStore.insert(std::pair<QString, uint64_t>(fileName.text(), maxSeq.text().toULong()));

        auto progressBar = std::make_unique<QProgressBar>();
        progressBar->setRange(0, maxSeq.text().toInt());
        progressBar->setValue(0);
        ui->fileListTable->setIndexWidget(m_fileListModel->index(i, 3), progressBar.get());
        m_progressBarStore.insert({fileName.text(), std::move(progressBar)});

        auto download_btn = new QPushButton("download", this); //指定了parent之后可以不用自己释放内存,会造成内存泄露
        connect(download_btn, SIGNAL(clicked(bool)), this, SLOT(on_clickDownloadButton()));
        download_btn->setProperty("fileName", fileName.text());
        ui->fileListTable->setIndexWidget(m_fileListModel->index(i, 4), download_btn);
    }

    m_clientStore.erase("fileList");
}

void Node::on_clickDownloadButton(){
    QPushButton *btn = (QPushButton *)sender();   //产生事件的对象

    QString fileName = btn->property("fileName").toString();

    uint64_t maxSeq;
    auto iter = m_fileInforStore.find(fileName);
    if(iter != m_fileInforStore.end()){
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

    auto client = std::make_unique<Client>(m_prefix, fileName.toStdString(), filePath, maxSeq);

    connect(client.get(), SIGNAL(fileCompleted(QString)),
        this, SLOT(on_fileCompleted(QString)),
        Qt::QueuedConnection);

    connect(client.get(), SIGNAL(displayProgress(QString, int)),
        this, SLOT(on_displayProgress(QString, int)),
        Qt::QueuedConnection);

    client->requestFile();
    
    m_clientStore.insert({fileName, std::move(client)});
}

void Node::on_displayProgress(QString fileName, int value){
    auto iter = m_progressBarStore.find(fileName);
    if(iter != m_progressBarStore.end()){
        iter->second->setValue(value);
    }
}

void Node::on_fileCompleted(QString fileName){
    m_clientStore.erase(fileName);
}