#include "flow_process.h"
#include <QApplication>
#include <base/io/file/file.hpp>
#include <boost/filesystem.hpp>
#include <QStyleFactory>
#include <boost/range/adaptors.hpp>
#include "interface_control/flow_process_ribbon.h"
#include "interface_control/time_unit_dlg.h"
#include "interface_control/distance_unit_dlg.h"


void set_style ()
{
    using namespace boost::filesystem;

    auto rng = boost::make_iterator_range (directory_iterator ("."), directory_iterator ());

    std::string qss;
    for (auto & it : rng)
    {
        if (it.path ().extension ().string () == ".css")
        {
            auto res = file::read_all (it.path ().string ().data ());
            if (res)
            {
                qss += res.value ();
            }
        }
    }
    qApp->setStyle (QStyleFactory::create ("fusion"));

    qApp->setStyleSheet (QString::fromStdString (qss));
    qApp->setFont (QFont ("宋体", 11));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    set_style();
//    time_unit_dlg t;
//    t.exec();
    flow_process w;
    w.show();

    return a.exec();
}
