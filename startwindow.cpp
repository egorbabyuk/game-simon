#include "startwindow.h"
#include "custombutton.h"
#include "mainwidget.h"

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QVBoxLayout>

StartWindow::StartWindow(QSize size, QWidget *prnt, QWidget* main)
    : QWidget(prnt)
{
    resize(size);

    CustomButton* start_btn = new CustomButton("START",QSize(250,50), this);
    CustomButton* level_btn = new CustomButton("SET LEVEL", QSize(250,50), this);
    level_btn->set_level_switch(true);
    CustomButton* exit_btn = new CustomButton("EXIT",QSize(250,50), this);

    QGraphicsDropShadowEffect* shadow_start = new QGraphicsDropShadowEffect(this);
    shadow_start->setColor(QColor(5,85,165));
    shadow_start->setOffset(3,6);
    QGraphicsDropShadowEffect* shadow_level = new QGraphicsDropShadowEffect;
    shadow_level->setColor(QColor(5,85,165));
    shadow_level->setOffset(3,6);
    QGraphicsDropShadowEffect* shadow_exit = new QGraphicsDropShadowEffect;
    shadow_exit->setColor(QColor(5,85,165));
    shadow_exit->setOffset(3,6);

    start_btn->setGraphicsEffect(shadow_start);
    level_btn->setGraphicsEffect(shadow_level);
    exit_btn->setGraphicsEffect(shadow_exit);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->setAlignment(Qt::AlignCenter);
    vbox->addWidget(start_btn);
    vbox->addSpacing(25);
    vbox->addWidget(level_btn);
    vbox->addSpacing(25);
    vbox->addWidget(exit_btn);

    connect(start_btn, &CustomButton::clicked, this, [this]() { emit start_pressed();});
    connect(level_btn, &CustomButton::level_changed, this, [=]() {
        MainWidget* main_widget = qobject_cast<MainWidget*>(main);
        if(main_widget) {
            main_widget->set_game_level(level_btn->get_level() - 1);
        }
    });
    connect(exit_btn, &CustomButton::clicked, this, []() { QApplication::quit(); });
}

void StartWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing);

    paint.drawPixmap(QPoint(0,0),QPixmap(":/background/background2.png"));
}
