#include "canvas_scene.h"
#include <QPainter>
#include <string>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <memory>
#include "item/symbol_item.h"
#include "item/process_item.h"
#include "item/checkout_item.h"
#include "item/carry_item.h"
#include "item/wait_item.h"
#include "item/temporary_storage_item.h"
#include <QLabel>
#include "json.hpp"
#include <boost/range/algorithm.hpp>
#include "interface_control/table_widget.h"
#include <QDebug>
#include <QHeaderView>
#include <QString>

const QPointF canvas_scene::top_left = QPointF (5, 5);

using std::begin;
using std::string;
using std::end;
using std::find_if;
using nlohmann::json;

bool canvas_scene::init()
{
    icons_.resize(second_table_row_size_);
    for (auto & it : icons_)
    {
        it.resize(second_table_behind_col);
    }

    connect (this, &canvas_scene::item_selection_changed, this, &canvas_scene::unselect_other);

    setBackgroundBrush(QColor (230, 230, 230));

//    setSceneRect(0, 0, 1500, 1000);

    const auto offset = 20;
    print_rect_ = QRectF(0, 0, width_ + 2 * top_left.x(), line_height_ * 31 + offset + 2 * top_left.y());

    add_symbol_item();
    set_table();

    dlg_time_unit_ = make_unique<time_unit_dlg> ();
    dlg_distance_unit_ = make_unique<distance_unit_dlg> ();

    //connect (this, &canvas_scene::scene_changed, [this] () { qDebug() << "scene changed"; });
    connect(dlg_time_unit_.get(), &time_unit_dlg::confirm, this, &canvas_scene::time_unit_confirm);
    connect(dlg_time_unit_.get(), &time_unit_dlg::cancel, this, &canvas_scene::time_unit_cancel);
    connect(dlg_distance_unit_.get(), &distance_unit_dlg::confirm, this, &canvas_scene::distance_unit_confirm);
    connect(dlg_distance_unit_.get(), &distance_unit_dlg::cancel, this, &canvas_scene::distance_unit_cancel);

    return true;
}

void canvas_scene::time_unit_exec()
{
    dlg_time_unit_->exec();
}

void canvas_scene::distance_unit_exec()
{
    dlg_distance_unit_->exec();
}

void canvas_scene::time_unit_confirm()
{
    str_time_unit_ = dlg_time_unit_->currentText();
    dlg_time_unit_->hide();
}

void canvas_scene::time_unit_cancel()
{
    dlg_time_unit_->hide();
    dlg_time_unit_->set_unit(str_time_unit_);
}

void canvas_scene::distance_unit_confirm()
{
    str_distance_unit_ = dlg_distance_unit_->currentText();
    dlg_distance_unit_->hide();
}

void canvas_scene::distance_unit_cancel()
{
    dlg_distance_unit_->hide();
    dlg_distance_unit_->set_unit(str_distance_unit_);
}

string canvas_scene::dump()
{
    using boost::range::find_if;
//    using std::end;
//    using std::begin;

    json data;
    data ["单位"]["时间"] = str_time_unit_.toStdString ();
    data ["单位"]["距离"] = str_distance_unit_.toStdString ();

    data ["隐藏"] = hide_state_;

    data ["数据"] = json::array ();
    for (int row = 0; row < second_table_row_size_; row ++)
    {
        const auto u_row = static_cast<size_t> (row);
        auto work_title = tablewidget_->item(row, 0)->text().toStdString();
        auto distance = tablewidget_->item(row, 1)->text().toStdString();
        auto time = tablewidget_->item(row, 2)->text().toStdString();

        const auto & row_icon = icons_.at(u_row);
        auto red_icon = find_if (row_icon, [] (auto&& item) { return item->is_selected (); });
        int pos = -1;
        if (red_icon != end (row_icon))
        {
            pos = red_icon - begin (row_icon);
        }

        data ["数据"].push_back ({
                                   {"工作说明", std::move (work_title)},
                                   {"距离", std::move (distance)},
                                   {"时间", std::move (time)},
                                   {"图标", pos}
                               });
    }


    return data.dump(4);
}

