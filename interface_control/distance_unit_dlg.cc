#include "distance_unit_dlg.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

distance_unit_dlg::distance_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    time_ = new QLabel("请设置距离单位:", this);
    unit_ = new QComboBox(this);
    confirm_ = new QPushButton("确定", this);
    cancel_ = new QPushButton("取消", this);

    QStringList text;
    text << "" << "米" << "百米" <<"千米" << "英尺" << "英寻" << "英里";
    unit_->addItems(text);

    hlayout1->addWidget(time_);
    hlayout1->addWidget(unit_);
    hlayout2->addWidget(confirm_);
    hlayout2->addWidget(cancel_);

    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);

    setLayout(layout);
    setFixedSize(220, 80);

    connect(unit_, &QComboBox::currentTextChanged, this, &distance_unit_dlg::currenttextchanged);
//    connect(unit_, &QComboBox::currentTextChanged, [](const QString & s) { qDebug() << s; });
    connect(confirm_, &QPushButton::clicked, this, &distance_unit_dlg::confirm);
    connect(cancel_, &QPushButton::clicked, this, &distance_unit_dlg::cancel);
}

QString distance_unit_dlg::currentText() const
{
    return unit_->currentText();
}
