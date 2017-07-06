#pragma once
#include "ribbon.h"

class flow_process_ribbon : public ribbon
{
    Q_OBJECT
signals:
    void file_menu_triggered (const QString &);
    void cut();
public:
    flow_process_ribbon(QWidget * parent = nullptr);
private:
    ribbon_tool *cut_, *copy_, *paste_, *del_;
    ribbon_tool *help_;
};

