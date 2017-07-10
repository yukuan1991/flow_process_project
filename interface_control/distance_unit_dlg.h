#pragma once
#include <QDialog>

class QComboBox;
class QLabel;
class distance_unit_dlg : public QDialog
{
    Q_OBJECT
signals:
    void currenttextchanged(const QString &s);
public:
    distance_unit_dlg(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
private:
    QComboBox* unit_;
    QLabel* time_;
};

