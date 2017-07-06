#pragma once
#include <QDialog>
#include <memory>

struct ui_about_us_dlg;

class about_us_dlg final : public QDialog
{
public:
    static void show_info (const QString & text, const QString & qr_code);
    static std::unique_ptr<about_us_dlg> make (QWidget * parent = nullptr);
    ~about_us_dlg () override;
    void set_qr_code_text (const QString & text);
    void set_description (const QString & text);
private:
    about_us_dlg(QWidget * parent);
    bool init ();
    bool init_ui ();
private:
    std::unique_ptr<ui_about_us_dlg> ui;
};


