#include "about_us_dlg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <base/lang/scope.hpp>
#include <QUrl>


using namespace std;

struct ui_about_us_dlg
{
    unique_ptr<QLabel> qr_code;
    unique_ptr<QLabel> text;
};

void about_us_dlg::show_info(const QString &text, const QString &qr_code)
{
    auto w = about_us_dlg::make ();
    w->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    w->set_description (text);
    w->set_qr_code_text (qr_code);
    w->exec ();
}

unique_ptr<about_us_dlg> about_us_dlg::make(QWidget *parent)
{
    auto ret = unique_ptr<about_us_dlg> (new about_us_dlg (parent));
    if (ret->init () == false)
    {
        return nullptr;
    }
    else
    {
        return ret;
    }
}

about_us_dlg::~about_us_dlg()
{

}

void about_us_dlg::set_qr_code_text(const QString &text)
{
    ui->qr_code->setText (text);
}

void about_us_dlg::set_description(const QString &text)
{
    ui->text->setText (text);
}

about_us_dlg::about_us_dlg(QWidget *parent)
    :QDialog (parent)
{

}

bool about_us_dlg::init()
{
    return init_ui ();
}

bool about_us_dlg::init_ui()
{
    ui = make_unique<ui_about_us_dlg> ();
    auto layout = make_unique<QHBoxLayout> ();
    SCOPE_EXIT { setLayout (layout.release ()); };

    int i = 0;

    {
        ui->text = make_unique<QLabel> (this);
        SCOPE_EXIT { layout->addWidget (ui->text.get ()); };
    }
    layout->setStretch (i ++, 1);

    {
        ui->qr_code = make_unique<QLabel> (this);
        SCOPE_EXIT { layout->addWidget (ui->qr_code.get ()); };
        connect (ui->qr_code.get (), &QLabel::linkActivated, [] (auto &&link)
        {
            QDesktopServices::openUrl (QUrl {link});
        });
    }
    layout->setStretch (i ++, 2);

    return true;
}

