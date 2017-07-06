#include "flow_process.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    flow_process w;
    w.show();

    return a.exec();
}
