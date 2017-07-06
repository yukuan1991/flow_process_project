#include "checkout_item.h"
#include <QPainter>

unique_ptr<checkout_item> checkout_item::make(QPointF pos)
{
    unique_ptr<checkout_item> ret (new checkout_item(pos));
    if(ret == nullptr or !ret->init())
    {
        return nullptr;
    }
    return ret;
}

checkout_item::checkout_item(QPointF pos, symbol_item *parent)
    : symbol_item (parent)
{
    setPos(pos);
}

bool checkout_item::init()
{
    if(!symbol_item::init())
    {
        return false;
    }

    return true;
}

//QPainterPath checkout_item::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
//    return path;
//}

void checkout_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(is_selected() ? selected_color() : unselected_color());
    painter->setBrush(Qt::white);
    painter->drawRect(boundingRect());
}
