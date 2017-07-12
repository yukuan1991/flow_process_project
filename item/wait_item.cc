#include "wait_item.h"
#include <QPainter>

unique_ptr<wait_item> wait_item::make(QPointF pos)
{
    unique_ptr<wait_item> ret (new wait_item(pos));
    if(ret == nullptr or !ret->init())
    {
        return nullptr;
    }

    return ret;
}

wait_item::wait_item(QPointF pos, symbol_item *parent)
    : symbol_item (parent)
{
    setPos(pos);
}

bool wait_item::init()
{
    if(!symbol_item::init())
    {
        return false;
    }

    return true;
}

void wait_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
//    QFont font("宋体", 17);
//    QFont font("Dotum", 17);
//    painter->setFont(font);
    painter->setBrush(Qt::white);
    painter->setPen(is_selected() ? selected_color() : unselected_color());
    const auto rect = QRectF(boundingRect().topLeft() - QPointF(5, 0), boundingRect().bottomRight() - QPointF(5, 0));
    painter->drawChord(rect, 90 * 16, -180 * 16);
//    painter->drawRect(boundingRect());

//    painter->drawText(boundingRect(), Qt::AlignCenter, "D");
}
