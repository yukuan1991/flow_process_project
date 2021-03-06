﻿#pragma once
#include <QDialog>
#include <QComboBox>

class QLabel;
class unit_dlg : public QDialog
{
    Q_OBJECT
signals:
    void currenttextchanged(const QString &s);
    void confirm();
    void cancel();
public:
    unit_dlg(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void set_unit(const QString& s) { unit_->setCurrentText(s); }
    QString currentText() const { return unit_->currentText(); }
protected:
    QComboBox* unit_;
    QLabel* label_;
    QPushButton* confirm_;
    QPushButton* cancel_;
};

