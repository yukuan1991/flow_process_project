#include "flow_process.h"
#include "ui_flow_process.h"
#include <QDebug>

flow_process::flow_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::flow_process)
{
    ui->setupUi(this);
    init_conn();
}

flow_process::~flow_process()
{
    delete ui;
}

void flow_process::init_conn()
{
//    connect(ui->flowprocess_ribbon, &ribbon::file_menu_triggered, [this] (const QString & s) { qDebug() << s; });
    connect(ui->flowprocess_ribbon, &ribbon::file_menu_triggered, [] (const QString & s) { qDebug() << s; });
    connect(ui->flowprocess_ribbon, &flow_process_ribbon::cut, []  { qDebug() << "cut"; });

}
