#include "time_unit_dlg.h"
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

time_unit_dlg::time_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QHBoxLayout* layout = new QHBoxLayout;
    time_ = new QLabel("请设置时间单位:", this);
    unit_ = new QComboBox(this);
    QStringList text;
    text << "" << "秒" << "分钟" << "小时";
    unit_->addItems(text);

    layout->addWidget(time_);
    layout->addWidget(unit_);
    setLayout(layout);

    connect(unit_, &QComboBox::currentTextChanged, [](const QString & s) { qDebug() << s; });
    connect(unit_, &QComboBox::currentTextChanged, this, &time_unit_dlg::currenttextchanged);

}




