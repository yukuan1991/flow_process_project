#pragma once
#include "item/symbol_item.h"

class temporary_storage_item : public symbol_item
{
public:
    static unique_ptr<temporary_storage_item> make(QPointF pos);
protected:
    temporary_storage_item(QPointF pos, symbol_item *parent = Q_NULLPTR);
    bool init();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
};

