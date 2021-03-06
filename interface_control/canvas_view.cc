﻿#include "canvas_view.h"
#include "json.hpp"
#include <QLineEdit>
#include "item/symbol_item.h"
#include <QCloseEvent>
#include <QDebug>

using nlohmann::json;

bool canvas_view::init()
{
    auto ptr_scene = scene_.get();
    this->setScene(ptr_scene);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHints (QPainter::SmoothPixmapTransform);

//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect (ptr_scene, &canvas_scene::scene_changed, [this]{ unsaved_content_ = true; });
    //connect (scene_.get(), &canvas_scene::scene_changed, [this]{ qDebug() << "unsaved_content_"; });
    connect (this, &canvas_view::saved, [this]{ unsaved_content_ = false; });
    //connect (this, &canvas_view::saved, [this]{ qDebug() << "saved_content_"; });

    connect(this, &canvas_view::time_unit_exec, ptr_scene, &canvas_scene::time_unit_exec);
    connect(this, &canvas_view::distance_unit_exec, ptr_scene, &canvas_scene::distance_unit_exec);

    connect(this, &canvas_view::hide_item, ptr_scene, &canvas_scene::hide);
    connect(ptr_scene, &canvas_scene::load_hide_state, this, &canvas_view::load_hide_state);

    connect(this, &canvas_view::hide_current_state, ptr_scene, &canvas_scene::hide_current_state);
    return true;
}

//void canvas_view::closeEvent(QCloseEvent *event)
//{
//    const bool unsaved = unsaved_content_;

//    if(unsaved)
//    {
//        emit view_closed();
//        event->ignore();
//    }
//    else
//    {
//        QGraphicsView::closeEvent(event);
//    }
//}

canvas_view::~canvas_view()
{
    qDebug() << "~canvas_view()";
}

void canvas_view::copy()
{
    scene_->copy();
}

void canvas_view::cut()
{
    scene_->cut();
}

void canvas_view::paste()
{
    scene_->paste();
}

void canvas_view::del()
{
    scene_->del();
}

bool canvas_view::load(const string &data)
{
    return scene_->load(data);
}

bool canvas_view::import(const std::vector<taskInfo> &data)
{
    return scene_->import(data);
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
