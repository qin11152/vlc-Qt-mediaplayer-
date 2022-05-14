#pragma once

#include <QtWidgets/QWidget>
#include "ui_MyMediaplayer.h"

enum CycleStatus
{
    SingleCycle,
    ListCycle,
    None
};

class MyMediaplayer : public QWidget
{
    Q_OBJECT

public:
    MyMediaplayer(QWidget *parent = Q_NULLPTR);

private:
    Ui::MyMediaplayerClass ui;
};
