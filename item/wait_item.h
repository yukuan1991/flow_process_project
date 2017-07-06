#pragma once
#include "item/symbol_item.h"

class wait_item : public symbol_item
{
public:
    static unique_ptr<wait_item> make(QPointF pos);
protected:
    wait_item(QPointF pos, symbol_item *parent = Q_NULLPTR);
    bool init();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
};
