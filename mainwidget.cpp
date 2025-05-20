#include "mainwidget.h"

#include <QParallelAnimationGroup>
#include <QVBoxLayout>

MainWidget::MainWidget(QWidget *prnt)
    : QWidget(prnt)
{}

void MainWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if(!is_init) {
        first_page = new StartWindow(rect().size(),this,this);
        second_page = new Game(this);
        stack = new QStackedWidget(this);
        stack->addWidget(first_page);
        stack->addWidget(second_page);
        stack->setCurrentIndex(0);

        QVBoxLayout* vbox = new QVBoxLayout(this);
        vbox->addWidget(stack);

        is_init = true;

        // switch to game page
        connect(first_page, &StartWindow::start_pressed, this, [=] {
            animate_transition(stack, 1);
            second_page->game_start(game_level);
        });

        // switch back to menu
        connect(second_page, &Game::switch_to_menu, this, [=]{
            animate_transition(stack, 0);
            second_page->restore_play_screen();
        });
    }
}

void MainWidget::animate_transition(QStackedWidget* stack, int next_index)
{
    QWidget* current_widget = stack->currentWidget();
    QWidget* next_widget = stack->widget(next_index);

    int w = stack->width();

    next_widget->setGeometry((next_index > stack->currentIndex() ? w : -w), 0, w, stack->height());
    next_widget->show();

    QPropertyAnimation* anim_old = new QPropertyAnimation(current_widget, "geometry");
    QPropertyAnimation* anim_new = new QPropertyAnimation(next_widget, "geometry");

    anim_old->setDuration(300);
    anim_new->setDuration(300);

    anim_old->setStartValue(current_widget->geometry());
    anim_old->setEndValue(QRect((next_index > stack->currentIndex() ? -w : w), 0, w, stack->height()));

    anim_new->setStartValue(next_widget->geometry());
    anim_new->setEndValue(QRect(0, 0, w, stack->height()));

    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    group->addAnimation(anim_old);
    group->addAnimation(anim_new);

    QObject::connect(group, &QParallelAnimationGroup::finished, this, [=]() {
        stack->setCurrentIndex(next_index);
        current_widget->hide();
        next_widget->setGeometry(0, 0, w, stack->height());
        group->deleteLater();
    });
    group->start();
}

void MainWidget::set_game_level(int gl)
{
    game_level = gl;
}
