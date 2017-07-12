#pragma once
#include <QDialog>
#include <QComboBox>

class QLabel;
class distance_unit_dlg : public QDialog
{
    Q_OBJECT
signals:
    void currenttextchanged(const QString &s);
public:
    distance_unit_dlg(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void set_unit(const QString& s) { unit_->setCurrentText(s); }
    QString currentText() const;

private:
    QComboBox* unit_;
    QLabel* time_;
    QPushButton* confirm_;
    QPushButton* cancel_;
};