bool canvas_scene::load(const string &data) try
{
    auto content = json::parse(data);

    std::string time_unit = content["单位"]["时间"];
    std::string distance_unit = content["单位"]["距离"];

    dlg_distance_unit_->set_unit(distance_unit.data());
    str_distance_unit_ = distance_unit.data();
    dlg_time_unit_->set_unit(time_unit.data());
    str_time_unit_ = time_unit.data();

    auto & table = content["数据"];

    int row = 0;
    for (auto & it : table)
    {
        const string work_title = it["工作说明"];
        const string time = it["时间"];
        const string distance = it ["距离"];

        tablewidget_->item(row, 0)->setText(work_title.data());
        tablewidget_->item(row, 1)->setText(distance.data());
        tablewidget_->item(row, 2)->setText(time.data());

        int pos = it["图标"];

        if (pos >= 0)
        {
            icons_.at(static_cast<size_t>(row)).at(static_cast<size_t> (pos))->set_selected(true);
        }
        row ++;
    }

    hide_state_ = content["隐藏"];
    hide(hide_state_);

    return true;
}
catch(const std::exception & e)
{
    Q_UNUSED(e);
    return false;
}

void canvas_scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    painter->setBrush(Qt::white);

    draw_table(painter);
    draw_table_text(painter);
    draw_symbol(painter);
    draw_connected_lines(painter);
    draw_counts(painter);
    draw_times(painter);
    draw_distance(painter);

}

bool canvas_scene::import(const std::vector<taskInfo>& data)
{
    int row = 0;
    for (auto & it : data)
    {
        const auto work_title_qstr = it.name;
        const string work_title = work_title_qstr.toStdString();
        const auto time_qstr = QString::number(it.stdTime);
        const string time = time_qstr.toStdString();

        tablewidget_->item(row, 0)->setText(work_title.data());
        tablewidget_->item(row, 2)->setText(time.data());

        int pos;
        if(it.type == workType::Processing)
        {
            pos = 0;
        }
        else if(it.type == workType::Checking)
        {
            pos = 1;
        }
        else if(it.type == workType::Moving)
        {
            pos = 2;
        }
        else if(it.type == workType::Waiting)
        {
            pos = 3;
        }
        else
        {
            pos = -1;
        }

        if (pos >= 0)
        {
            icons_.at(static_cast<size_t>(row)).at(static_cast<size_t> (pos))->set_selected(true);
        }
        row ++;
    }

    return true;
}



/*每行高30个像素, 共31行*/
void canvas_scene::draw_table(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    painter->drawRect(first_table_rect);

    /*表一、表二的上下间隔为20*/
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() + width_ / 2,
                                         first_table_rect.bottomLeft().y() + 20);
    const auto second_table_rect = QRectF(second_table_topleft.x(),
                                          second_table_topleft.y(),
                                          width_ / 2, second_table_height_);

    painter->drawRect(second_table_rect);

    /*表一的横线*/
    for(int i = 1; i < first_table_row_size_; i++)
    {
        painter->drawLine(QPointF(first_table_rect.x(),
                                  first_table_rect.y() + i * line_height_),
                          QPointF(first_table_rect.x() + width_,
                                  first_table_rect.y() + i * line_height_));
    }


    /*表二的横线*/
    painter->drawLine(QPointF(second_table_rect.x(),
                              second_table_rect.y() + 2 * line_height_),
                      QPointF(second_table_rect.x() + width_ / 2,
                              second_table_rect.y() + 2 * line_height_));
    painter->drawLine(QPointF(second_table_rect.x() ,
                              second_table_rect.y() + line_height_),
                      QPointF(second_table_rect.x() + width_ / 2,
                              second_table_rect.y() + line_height_));

//    for(int i = 1; i < second_table_row_size_; i++)
//    {
//        painter->drawLine(QPointF(second_table_rect.x(),
//                                  second_table_rect.y() + (2 + i) * line_height_),
//                          QPointF(second_table_rect.x() + width_ / 2,
//                                  second_table_rect.y() + (2 + i) * line_height_ ));
//    }

