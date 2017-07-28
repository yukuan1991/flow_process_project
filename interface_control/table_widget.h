#ifndef TABLE_WIDGET_H
#define TABLE_WIDGET_H
#include <QTableWidget>


class table_widget : public QTableWidget
{
public:    
    template<typename ... ARGS>
    table_widget(ARGS && ...args) : QTableWidget(std::forward<ARGS> (args)...)
    {

    }


    void on_copy_del (unsigned flag = 0);
    void on_paste ();

protected:
    void keyPressEvent (QKeyEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;
public:
    static constexpr uint32_t OPERATION_DEL = 0b1;
    static constexpr uint32_t OPERATION_COPY = 0b10;
private:
    std::vector<std::vector<std::string>> get_clip_structure ();
    std::tuple<int, int, int, int> get_rect (bool* ok = nullptr);
private:
    QTableWidgetItem current_click_item_;
};

#endif // TABLE_WIDGET_H
