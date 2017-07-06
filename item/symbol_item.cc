#include "item/symbol_item.h"
#include <QPainter>

symbol_item::symbol_item(QGraphicsItem *parent)
    : QGraphicsObject (parent)
{

}

bool symbol_item::init()
{
    connect (this, &symbol_item::select_changed, [this] (auto){ this->update (); });
    //connect (this, &symbol_item::select_changed, [] (auto){ qDebug () << "selection changed"; });
    return true;
}

QRectF symbol_item::boundingRect() const
{
    return QRectF{0, 0, item_width, item_height};
}

void symbol_item::set_selected(bool b)
{
    auto old_val = is_selected_;
    is_selected_ = b;
    if (old_val != is_selected_)
    {
        emit select_changed(b);
    }

//    if(is_selected_)
//    {
//        ++times_;
//    }
//    else
//    {
//        --times_;
//    }
}

void symbol_item::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
    set_selected(! is_selected ());
}