//    /*表二 一分为二的竖线*/
//    painter->drawLine(QPointF(second_table_rect.x() + width_ / 2,
//                              second_table_rect.y()),
//                      QPointF(second_table_rect.bottomLeft().x() + width_ / 2,
//                              second_table_rect.bottomLeft().y()));




    /*表二 序号栏行宽55, 工作说明栏行宽150， 距离栏及时间栏行宽85*/
//    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_,
//                              second_table_rect.y()),
//                      QPointF(second_table_rect.x() + serial_number_width_,
//                              second_table_rect.y() + second_table_height_));
//    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_,
//                              second_table_rect.y()),
//                      QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_,
//                              second_table_rect.y() + second_table_height_));
//    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_ + distance_width_,
//                              second_table_rect.y()),
//                      QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_ + distance_width_,
//                              second_table_rect.y() + second_table_height_));
//    painter->drawLine(QPointF(second_table_rect.x() + width_ / 2, second_table_rect.y()),
//                      QPointF(second_table_rect.x() + width_ / 2,
//                              second_table_rect.y() + second_table_height_));


    /*表一 加工、检验、搬运、等待、暂存栏行宽75*/
    for(int i = 0; i < first_table_behind_col_; i++)
    {
        painter->drawLine(QPointF(first_table_rect.x() + i * (width_ / 10) + width_ / 2,
                                  first_table_rect.y()),
                          QPointF(first_table_rect.x() + i * (width_ / 10) + width_ / 2,
                                  first_table_rect.y() + first_table_height_));
    }
}

void canvas_scene::draw_table_text(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);


    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() + width_ / 2,
                                         first_table_rect.bottomLeft().y() + 20);


    QFont font("宋体", 12);
    painter->setFont(font);

    painter->drawText(QRectF(second_table_topleft.x(),
                             second_table_topleft.y(), width_ / 2, line_height_),
                             Qt::AlignCenter, tr("符号"));

    /*表一 前半张表各行的内容*/
    {
        painter->save();
        QFont font("宋体", 14);
        painter->setFont(font);

        painter->drawText(QRectF(first_table_rect.x(),
                                 first_table_rect.y(), width_ / 2,
                                 line_height_), Qt::AlignCenter, tr("作业内容"));
        painter->drawText(QRectF(first_table_rect.x(),
                                 first_table_rect.y() + line_height_, width_ / 2,
                                 line_height_), Qt::AlignCenter, tr("次数"));
        painter->drawText(QRectF(first_table_rect.x(),
                                 first_table_rect.y() + 2 * line_height_, width_ / 2,
                                 line_height_), Qt::AlignCenter, "距离单位:" + dlg_distance_unit_->currentText());

        painter->drawText(QRectF(first_table_rect.x(),
                                 first_table_rect.y() + 3 * line_height_, width_ / 2,
                                 line_height_), Qt::AlignCenter, "时间单位:" + dlg_time_unit_->currentText());

        QPointF symbol_table_topleft(first_table_rect.x() + width_ / 2,
                                     first_table_rect.y() );
        painter->drawText(QRectF(symbol_table_topleft.x(), symbol_table_topleft.y(),
                                 width_ / 10, line_height_), Qt::AlignVCenter, tr(" 加工"));
        painter->drawText(QRectF(symbol_table_topleft.x() + width_ / 10,
                                 symbol_table_topleft.y(),
                                 width_ / 10, line_height_), Qt::AlignVCenter, tr(" 检验"));
        painter->drawText(QRectF(symbol_table_topleft.x() + 2 * (width_ / 10),
                                 symbol_table_topleft.y(),
                                 width_ / 10, line_height_), Qt::AlignVCenter, tr(" 搬运"));
        painter->drawText(QRectF(symbol_table_topleft.x() + 3 * (width_ / 10),
                                 symbol_table_topleft.y(),
                                 width_ / 10, line_height_), Qt::AlignVCenter, tr(" 等待"));
        painter->drawText(QRectF(symbol_table_topleft.x() + 4 * (width_ / 10),
                                 symbol_table_topleft.y(),
                                 width_ / 10, line_height_), Qt::AlignVCenter, tr(" 暂存"));
        painter->restore();
    }
}

