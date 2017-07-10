#include "embed_table.h"

embed_table::embed_table(QWidget *parent)
    : QTableWidget (parent)
{

}

embed_table::embed_table(int rows, int columns, QWidget *parent)
    : QTableWidget (rows, columns, parent)
{

}
