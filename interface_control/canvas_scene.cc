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

const QPointF canvas_scene::top_left = QPointF (0, 0);

using std::begin;
using std::string;
using std::end;
using std::find_if;
using nlohmann::json;

bool canvas_scene::init()
{
    second_table_edit_.resize (second_table_row_size_);
    for (auto & it : second_table_edit_)
    {
        it.resize (second_table_front_col);
    }

    //unit_edit_.resize(2);

    icons_.resize(second_table_row_size_);
    for (auto & it : icons_)
    {
        it.resize(second_table_behind_col);
    }

    connect (this, &canvas_scene::item_selection_changed, this, &canvas_scene::unselect_other);

    setBackgroundBrush(QColor (230, 230, 230));

    setSceneRect(0, 0, 1500, 1000);

    const auto offset = 20;
    print_rect_ = QRectF(0, 0, width_, line_height_ * 31 + offset);

    set_lineedit();
    add_symbol_item();

    //connect (this, &canvas_scene::scene_changed, [this] () { qDebug() << "scene changed"; });


    return true;
}

string canvas_scene::dump()
{
    using boost::range::find_if;
//    using std::end;
//    using std::begin;

    json data;
    data ["单位"]["时间"] = edit_time_unit_->text ().toStdString ();
    data ["单位"]["距离"] = edit_distance_unit_->text ().toStdString ();

    data ["数据"] = json::array ();
    for (int row = 0; row < second_table_row_size_; row ++)
    {
        const auto u_row = static_cast<size_t> (row);
        auto work_title = second_table_edit_.at (u_row).at (0)->text().toStdString();
        auto distance = second_table_edit_.at (u_row).at (1)->text().toStdString();
        auto time = second_table_edit_.at (u_row).at (2)->text().toStdString();

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
    edit_distance_unit_->setText(distance_unit.data());
    edit_time_unit_->setText (time_unit.data ());

    auto & table = content["数据"];

    size_t row = 0;
    for (auto & it : table)
    {
        const string work_title = it["工作说明"];
        const string time = it["时间"];
        const string distance = it ["距离"];

        second_table_edit_.at(row).at (0)->setText(work_title.data());
        second_table_edit_.at(row).at (1)->setText(distance.data());
        second_table_edit_.at(row).at (2)->setText(time.data());

        int pos = it["图标"];

        if (pos >= 0)
        {
            icons_.at(row).at(static_cast<size_t> (pos))->set_selected(true);
        }
        row ++;
    }

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

/*每行高40个像素，共32行*/
void canvas_scene::draw_table(QPainter *painter)
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    painter->drawRect(first_table_rect);

    /*表一、表二的上下间隔为20*/
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
                                         first_table_rect.bottomLeft().y() + 20);
    const auto second_table_rect = QRectF(second_table_topleft.x(),
                                          second_table_topleft.y(),
                                          width_, second_table_height_);

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
                      QPointF(second_table_rect.x() + width_,
                              second_table_rect.y() + 2 * line_height_));
    painter->drawLine(QPointF(second_table_rect.x() + width_ / 2,
                              second_table_rect.y() + line_height_),
                      QPointF(second_table_rect.x() + width_,
                              second_table_rect.y() + line_height_));

    for(int i = 1; i < second_table_row_size_; i++)
    {
        painter->drawLine(QPointF(second_table_rect.x(),
                                  second_table_rect.y() + (2 + i) * line_height_),
                          QPointF(second_table_rect.x() + width_ / 2,
                                  second_table_rect.y() + (2 + i) * line_height_ ));
    }

    /*表二 一分为二的竖线*/
    painter->drawLine(QPointF(second_table_rect.x() + width_ / 2,
                              second_table_rect.y()),
                      QPointF(second_table_rect.bottomLeft().x() + width_ / 2,
                              second_table_rect.bottomLeft().y()));




    /*表二 序号栏行宽55, 工作说明栏行宽150， 距离栏及时间栏行宽85*/
    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_,
                              second_table_rect.y()),
                      QPointF(second_table_rect.x() + serial_number_width_,
                              second_table_rect.y() + second_table_height_));
    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_,
                              second_table_rect.y()),
                      QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_,
                              second_table_rect.y() + second_table_height_));
    painter->drawLine(QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_ + distance_width_,
                              second_table_rect.y()),
                      QPointF(second_table_rect.x() + serial_number_width_ + job_content_width_ + distance_width_,
                              second_table_rect.y() + second_table_height_));
    painter->drawLine(QPointF(second_table_rect.x() + width_ / 2, second_table_rect.y()),
                      QPointF(second_table_rect.x() + width_ / 2,
                              second_table_rect.y() + second_table_height_));


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
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
                                         first_table_rect.bottomLeft().y() + 20);

    QFont font("宋体", 12);
    painter->setFont(font);

    /*表二 各列的标题*/
    painter->drawText(QRectF(second_table_topleft.x(), second_table_topleft.y(),
                             serial_number_width_, 2 * line_height_),
                      Qt::AlignCenter, tr("序号"));
    painter->drawText(QRectF(second_table_topleft.x() + serial_number_width_,
                             second_table_topleft.y(),job_content_width_, 2 * line_height_),
                      Qt::AlignCenter, tr("工作说明"));
    painter->drawText(QRectF(second_table_topleft.x() + serial_number_width_ + job_content_width_,
                             second_table_topleft.y(),
                             distance_width_, 2 * line_height_), Qt::AlignCenter, tr("距离"));
    const auto offset = 2;
    {
        painter->save();
        QFont font("宋体", 10);
        painter->setFont(font);

        painter->drawText(QRectF(second_table_topleft.x() + serial_number_width_ +
                                 job_content_width_ + offset,
                          second_table_topleft.y() + line_height_ + line_height_ / 2 - 2 * offset,
                                 distance_width_ / 2, line_height_ /2),
                          Qt::AlignCenter, tr("单位:"));
        painter->drawText(QRectF(second_table_topleft.x() + serial_number_width_ +
                                 job_content_width_ + distance_width_ +offset,
                          second_table_topleft.y() + line_height_ + line_height_ / 2 - 2 * offset,
                                 time_width_ / 2, line_height_ /2),
                          Qt::AlignCenter, tr("单位:"));
        painter->restore();
    }
    painter->drawText(QRectF(second_table_topleft.x() + serial_number_width_ +
                             job_content_width_ + distance_width_,
                             second_table_topleft.y(),
                             time_width_, 2 * line_height_), Qt::AlignCenter, tr("时间"));
    painter->drawText(QRectF(second_table_topleft.x() + width_ / 2,
                             second_table_topleft.y(), width_ / 2, line_height_),
                             Qt::AlignCenter, tr("符号"));

    /*表二 序号部分*/
    for(int i = 1; i <= second_table_row_size_; i++)
    {
        painter->save();
        QFont font("宋体", 10);
        painter->setFont(font);
        auto serial_num = QString::number(i);
        painter->drawText(QRectF(second_table_topleft.x() ,
                                 second_table_topleft.y() + (i + 1) * line_height_,
                                 serial_number_width_, line_height_),
                          Qt::AlignCenter, serial_num);
        painter->restore();
    }

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
                                 line_height_), Qt::AlignCenter, "时间单位:" + edit_time_unit_->text ());

        painter->drawText(QRectF(first_table_rect.x(),
                                 first_table_rect.y() + 3 * line_height_, width_ / 2,
                                 line_height_), Qt::AlignCenter, "距离单位:"  + edit_distance_unit_->text ());

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
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
                                         first_table_rect.bottomLeft().y() + 20);


    /*表二符号部分*/
    {
        const auto width = 20;
        const auto height = 20;
        const auto offset_tab_x = 25;
        const auto offset_tab_y = 5;
        const QPointF symbol_table_topleft(second_table_topleft.x() + width_ / 2,
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
        QFont font("宋体", 17);
        painter->setFont(font);
        QRectF wait_rect(symbol_table_topleft.x() + 3 * (width_ / 10) + offset_tab_x,
                         symbol_table_topleft.y() + offset_tab_y,
                         width, height);
        painter->drawText(wait_rect, Qt::AlignCenter, tr("D"));
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
        QFont font("宋体", 17);
        painter->setFont(font);
        QRectF wait_rect(symbol_table_topleft.x() + 3.5 * (width_ / 10),
                               symbol_table_topleft.y(),
                               (width_ / 10 / 2), line_height_);
        painter->drawText(wait_rect, Qt::AlignCenter, tr("D"));
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

void canvas_scene::set_lineedit()
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
                                         first_table_rect.bottomLeft().y() + 20);
    const auto offset = 2;
    for(int row = 0; row < second_table_row_size_; row++)
    {
        size_t col = 0;
        const auto u_row = static_cast<size_t> (row);
        {
            auto edit = std::make_unique<QLineEdit> ();

            edit->setFont(QFont("宋体", 10));

            connect(edit.get(), &QLineEdit::textChanged, [this](){ emit scene_changed(); });

            edit->resize (job_content_width_ - offset, line_height_ - offset);
            second_table_edit_.at(u_row).at(col) = edit.get();

            auto proxy = addWidget (edit.release());
            proxy->setPos(second_table_topleft + QPointF (offset_topleft_width_, offset_topleft_height_)
                           + QPointF (serial_number_width_, 2 * line_height_)
                           + QPointF (0, row * line_height_) + QPointF (offset, offset));
        }
        col ++;

        {

            auto edit = std::make_unique<QLineEdit> ();

            edit->setFont(QFont("宋体", 10));
            edit->setAlignment(Qt::AlignCenter);

            connect(edit.get(), &QLineEdit::textChanged, [this](){ emit scene_changed(); });


            edit->resize (distance_width_ - offset, line_height_ - offset);
            second_table_edit_.at(u_row).at(col) = edit.get();
            auto proxy = addWidget (edit.release());
            proxy->setPos(second_table_topleft + QPointF (offset_topleft_width_, offset_topleft_height_) +
                           QPointF (serial_number_width_, 2 * line_height_) +
                           QPointF (job_content_width_, row * line_height_) + QPointF (offset, offset));

        }
        col ++;

        {
            auto edit = std::make_unique<QLineEdit> ();

            edit->setFont(QFont("宋体", 10));
            edit->setAlignment(Qt::AlignCenter);

            connect(edit.get(), &QLineEdit::textChanged, [this](){ emit scene_changed(); });

            edit->resize (time_width_ - offset, line_height_ - offset);
            second_table_edit_.at(u_row).at(col) = edit.get();

            auto proxy = addWidget (edit.release());
            proxy->setPos(second_table_topleft + QPointF (offset_topleft_width_, offset_topleft_height_) +
                           QPointF (serial_number_width_, 2 * line_height_) +
                           QPointF (job_content_width_ + distance_width_, row * line_height_) +
                           QPointF (offset, offset));
        }
        col ++;
    }

    /*距离单位edit*/
    {
        auto edit = std::make_unique<QLineEdit> ();

        edit->setFont(QFont("宋体", 10));
        edit->setAlignment(Qt::AlignCenter);

        connect(edit.get(), &QLineEdit::textChanged, [this](){ emit scene_changed(); });

        edit->resize(distance_width_ / 2 - 2 * offset, static_cast<int>(line_height_ / 1.5 - offset));
        edit_distance_unit_ = edit.get();
        //unit_edit_.at(static_cast<size_t>(0)) = edit.get();

        auto proxy = addWidget(edit.release());
        proxy->setPos(second_table_topleft.x() + serial_number_width_ + job_content_width_ + distance_width_ / 2 +offset,
                      second_table_topleft.y() + line_height_ + line_height_ / 2 - 2 * offset);
    }

    /*时间单位edit*/
    {
        auto edit = std::make_unique<QLineEdit> ();

        edit->setFont(QFont("宋体", 10));
        edit->setAlignment(Qt::AlignCenter);

        connect(edit.get(), &QLineEdit::textChanged, [this](){ emit scene_changed(); });

        edit->resize(time_width_ / 2 - 2 * offset, static_cast<int>(line_height_ / 1.5 - offset));
        edit_time_unit_ = edit.get();
        //unit_edit_.at(static_cast<size_t>(1)) = edit.get();

        auto proxy = addWidget(edit.release());
        proxy->setPos(second_table_topleft.x() + serial_number_width_ +
                      job_content_width_ + distance_width_ + time_width_ / 2 + offset,
                      second_table_topleft.y() + line_height_  + line_height_ / 2 - 2 * offset);
    }
}

void canvas_scene::add_symbol_item()
{
    const auto first_table_rect = QRectF(top_left.x() + offset_topleft_width_,
                                         top_left.y() + offset_topleft_height_,
                                         width_, first_table_height_);
    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
                                         first_table_rect.bottomLeft().y() + 20);

    QPointF symbol_table_topleft(second_table_topleft.x() + width_ / 2,
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
//    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
//                                               first_table_rect.bottomLeft().y() + 20);
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
//    const auto second_table_topleft =  QPointF(first_table_rect.bottomLeft().x() ,
//                                               first_table_rect.bottomLeft().y() + 20);
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

        const auto edit_data = second_table_edit_.at(row).at(2)->text();
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
        else if(static_cast<int>(sums.at(col)) == 0)
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

        const auto edit_data = second_table_edit_.at(row).at(1)->text();
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
        else if(static_cast<int>(sums.at(col)) == 0)
        {
            continue;
        }
        else
        {
            text = QString::number(sums.at(col));
        }

        painter->drawText(QRectF(first_table_rect.x() + width_ / 2 + (width_ / 10) * col,
                                 first_table_rect.y() + 3 * line_height_,
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


