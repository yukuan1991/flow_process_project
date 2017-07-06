#include "carry_item.h"
#include <QPainter>

unique_ptr<carry_item> carry_item::make(QPointF pos)
{
    unique_ptr<carry_item> ret (new carry_item(pos));
    if(ret == nullptr or !ret->init())
    {
        return nullptr;
    }

    return ret;
}

carry_item::carry_item(QPointF pos, symbol_item *parent)
    : symbol_item (parent)
{
    setPos(pos);
}


bool carry_item::init()
{
    if(!symbol_item::init())
    {
        return false;
    }
    return true;
}

//QPainterPath carry_item::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
//    return path;
//}

void carry_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(is_selected() ? selected_color() : unselected_color());
    const auto carry_line_right = QPointF(item_width, item_height / 2);
    painter->drawLine(QPointF(0, item_height / 2), carry_line_right);
    painter->drawLine(QPointF(carry_line_right.x() - 5, carry_line_right.y() - 3),
                      carry_line_right);
    painter->drawLine(QPointF(carry_line_right.x() - 5, carry_line_right.y() + 3),
                      carry_line_right);
}
