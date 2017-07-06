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
        std::array<ui_group, 1> help;

        button_cell b;
        b.add ("帮助", QPixmap ("png/帮助.png"), help_);
        b.set_title("第一类");

        help[0] = ::move (b);

        add_tab(help, "帮助");
    }

    connect(cut_, &ribbon_tool::clicked, this, &flow_process_ribbon::cut);
}
