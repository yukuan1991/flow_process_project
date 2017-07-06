#include "temporary_storage_item.h"
#include <QPainter>

unique_ptr<temporary_storage_item> temporary_storage_item::make(QPointF pos)
{
    unique_ptr<temporary_storage_item> ret (new temporary_storage_item(pos));
    if(ret == nullptr or !ret->init())
    {
        return nullptr;
    }

    return ret;
}

temporary_storage_item::temporary_storage_item(QPointF pos, symbol_item *parent)
    : symbol_item (parent)
{
    setPos(pos);
}

bool temporary_storage_item::init()
{
    if(!symbol_item::init())
    {
        return false;
    }

    return true;
}

void temporary_storage_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(is_selected() ? selected_color() : unselected_color());
    painter->setBrush(Qt::white);
    const QPointF  temporary_storage_points[3] =
    {
        QPointF(0, 0), QPointF(item_width, 0), QPointF(item_width / 2, item_height)
    };
    painter->drawPolygon(temporary_storage_points, 3);
}
