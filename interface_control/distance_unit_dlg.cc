#include "distance_unit_dlg.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDebug>

distance_unit_dlg::distance_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QHBoxLayout* layout = new QHBoxLayout;
    time_ = new QLabel("请设置距离单位:", this);
    unit_ = new QComboBox(this);
    QStringList text;
    text << "" << "米" << "百米" <<"千米" << "英尺" << "英寻" << "英里";
    unit_->addItems(text);

    layout->addWidget(time_);
    layout->addWidget(unit_);
    setLayout(layout);

    connect(unit_, &QComboBox::currentTextChanged, this, &distance_unit_dlg::currenttextchanged);
    connect(unit_, &QComboBox::currentTextChanged, [](const QString & s) { qDebug() << s; });
}

QString distance_unit_dlg::currentText() const
{
    return unit_->currentText();
}
