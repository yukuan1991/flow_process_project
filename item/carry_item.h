#pragma once
#include "item/symbol_item.h"


class carry_item : public symbol_item
{
public:
    static unique_ptr<carry_item> make(QPointF pos);
protected:
    carry_item(QPointF pos, symbol_item *parent = Q_NULLPTR);
    bool init();
//    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
};

