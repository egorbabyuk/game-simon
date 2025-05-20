#pragma once

#include "simplecube.h"
#include "custombutton.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <random>

class Game : public QWidget
{
    Q_OBJECT
public:
    Game(QWidget* prnt = nullptr);

    void game_start(int level);
    void game_logic(int level);
    void restore_play_screen();

signals:
    void target_cube();
    void next_round();
    void switch_to_menu();
    void start();

private slots:
    void player_start(SimpleCube*);

private:
    void setCubesEnable(bool value);
    void timer_before_start();
    void lose_screen();

    QTimer start_timer;

    int game_level;
    int time_before_start{1};
    int current_score{};
    int total_score{};
    int vector_counter{};
    int current_rounds;
    int current_speed;

    enum levels {easy, medium, hard};
    enum levels_speed {easy_speed = 1000, medium_speed = 700, hard_speed = 500 };
    enum rounds {easy_rounds = 2, medium_rounds = 3, hard_rounds = 5};

    QGraphicsView* view = nullptr;
    QGraphicsScene* scene = nullptr;
    QGraphicsTextItem* score_text = nullptr;

    QGraphicsTextItem* lose_score_text = nullptr;
    QGraphicsBlurEffect* blur_for_lose = nullptr;
    QGraphicsPixmapItem* blur_pix = nullptr;
    QGraphicsRectItem* lose_rect = nullptr;
    QGraphicsWidget* lose_widget = nullptr;
    CustomButton* restart_btn = nullptr;
    CustomButton* menu_btn = nullptr;

    QVector<SimpleCube*> cubes;
    QVector<int> combination;

    std::mt19937 myRandom;
};



