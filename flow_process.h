#ifndef FLOW_PROCESS_H
#define FLOW_PROCESS_H

#include <QWidget>

class canvas_view;
class QMdiSubWindow;
namespace Ui {
class flow_process;
}

class flow_process : public QWidget
{
    Q_OBJECT

public:
    explicit flow_process(QWidget *parent = 0);
    ~flow_process();
private:
    void init_conn();
    void set_button_enabled();
private:
    void file_operations(const QString& s);
    void file_new();
    void file_open();
    void file_save();
    void file_save_as();
    void save_subwindow(QMdiSubWindow* sub_window);
    void file_print();
private:
    void help_advice();
private:
    canvas_view* create_canvas_view();
    canvas_view* active_canvas_view();
private:
    Ui::flow_process *ui;
};

#endif // FLOW_PROCESS_H
