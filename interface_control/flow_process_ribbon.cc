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
        std::array<ui_group, 1> help;

        button_cell b;
        b.add ("帮助", QPixmap ("png/帮助.png"), help_);
        b.set_title("第一类");

        help[0] = ::move (b);

        add_tab(help, "帮助");
    }

    connect(cut_, &ribbon_tool::clicked, this, &flow_process_ribbon::cut);
    connect(copy_, &ribbon_tool::clicked, this, &flow_process_ribbon::copy);
    connect(paste_, &ribbon_tool::clicked, this, &flow_process_ribbon::paste);
    connect(del_, &ribbon_tool::clicked, this, &flow_process_ribbon::del);

    connect(time_unit_, &ribbon_tool::clicked, this, &flow_process_ribbon::time_unit);
    connect(distance_unit_, &ribbon_tool::clicked, this, &flow_process_ribbon::distance_unit);

    connect (this, &flow_process_ribbon::set_enabled, cut_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, copy_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, paste_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, del_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, time_unit_, &ribbon_tool::setEnabled);
    connect (this, &flow_process_ribbon::set_enabled, distance_unit_, &ribbon_tool::setEnabled);

    connect(help_, &ribbon_tool::clicked, this, &flow_process_ribbon::help);

}