void canvas_scene::draw_symbol(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);

    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() + width_ / 2,
                                         first_table_rect.bottomLeft().y() + 20);


    /*表二符号部分*/
    {
        const auto width = 20;
        const auto height = 20;
        const auto offset_tab_x = 25;
        const auto offset_tab_y = 5;
        const QPointF symbol_table_topleft(second_table_topleft.x(),
                                                  second_table_topleft.y() + line_height_);

        painter->drawEllipse(QRectF(symbol_table_topleft.x() + offset_tab_x,
                                    symbol_table_topleft.y() + offset_tab_y, width, height));

        painter->drawRect(QRectF(symbol_table_topleft.x() + (width_ / 10) + offset_tab_x,
                                 symbol_table_topleft.y() + offset_tab_y, width, height));

        const auto carry_line_right = QPointF(symbol_table_topleft.x() + offset_tab_x +
                                              2 * (width_ / 10) + 20,
                                              symbol_table_topleft.y() + 15);
        painter->drawLine(QPointF(symbol_table_topleft.x() + 2 * (width_ / 10) + offset_tab_x,
                                  symbol_table_topleft.y() + 15), carry_line_right);
        painter->drawLine(QPointF(carry_line_right.x() - 5, carry_line_right.y() - 3),
                          carry_line_right);
        painter->drawLine(QPointF(carry_line_right.x() - 5, carry_line_right.y() + 3),
                          carry_line_right);

        painter->save();
        QRectF wait_rect(symbol_table_topleft.x() + 3 * (width_ / 10) + offset_tab_x - 5,
                         symbol_table_topleft.y() + offset_tab_y,
                         width, height);
        painter->setBrush(Qt::white);
        painter->drawChord(wait_rect, 90 * 16, -180 * 16);
//        QFont font("Dotum", 17);
//        painter->setFont(font);
//        QRectF wait_rect(symbol_table_topleft.x() + 3 * (width_ / 10) + offset_tab_x,
//                         symbol_table_topleft.y() + offset_tab_y,
//                         width, height);
//        painter->drawText(wait_rect, Qt::AlignCenter, tr("D"));
        painter->restore();

        const QPointF  temporary_storage_points[3] =
        {
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x,
            symbol_table_topleft.y() + offset_tab_y),
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x + width,
            symbol_table_topleft.y() + offset_tab_y),
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x + width / 2,
            symbol_table_topleft.y() + offset_tab_y + height),
        };
        painter->drawPolygon(temporary_storage_points, 3);
    }

    /*表一符号部分*/
    {
        const auto width = 15;
        const auto height = 15;

        const auto offset_tab_x = 50;
        const auto offset_tab_y = 8;
        const QPointF symbol_table_topleft(first_table_rect.x() + width_ / 2,
                                                 first_table_rect.y());

        painter->drawEllipse(QRectF(symbol_table_topleft.x() + offset_tab_x,
                                    symbol_table_topleft.y() + offset_tab_y, width, height));

        painter->drawRect(QRectF(symbol_table_topleft.x() + (width_ / 10) + offset_tab_x,
                                 symbol_table_topleft.y() + offset_tab_y, width, height));

        const auto carry_line_right = QPointF(symbol_table_topleft.x() + offset_tab_x +
                                                    2 * (width_ / 10) + width,
                                                    symbol_table_topleft.y() + 15);
        painter->drawLine(QPointF(symbol_table_topleft.x() + 2 * (width_ / 10) + offset_tab_x,
                                  symbol_table_topleft.y() + 15),
                          carry_line_right);
        painter->drawLine(QPointF(carry_line_right.x() - 5,
                                  carry_line_right.y() - 3),
                          carry_line_right);
        painter->drawLine(QPointF(carry_line_right.x() - 5, carry_line_right.y() + 3),
                          carry_line_right);

        painter->save();
//        QFont font("Dotum", 17);
//        painter->setFont(font);
//        QRectF wait_rect(symbol_table_topleft.x() + 3.5 * (width_ / 10),
//                               symbol_table_topleft.y(),
//                               (width_ / 10 / 2), line_height_);
//        painter->drawText(wait_rect, Qt::AlignCenter, tr("D"));

        QRectF wait_rect(symbol_table_topleft.x() + (width_ / 10) * 3 + offset_tab_x - 5,
                               symbol_table_topleft.y() + offset_tab_y, width, height);
        painter->setBrush(Qt::white);
        painter->drawChord(wait_rect, 90 * 16, -180 * 16);
        painter->restore();

        const QPointF  temporary_storage_points[3] =
        {
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x,
            symbol_table_topleft.y() + offset_tab_y),
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x + width,
            symbol_table_topleft.y() + offset_tab_y),
            QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_tab_x + width / 2,
            symbol_table_topleft.y() + offset_tab_y + height),
        };
        painter->drawPolygon(temporary_storage_points, 3);
    }
}



