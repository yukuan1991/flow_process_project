#pragma once
#include <QTableWidget>

class embed_table : public QTableWidget
{
public:
    embed_table(QWidget *parent = nullptr);
    embed_table(int rows, int columns, QWidget *parent = nullptr);

};
