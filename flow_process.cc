#include "flow_process.h"
#include "ui_flow_process.h"

flow_process::flow_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::flow_process)
{
    ui->setupUi(this);
}

flow_process::~flow_process()
{
    delete ui;
}
