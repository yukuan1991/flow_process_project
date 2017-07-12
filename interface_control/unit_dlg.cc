#include "unit_dlg.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


unit_dlg::unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    label_ = new QLabel(this);
    unit_ = new QComboBox(this);
    confirm_ = new QPushButton("确定", this);
    cancel_ = new QPushButton("取消", this);
//    QStringList text;
//    text << "" << "秒" << "分钟" << "小时";
//    unit_->addItems(text);

    hlayout1->addWidget(label_);
    hlayout1->addWidget(unit_);

    hlayout2->addWidget(confirm_);
    hlayout2->addWidget(cancel_);

    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);

    setLayout(layout);
    setFixedSize(220, 80);
//    connect(unit_, &QComboBox::currentTextChanged, this, &unit_dlg::currenttextchanged);
    connect(confirm_, &QPushButton::clicked, this, &unit_dlg::confirm);
    connect(cancel_, &QPushButton::clicked, this, &unit_dlg::cancel);

}
