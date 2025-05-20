#pragma once

#include "startwindow.h"
#include "game.h"

#include <QWidget>
#include <QStackedWidget>

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget* prnt = nullptr);
    void set_game_level(int gl);

protected:
    virtual void showEvent(QShowEvent*) override;

private:
    void animate_transition(QStackedWidget* stack, int next_index);

    StartWindow* first_page;
    Game* second_page;
    QStackedWidget* stack;

    bool is_init = false;
    int game_level{};
};

