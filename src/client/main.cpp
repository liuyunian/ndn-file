#include <iostream>
#include <QApplication>

#include "collect_information.h"

int main(int argc, char* argv[]){
    QApplication app(argc, argv);

    CollectInformation collect;
    collect.show(); //显示节点信息输入窗口
    
    return app.exec();
}