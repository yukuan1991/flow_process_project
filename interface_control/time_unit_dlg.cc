#include "time_unit_dlg.h"
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

time_unit_dlg::time_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    time_ = new QLabel("请设置时间单位:", this);
    unit_ = new QComboBox(this);
    confirm_ = new QPushButton("确定", this);
    cancel_ = new QPushButton("取消", this);
    QStringList text;
    text << "" << "秒" << "分钟" << "小时";
    unit_->addItems(text);

    hlayout1->addWidget(time_);
    hlayout1->addWidget(unit_);

//    hlayout2->addSpacerItem(new QSpacerItem(0, 0));
    hlayout2->addWidget(confirm_);
    hlayout2->addWidget(cancel_);

    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);

    setLayout(layout);
    setFixedSize(220, 80);
//    connect(unit_, &QComboBox::currentTextChanged, [](const QString & s) { qDebug() << s; });
    connect(unit_, &QComboBox::currentTextChanged, this, &time_unit_dlg::currenttextchanged);
    connect(confirm_, &QPushButton::clicked, this, &time_unit_dlg::confirm);
    connect(cancel_, &QPushButton::clicked, this, &time_unit_dlg::cancel);

}

QString time_unit_dlg::currentText() const
{
    return unit_->currentText();
}




