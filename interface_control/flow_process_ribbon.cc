#include "flow_process_ribbon.h"
#include <QPixmap>
#include <base/lang/move.hpp>
#include <QDebug>

flow_process_ribbon::flow_process_ribbon(QWidget *parent)
    :ribbon (parent)
{
    {
        std::array<ui_group, 1> edit;

        button_cell b;
        b.add ("剪切", QPixmap ("png/剪切.png"), cut_);
        b.add ("复制", QPixmap ("png/复制.png"), copy_);
        b.add ("粘贴", QPixmap ("png/粘贴.png"), paste_);
        b.add ("删除", QPixmap ("png/删除.png"), del_);
        b.set_title("第一类");

        edit[0] = ::move (b);

        add_tab(edit, "编辑");
    }

    {
        std::array<ui_group, 1> setting;

        button_cell b;
        b.add ("时间单位", QPixmap ("png/时间单位.png"), time_unit_);
        b.add ("距离单位", QPixmap ("png/距离单位.png"), distance_unit_);
        b.set_title("设置单位");

        setting[0] = ::move (b);

        add_tab(setting, "设置");
    }

    {
        std::array<ui_group, 1> view;

        button_cell b;
        b.add ("隐藏", QPixmap ("png/隐藏.png"), hide_);
        b.set_title("符号隐藏");

        view[0] = ::move (b);

        add_tab(view, "视图");
    }

    {
        std::array<ui_group, 1> help;

        button_cell b;
        b.add ("帮助", QPixmap ("png/帮助.png"), help_);
        b.set_title("第一类");

        help[0] = ::move (b);

        add_tab(help, "帮助");
    }

    hide_->setCheckable(true);

    connect(cut_, &ribbon_tool::clicked, this, &flow_process_ribbon::cut);
    connect(copy_, &ribbon_tool::clicked, this, &flow_process_ribbon::copy);
    connect(paste_, &ribbon_tool::clicked, this, &flow_process_ribbon::paste);
    connect(del_, &ribbon_tool::clicked, this, &flow_process_ribbon::del);

    connect(time_unit_, &ribbon_tool::clicked, this, &flow_process_ribbon::time_unit_exec);
    connect(distance_unit_, &ribbon_tool::clicked, this, &flow_process_ribbon::distance_unit_exec);
    connect(hide_, &ribbon_tool::clicked, this, &flow_process_ribbon::hide_clicked);

    connect(this, &flow_process_ribbon::set_hide_checked, hide_, &ribbon_tool::setChecked);
//    connect(hide_, &ribbon_tool::toggled, this, &flow_process_ribbon::hide_clicked);

    connect (this, &flow_process_ribbon::set_enabled, cut_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, copy_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, paste_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, del_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, time_unit_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, distance_unit_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, hide_, &ribbon_tool::setEnabled);


    connect(help_, &ribbon_tool::clicked, this, &flow_process_ribbon::help);

}

void flow_process_ribbon::hide_clicked()
{
    if(hide_->isChecked())
    {
        emit hide_item(true);
    }
    else
    {
        emit hide_item(false);
    }
}
