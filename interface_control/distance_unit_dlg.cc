#include "distance_unit_dlg.h"
#include <QLabel>

distance_unit_dlg::distance_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : unit_dlg (parent, f)
{
    label_->setText("请设置距离单位:");

    QStringList text;
    text << "" << "米" << "百米" <<"千米" << "英尺" << "英寻" << "英里";
    unit_->addItems(text);
}


