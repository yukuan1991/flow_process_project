#ifndef FLOW_PROCESS_H
#define FLOW_PROCESS_H

#include <QWidget>

namespace Ui {
class flow_process;
}

class flow_process : public QWidget
{
    Q_OBJECT

public:
    explicit flow_process(QWidget *parent = 0);
    ~flow_process();

private:
    Ui::flow_process *ui;
};

#endif // FLOW_PROCESS_H
