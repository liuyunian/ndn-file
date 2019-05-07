#include <iostream>
#include <QApplication>

#include "node_list.h"

int main(int argc, char* argv[]){
    QApplication app(argc, argv);

    NodeList nl;
    nl.show();

    return app.exec();
}