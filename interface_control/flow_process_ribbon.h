#pragma once
#include "ribbon.h"

class flow_process_ribbon : public ribbon
{
    Q_OBJECT
signals:
    void cut();
    void copy();
    void paste();
    void del();
    void time_unit_exec();
    void distance_unit_exec();
    void hide_item(bool);
    void help();

    void set_hide_checked(bool);
public:
    flow_process_ribbon(QWidget * parent = nullptr);
    void hide_clicked();
private:
    ribbon_tool *cut_, *copy_, *paste_, *del_;
    ribbon_tool *hide_;
    ribbon_tool *help_;
    ribbon_tool *time_unit_, *distance_unit_;
};