void canvas_scene::set_table()
{
    tablewidget_ = new table_widget(second_table_row_size_, second_table_front_col);
    QStringList header;
    header << "工作说明" << "距离" << "时间";
    tablewidget_->setHorizontalHeaderLabels(header);

    const auto offset = 20;
    tablewidget_->move(static_cast<int>(top_left.x()), static_cast<int>(first_table_height_ + offset + top_left.y()));
    tablewidget_->resize(static_cast<int>(width_ / 2 - top_left.x()), second_table_height_);

    tablewidget_->horizontalHeader()->setFixedHeight(line_height_ * 2);
    tablewidget_->verticalHeader()->setFixedWidth(serial_number_width_);
    tablewidget_->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tablewidget_->verticalHeader()->setDefaultSectionSize(line_height_);
    tablewidget_->setColumnWidth(0, static_cast<int>(job_content_width_ - top_left.x()));
    tablewidget_->setColumnWidth(1, distance_width_);
    tablewidget_->setColumnWidth(2, time_width_);
    tablewidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tablewidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    addWidget(tablewidget_);

    for (int i = 0; i < second_table_row_size_; i ++)
    {
        tablewidget_->setItem(i, 0, new QTableWidgetItem);
        tablewidget_->setItem(i, 1, new QTableWidgetItem);
        tablewidget_->setItem(i, 2, new QTableWidgetItem);
        tablewidget_->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        tablewidget_->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        tablewidget_->item(i, 2)->setTextAlignment(Qt::AlignCenter);
    }

//    tablewidget_->hide();

}

