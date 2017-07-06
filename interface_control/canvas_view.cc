#include "canvas_view.h"
#include "json.hpp"
#include <QLineEdit>
#include "item/symbol_item.h"
#include <QCloseEvent>

using nlohmann::json;

bool canvas_view::init()
{
    this->setScene(scene_.get());
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    connect (scene_.get(), &canvas_scene::scene_changed, [this]{ unsaved_content_ = true; });
    //connect (scene_.get(), &canvas_scene::scene_changed, [this]{ qDebug() << "unsaved_content_"; });
    connect (this, &canvas_view::saved, [this]{ unsaved_content_ = false; });
    //connect (this, &canvas_view::saved, [this]{ qDebug() << "saved_content_"; });

    return true;
}

void canvas_view::closeEvent(QCloseEvent *event)
{
    const bool unsaved = unsaved_content_;

    if(unsaved)
    {
        emit view_closed();
        event->ignore();
    }
    else
    {
        QGraphicsView::closeEvent(event);
    }
}

canvas_view::~canvas_view()
{

}

bool canvas_view::load(const string &data)
{
    return scene_->load(data);
}



std::string canvas_view::dump()
{
    return scene_->dump ();
}

void canvas_view::print_render(QPrinter *printer)
{
    QPainter painter(printer);
    scene()->render(&painter, QRectF(), scene_.get()->print_rect());
}
