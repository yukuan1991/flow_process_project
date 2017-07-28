#include "table_widget.h"
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <boost/scope_exit.hpp>
#include <QMouseEvent>
#include <QMenu>
#include <QKeyEvent>

#include <QDebug>

void table_widget::mouseReleaseEvent(QMouseEvent *event)
{
    QTableWidget::mouseReleaseEvent(event);

//    auto item = this->itemAt(event->pos());
    if(event->button() == Qt::RightButton)
    {
        auto menu = std::make_unique<QMenu> ();

        auto action_cut = menu->addAction("剪切");
        auto action_copy = menu->addAction ("复制");
        auto action_paste = menu->addAction ("粘贴");
        auto action_del = menu->addAction ("删除");

        connect (action_copy, &QAction::triggered, [this] {on_copy_del (OPERATION_COPY);});
        connect (action_cut, &QAction::triggered, [this] {on_copy_del (OPERATION_DEL | OPERATION_COPY);});
        connect (action_del, &QAction::triggered, [this] { on_copy_del (OPERATION_DEL);});
        connect (action_paste, &QAction::triggered, this, &table_widget::on_paste);
        menu->exec(QCursor::pos());
    }
}

void table_widget::on_copy_del(unsigned flag)
{
    if (flag == OPERATION_DEL)
    {
        auto ret = QMessageBox::question(this, "删除", "是否删除?");
        if(ret != QMessageBox::Yes)
        {
            return;
        }
    }

    auto board = QApplication::clipboard();

    int min_row, min_col, max_row, max_col;
    bool is_ok;
    std::tie (min_row, min_col, max_row, max_col) = get_rect(&is_ok);
    if(!is_ok)
    {
        QMessageBox::information(this, "表格操作", "当前状态下无法操作");
        return;
    }

    QString clip_data;
    for(int i = min_row; i <= max_row; i++)
    {
        for(int j = min_col; j <= max_col; j++)
        {
            auto item = this->item(i, j);
            if(flag & OPERATION_COPY)
            {
                clip_data += item->text();

                if(j != max_col)
                {
                    clip_data += '\t';
                }
            }

            if(flag & OPERATION_DEL)
            {
                item->setData(Qt::EditRole, QString{});
            }
        }
        if (i != max_row)
        {
            clip_data += '\n';
        }
    }

    board->setText (clip_data);
    qDebug() << clip_data;
}

void table_widget::on_paste()
{
    int min_row, min_col;
    bool is_ok;
    std::tie(min_row, min_col, std::ignore, std::ignore) = get_rect(&is_ok);
    if (!is_ok)
    {
        QMessageBox::information (this, "粘贴", "无粘贴目的地");
        return;
    }

    auto data = get_clip_structure();
    if(data.empty())
    {
        return;
    }

    for(unsigned i = 0; i < data.size(); i++)
    {
        for(unsigned j = 0; j < data[i].size(); j ++)
        {
            auto current_row = min_row + static_cast<int>(i);
            auto current_col = min_col + static_cast<int>(j);
            if(current_row >= this->rowCount() or current_col >= this->columnCount())
            {
                continue;
            }
            auto item = this->item(current_row, current_col);
            item->setData(Qt::EditRole, data[i][j].data());
        }
    }
}

void table_widget::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if(event->key() == Qt::Key_C)
        {
            on_copy_del(OPERATION_COPY);
            return;
        }
        else if(event->key() == Qt::Key_X)
        {
            on_copy_del(OPERATION_COPY | OPERATION_DEL);
            return;
        }
        else if(event->key() == Qt::Key_V)
        {
            on_paste();
            return;
        }
    }

    QTableView::keyPressEvent(event);
}

std::vector<std::vector<std::string> > table_widget::get_clip_structure()
{
    auto board = QApplication::clipboard();
    auto clip = board->text().toStdString();
    if(clip.empty())
    {
        return {};
    }

    if(clip.back() == '\n')
    {
        clip.pop_back();
    }

    auto lambda_split = [](const std::string&str, char delim)
    {
        std::vector<std::string> splitted;

        unsigned split_begin = 0;
        for(unsigned i = 0; i < str.size(); i++)
        {
            if(str[i] == delim)
            {
                splitted.emplace_back(str.substr(split_begin, i - split_begin));
                split_begin = i + 1;
            }
        }
        splitted.emplace_back(str.substr(split_begin));

        return splitted;
    };

    auto lines = lambda_split(clip, '\n');
    std::vector<std::vector<std::string>> table_info;
    for_each(lines.begin(), lines.end(), [&](auto && iter) { table_info.emplace_back(lambda_split(iter, '\t'));});

    return table_info;
}

std::tuple<int, int, int, int> table_widget::get_rect(bool *ok)
{
    if (ok) *ok = true;
    auto ret = std::make_tuple(-1, -1, -1, -1);
    auto list = selectedItems();

    if(list.empty())
    {
        if(ok) *ok = false;
        return ret;
    }

    int min_row = 9999, max_row = 0, min_col = 9999, max_col = 0;
    for(const auto & iter : list)
    {
        if(iter->row() < min_row)
        {
            min_row = iter->row();
        }
        if(iter->row() > max_row)
        {
            max_row = iter->row();
        }

        if(iter->column() < min_col)
        {
            min_col = iter->column();
        }
        if(iter->column() > max_col)
        {
            max_col = iter->column();
        }
    }

    return std::make_tuple (min_row, min_col, max_row, max_col);
}