void canvas_scene::add_symbol_item()
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);

    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() + width_ / 2,
                                         first_table_rect.bottomLeft().y() + 20);


    QPointF symbol_table_topleft(second_table_topleft.x(),
                                       second_table_topleft.y() + 2 * line_height_);


    const auto offset_x = 25;
    const auto offset_y = 5;
    for(int row = 0; row < second_table_row_size_; row++)
    {
        size_t col = 0;
        auto process_item = process_item::make
                (QPointF(symbol_table_topleft.x() + offset_x,
                            symbol_table_topleft.y() + offset_y + line_height_ * row));
        icons_.at(static_cast<size_t> (row)).at(static_cast<size_t>(col)) = process_item.get();

        connect (process_item.get (), &symbol_item::select_changed, [this, row, col] (bool b)
        {
            this->item_selection_changed(row, static_cast<int>(col), b);
        });
        connect (process_item.get(), &symbol_item::select_changed, [this, col] (bool b)
        {
            b ? ++times_.at(col) : --times_.at(col);
        });
        connect (process_item.get(), &symbol_item::select_changed, [this] ()
        {
            emit this->scene_changed();
        });

        addItem(process_item.release());
        col++;

        auto checkout_item = checkout_item::make
                (QPointF(symbol_table_topleft.x() + (width_ / 10) + offset_x,
                         symbol_table_topleft.y() + offset_y + line_height_ * row));
        icons_.at(static_cast<size_t> (row)).at(static_cast<size_t> (col)) = checkout_item.get();
        connect (checkout_item.get (), &symbol_item::select_changed, [this, row, col] (bool b)
        {
            this->item_selection_changed(row, static_cast<int>(col), b);
        });
        connect (checkout_item.get(), &symbol_item::select_changed, [this, col] (bool b)
        {
            b ? ++times_.at(col) : --times_.at(col);
        });
        connect (checkout_item.get(), &symbol_item::select_changed, [this] ()
        {
            emit this->scene_changed();
        });
        addItem(checkout_item.release());
        col++;

        auto carry_item = carry_item::make
                (QPointF(symbol_table_topleft.x() + 2 *(width_ / 10) + offset_x,
                         symbol_table_topleft.y() + offset_y + line_height_ * row));
        icons_.at(static_cast<size_t> (row)).at(static_cast<size_t> (col)) = carry_item.get();
        connect (carry_item.get (), &symbol_item::select_changed, [this, row, col] (bool b)
        {
            this->item_selection_changed(row, static_cast<int>(col), b);
        });
        connect (carry_item.get(), &symbol_item::select_changed, [this, col] (bool b)
        {
            b ? ++times_.at(col) : --times_.at(col);
        });
        connect (carry_item.get(), &symbol_item::select_changed, [this] ()
        {
            emit this->scene_changed();
        });
        addItem(carry_item.release());
        col++;

        auto wait_item = wait_item::make
                (QPointF(symbol_table_topleft.x() + 3 * (width_ / 10) + offset_x,
                         symbol_table_topleft.y() + offset_y + line_height_ * row));
        icons_.at(static_cast<size_t> (row)).at(static_cast<size_t> (col)) = wait_item.get();
        connect (wait_item.get (), &symbol_item::select_changed, [this, row, col] (bool b)
        {
            this->item_selection_changed(row, static_cast<int>(col), b);
        });
        connect (wait_item.get(), &symbol_item::select_changed, [this, col] (bool b)
        {
            b ? ++times_.at(col) : --times_.at(col);
        });
        connect (wait_item.get(), &symbol_item::select_changed, [this] ()
        {
            emit this->scene_changed();
        });
        addItem(wait_item.release());
        col++;

        auto temporary_storge_item = temporary_storage_item::make
                (QPointF(symbol_table_topleft.x() + 4 * (width_ / 10) + offset_x,
                         symbol_table_topleft.y() + offset_y + line_height_ * row));
        icons_.at(static_cast<size_t> (row)).at(static_cast<size_t> (col)) = temporary_storge_item.get();
        connect (temporary_storge_item.get (), &symbol_item::select_changed, [this, row, col] (bool b)
        {
            this->item_selection_changed(row, static_cast<int>(col), b);
        });
        connect (temporary_storge_item.get(), &symbol_item::select_changed, [this, col] (bool b)
        {
            b ? ++times_.at(col) : --times_.at(col);
        });
        connect (temporary_storge_item.get(), &symbol_item::select_changed, [this] ()
        {
            emit this->scene_changed();
        });
        addItem(temporary_storge_item.release());
        col++;
    }

}

void canvas_scene::draw_connected_lines(QPainter *painter)
{
    painter->save();
    QPen pen;
    pen.setWidthF (2.0);
    painter->setPen (pen);

    const symbol_item * last_item = nullptr;
    for (auto & it : icons_)
    {
        const symbol_item * current_item = nullptr;

        auto found = find_if (begin (it), end (it), [](auto && it)
        {
            return it->is_selected ();
        });
        if (found != end (it))
        {
            current_item = *found;
        }

        if (last_item != nullptr and current_item != nullptr)
        {
            painter->drawLine (last_item->sceneBoundingRect().center(),
                               current_item->sceneBoundingRect().center());
        }

        last_item = current_item;
    }

    painter->restore();
}

void canvas_scene::draw_counts(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);

    for(int col = 0; col < first_table_behind_col_; col++)
    {
        const auto i_time = times_.at(static_cast<size_t> (col));
        if(i_time == 0)
        {
            continue;
        }

        painter->drawText(QRectF(first_table_rect.x() + width_ / 2 + (width_ / 10) * col,
                                 first_table_rect.y() + line_height_,
                                 width_ / 10, line_height_), Qt::AlignCenter, QString::number(i_time));
    }
}

