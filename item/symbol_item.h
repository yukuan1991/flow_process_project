#pragma once
#include <QGraphicsItem>
#include <memory>

using namespace std;


class symbol_item : public QGraphicsObject
{
    Q_OBJECT
signals:
    void select_changed (bool);

public:
    bool is_selected () const noexcept{ return is_selected_; }
    void set_selected (bool b);

protected:
    symbol_item(QGraphicsItem *parent = Q_NULLPTR);
    bool init();

    QRectF boundingRect() const override;

    static auto selected_color () { return Qt::red; }
    static auto unselected_color () { return Qt::black; }

    void mousePressEvent (QGraphicsSceneMouseEvent * event) override;
protected:
    static constexpr auto item_width = 20;
    static constexpr auto item_height = 20;


private:
    bool is_selected_ = false;
};
