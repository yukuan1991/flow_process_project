#pragma once
#include <QGraphicsView>
#include <memory>
#include "canvas_scene.h"
#include <QPrinter>

using std::unique_ptr;
using std::make_unique;
class canvas_view : public QGraphicsView
{
    Q_OBJECT
public:
    template<typename ... ARGS>
    static unique_ptr<canvas_view> make (ARGS && ...args)
    {
        unique_ptr<canvas_view> ret (new canvas_view (std::forward<ARGS> (args)...));
        if(ret == nullptr or !ret->init())
        {
            return nullptr;
        }

        return ret;
    }
   virtual ~canvas_view();
signals:
    void saved();
    void view_closed();
    void time_unit_changed(const QString &s);
    void distance_unit_changed(const QString &s);
    void load_time_unit(const QString &s);
    void load_distance_unit(const QString &s);
public:
    bool is_unsaved() { return unsaved_content_; }
    bool load (const std::string& data);
    std::string dump();
    QString attached_file() { return windowTitle(); }
    void set_attached_file(QString attached_file) { setWindowTitle(attached_file); }
public:
    void print_render(QPrinter* printer);
protected:
    template<typename ... ARGS>
    canvas_view(ARGS && ... args) : QGraphicsView(std::forward<ARGS> (args)...) {}
    bool init();
//protected:
//    void closeEvent(QCloseEvent* event) override;
private:
    unique_ptr<canvas_scene> scene_ = canvas_scene::make (QRectF{0, 0, 1100, 1500});

    bool unsaved_content_ = false;

};