void canvas_scene::draw_times(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    std::array<qreal, 5> sums {};
    for(size_t row = 0; row < second_table_row_size_; row++)
    {
        const auto & row_icons = icons_.at(row);
        auto found = find_if (begin (row_icons), end (row_icons), [] (auto && item)
        {
            return item->is_selected();
        });

        if (found == end (row_icons))
        {
            continue;
        }

        size_t index = static_cast<size_t> (found - begin (row_icons));
        if (sums.at(index) < -1)
        {
            continue;
        }

        const auto cell = tablewidget_->item(static_cast<int>(row), 2);
        assert (cell != nullptr);

        const auto edit_data = cell->text();
        if (edit_data.isEmpty())
        {
            continue;
        }

        bool is_ok = false;
        const auto value = edit_data.toDouble(&is_ok);
        if (!is_ok)
        {
            sums.at(index) = - 100;
            continue;
        }

        sums.at(index) += value;
    }

    for (size_t col = 0; col < first_table_behind_col_; col++)
    {
        QString text;
        if(sums.at(col) < -1)
        {
            text = "错误";
        }
        else if(!(sums.at(col) > 0))
        {
//            qDebug() << "continue";
            continue;
        }
        else
        {
            text = QString::number(sums.at(col));
//            qDebug() <<  text;
//            qDebug() << col;
        }

        painter->drawText(QRectF(first_table_rect.x() + width_ / 2 + (width_ / 10) * col,
                                 first_table_rect.y() + 3 * line_height_,
                                 width_ / 10, line_height_), Qt::AlignCenter, text);
    }
}

void canvas_scene::draw_distance(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    std::array<qreal, 5> sums {};
    for(size_t row = 0; row < second_table_row_size_; row++)
    {
        const auto & row_icons = icons_.at(row);
        auto found = find_if(begin(row_icons), end(row_icons), [] (auto && item)
        {
            return item->is_selected();
        });

        if(found == end(row_icons))
        {
            continue;
        }

        size_t index = static_cast<size_t>(found - begin(row_icons));
        if(sums.at(index) < -1)
        {
            continue;
        }

        const auto cell = tablewidget_->item(static_cast<int>(row), 1);
        assert (cell);
        const auto edit_data = cell->text();

        if(edit_data.isEmpty())
        {
            continue;
        }

        bool is_ok = false;
        const auto value = edit_data.toDouble(&is_ok);
        if(!is_ok)
        {
            sums.at(index) = -100;
            continue;
        }

        sums.at(index) += value;
    }

    for (size_t col = 0; col < first_table_behind_col_; col++)
    {
        QString text;
        if(sums.at(col) < -1)
        {
            text = "错误";
        }
        else if(!(sums.at(col) > 0))
        {
            continue;
        }
        else
        {
            text = QString::number(sums.at(col));
        }

        painter->drawText(QRectF(first_table_rect.x() + width_ / 2 + (width_ / 10) * col,
                                 first_table_rect.y() + 2 * line_height_,
                                 width_ / 10, line_height_), Qt::AlignCenter, text);
    }
}


//void canvas_scene::change_second_tab_data(int col, const QString &content)
//{
//    qDebug () << " col " << col << "changed to : " << content;
//    second_table_data_.at(static_cast<size_t> (col)) = content;

//}

void canvas_scene::unselect_other(int row, int col, bool selected)
{
    if (!selected)
    {
        return;
    }

    int i = 0;
    for (auto & it : icons_.at(static_cast<size_t> (row)))
    {
        if (i != col)
        {
            it->set_selected(false);
        }
        i ++;
    }
}

void canvas_scene::hide(bool b)
{
    hide_state_ = b;
    if(b)
    {
        for(size_t row = 0; row < second_table_row_size_; row++)
        {
            for(auto & it : icons_.at(static_cast<size_t> (row)))
            {
                if(!it->is_selected())
                {
                    it->hide();
                }
            }
        }
    }
    else
    {
        for(size_t row = 0; row < second_table_row_size_; row++)
        {
            for(auto & it : icons_.at(static_cast<size_t> (row)))
            {
                if(!it->is_selected())
                {
                    it->show();
                }
            }
        }
    }
}

void canvas_scene::copy()
{
    tablewidget_->on_copy_del(table_widget::OPERATION_COPY);
}

void canvas_scene::cut()
{
    tablewidget_->on_copy_del(table_widget::OPERATION_COPY | table_widget::OPERATION_DEL);
}

void canvas_scene::paste()
{
    tablewidget_->on_paste();
}

void canvas_scene::del()
{
    tablewidget_->on_copy_del(table_widget::OPERATION_DEL);
}




