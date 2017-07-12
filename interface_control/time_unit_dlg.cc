#include "time_unit_dlg.h"
#include <QLabel>

time_unit_dlg::time_unit_dlg(QWidget *parent, Qt::WindowFlags f)
    : unit_dlg (parent, f)
{
    label_->setText("请设置时间单位:");
    QStringList text;
    text << "" << "秒" << "分钟" << "小时";
    unit_->addItems(text);
}





