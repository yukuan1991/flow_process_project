#pragma once
#include "item/symbol_item.h"

class checkout_item : public symbol_item
{
public:
    static unique_ptr<checkout_item> make(QPointF pos);
protected:
    checkout_item(QPointF pos, symbol_item *parent = Q_NULLPTR);
    bool init();
    //QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

};

