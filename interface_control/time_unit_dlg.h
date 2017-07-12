#pragma once
#include <QDialog>
#include "interface_control/unit_dlg.h"

class time_unit_dlg : public unit_dlg
{
    Q_OBJECT
public:
    time_unit_dlg(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
};

