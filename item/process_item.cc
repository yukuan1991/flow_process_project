#include "process_item.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>

unique_ptr<process_item> process_item::make(QPointF pos)
{
    unique_ptr<process_item> ret (new process_item(pos));
    if(ret == nullptr or !ret->init())
    {
        return nullptr;
    }

    return ret;
}

process_item::process_item(QPointF pos, symbol_item* parent)
    : symbol_item (parent)
{
    setPos(pos);
}

bool process_item::init()
{
    if (!symbol_item::init())
    {
        return false;
    }

    return true;
}

//QPainterPath process_item::shape() const
//{
//    QPainterPath path;
//    //const auto rect = QRectF(0, 0, item_width, item_height);
//    path.addEllipse(boundingRect());
//    return path;
//}

void process_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(is_selected() ? selected_color() : unselected_color());
    //scene ()->backgroundBrush().color();
    painter->setBrush(Qt::white);

    painter->drawEllipse(boundingRect());
}
