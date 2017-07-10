#pragma once
#include <QGroupBox>

class embed_groupbox : public QGroupBox
{
public:    
    embed_groupbox(QWidget *parent = nullptr);
    embed_groupbox(const QString &title, QWidget *parent = nullptr);

};

