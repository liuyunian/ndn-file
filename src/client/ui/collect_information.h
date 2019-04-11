#ifndef COLLECT_INFORMATION_H
#define COLLECT_INFORMATION_H

#include <QDialog>
#include <QLayout>
#include <vector>
#include <memory>

#include "node.h"

namespace Ui {
class CollectInformation;
}

class CollectInformation : public QDialog
{
    Q_OBJECT

public:
    explicit CollectInformation(QWidget *parent = 0);
    ~CollectInformation();

private slots:
    void on_AddNodeInfor_clicked();

    void on_ButtonBox_accepted();

    void on_ButtonBox_rejected();

private:
    Ui::CollectInformation *ui;
    std::vector<std::shared_ptr<Node>> m_nodeList;
};

#endif // COLLECT_INFORMATION_H
