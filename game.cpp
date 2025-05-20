#include "game.h"

#include <QLabel>
#include <QParallelAnimationGroup>
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextCursor>
#include <algorithm>

Game::Game(QWidget *prnt)
    : QWidget(prnt), myRandom(std::random_device{}())
{
    QSize size(350,350);
    setMinimumSize(size);

    scene = new QGraphicsScene(0,0, size.width() - 30, size.height() - 30,this);

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFrameStyle(QGraphicsView::NoFrame);

    // background
    QGraphicsPixmapItem* background = new QGraphicsPixmapItem(QPixmap(":/background/background2.png"));
    background->setPos(view->mapToScene(30,25));
    scene->addItem(background);

    // score text
    QFont font_text("Intro", 22, 4);
    font_text.setBold(true);
    score_text = new QGraphicsTextItem;
    score_text->setFont(font_text);
    score_text->setDefaultTextColor(QColor(255,255,255));
    score_text->setPlainText("SCORE " + QString::number(total_score));
    score_text->setX(scene->sceneRect().center().x() - (score_text->boundingRect().width() / 2));
    score_text->setY(-45);
    scene->addItem(score_text);

    QGraphicsDropShadowEffect* text_shadow = new QGraphicsDropShadowEffect;
    text_shadow->setColor(QColor(12,29,67));
    text_shadow->setOffset(0,4);
    text_shadow->setBlurRadius(8);
    score_text->setGraphicsEffect(text_shadow);

    // creating cubes
    const int cube_size = 70;
    const int spacing = 10;
    const int grid_cols = 3;
    const int grid_rows = 3;

    int grid_width = grid_cols * cube_size + (grid_cols - 1) * spacing;
    int grid_height = grid_rows * cube_size + (grid_rows - 1) * spacing;

    int offsetX = (scene->sceneRect().width() - grid_width) / 2;
    int offsetY = (scene->sceneRect().height() - grid_height) / 2;

    QPainterPath path;
    path.addRoundedRect(0,0, cube_size,cube_size, 15,15);

    for(int i{}; i < 9; ++i) {
        auto* cube = new SimpleCube(path);

        int row = i / grid_cols;
        int col = i % grid_cols;

        int x = offsetX + col * (cube_size + spacing);
        int y = offsetY + row * (cube_size + spacing);

        cube->setPos(x, y);

        QGraphicsDropShadowEffect* cube_shadow = new QGraphicsDropShadowEffect;
        cube_shadow->setColor(QColor(18,69,127));
        cube_shadow->setOffset(3,6);
        cube_shadow->setBlurRadius(5);

        cube->setGraphicsEffect(cube_shadow);
        cube->setCubeEnabled(false);

        scene->addItem(cube);
        cubes.append(cube);

        connect(cube, &SimpleCube::clicked, this, &Game::player_start);
    }

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0,0,0,0);
    vbox->setAlignment(Qt::AlignCenter);
    vbox->addWidget(view);

    // initialization vector of cubes positions
    for(int i{}, number{}; i < 9; ++i) {
        combination.append(number);
        if(number++ == 8)
            number = 0;
    }
}

void Game::game_start(int level)
{
    game_level = level;

    start_timer.start(300);
    start_timer.setSingleShot(true);

    disconnect(&start_timer, nullptr, nullptr, nullptr);
    connect(&start_timer, &QTimer::timeout, this, [=]() { game_logic(level); });
}

void Game::game_logic(int level)
{
    // backlighting cubes with combination
    auto game_cycle = [=](int backlights_amount, int speed) {
        int counter{};
        int max_count = backlights_amount;

        QTimer* comb_timer = new QTimer(this);

        SimpleCube* prev_cube = nullptr;

        comb_timer->start(speed);
        connect(comb_timer, &QTimer::timeout, this, [=]() mutable {
            // disconnect already backlighted cube
            if(prev_cube) {
                disconnect(this, &Game::target_cube, prev_cube, &SimpleCube::target_backlight);
            }

            // current cube for backlight by combination
            SimpleCube* current_cube = cubes[combination[counter]];

            connect(this, &Game::target_cube, current_cube, &SimpleCube::target_backlight);
            emit target_cube();

            prev_cube = current_cube;

            if(++counter >= max_count) {
                comb_timer->stop();
                // disconnect last baclkighted cube
                disconnect(this, &Game::target_cube, prev_cube, &SimpleCube::target_backlight);

                // timer that waits for backlight end
                QTimer::singleShot(600,this, [=](){setCubesEnable(true);});

                delete comb_timer;
                comb_timer = nullptr;
            }
        });
    };
    // func for combines timer_before_start and game_cycle
    auto starting_func = [=](int level) {
        current_rounds = level;
        timer_before_start();

        disconnect(this, &Game::start, nullptr,nullptr);

        connect(this, &Game::start, this, [=]() {
            game_cycle(current_rounds, current_speed);
        });
    };

    // shuffle combination for backlighting cubes
    std::shuffle(std::begin(combination), std::end(combination), myRandom);

    if(level == easy) {
        current_speed = easy_speed;
        starting_func(easy_rounds);
    }
    else if(level == medium) {
        current_speed = medium_speed;
        starting_func(medium_rounds);
    }
    else {
        current_speed = hard_speed;
        starting_func(hard_rounds);
    }
}

void Game::restore_play_screen()
{
    lose_widget->setOpacity(0);
    blur_pix->setOpacity(0);

    lose_rect->setOpacity(0);
    lose_rect->hide();

    lose_score_text->hide();

    total_score = 0;
    vector_counter = 0;

    score_text->setPlainText("SCORE " + QString::number(total_score));
}

void Game::player_start(SimpleCube* current_cube)
{
    // if pressed cube is wrong
    if(cubes[combination[vector_counter]] != current_cube) {
        current_cube->wrong_target();
        setCubesEnable(false);

        // lose blur
        QTimer::singleShot(500, this, [=]() {
            QPixmap screen = view->grab();

            // create QTextDocument for total score after lose
            auto set_text = [=]() {
                QTextDocument* doc = new QTextDocument;
                QTextCursor cursor(doc);

                QTextBlockFormat centerFormat;
                centerFormat.setAlignment(Qt::AlignCenter);

                QTextCharFormat textFormat;
                textFormat.setFont(score_text->font());
                textFormat.setForeground(QColor(255,255,255));

                cursor.insertBlock(centerFormat);
                cursor.insertText("TOTAL SCORE:", textFormat);
                cursor.insertBlock(centerFormat);
                cursor.insertText(QString::number(total_score), textFormat);

                return doc;
            };

            if(!blur_pix) {
                blur_pix = new QGraphicsPixmapItem(screen);
                blur_pix->setPos(view->mapToScene(0,0));

                blur_for_lose = new QGraphicsBlurEffect;
                blur_for_lose->setBlurRadius(16);
                blur_pix->setGraphicsEffect(blur_for_lose);

                lose_rect = new QGraphicsRectItem(0,0, view->rect().width(), view->rect().height());
                lose_rect->setPos(view->mapToScene(0,0));
                lose_rect->setBrush(QColor(242,35,49));
                lose_rect->setOpacity(0);

                scene->addItem(blur_pix);
                scene->addItem(lose_rect);

                // lose score
                QTextDocument* doc(set_text());
                lose_score_text = new QGraphicsTextItem;
                lose_score_text->setDocument(set_text());
                lose_score_text->setTextWidth(doc->idealWidth());
                lose_score_text->setX(scene->sceneRect().center().x() - (lose_score_text->boundingRect().width() / 2));
                lose_score_text->setDefaultTextColor(QColor(255,255,255));

                scene->addItem(lose_score_text);

                // lose buttons
                restart_btn = new CustomButton("RESTART", QSize(250,50));
                menu_btn = new CustomButton("MENU", QSize(250,50));

                QGraphicsLinearLayout* linear = new QGraphicsLinearLayout(Qt::Vertical);
                lose_widget = new QGraphicsWidget;
                lose_widget->setLayout(linear);

                QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
                shadow->setOffset(4,8);
                shadow->setBlurRadius(7);
                shadow->setColor(QColor(5,85,165));
                lose_widget->setGraphicsEffect(shadow);

                QGraphicsProxyWidget* proxy_exit = scene->addWidget(menu_btn);
                QGraphicsProxyWidget* proxy_restart = scene->addWidget(restart_btn);
                linear->addItem(proxy_exit);
                linear->addItem(proxy_restart);

                lose_widget->setPos(scene->sceneRect().center().x() - 5 - restart_btn->width() / 2,
                                    scene->sceneRect().center().y() - restart_btn->height() / 2);

                scene->addItem(lose_widget);

                connect(restart_btn, &CustomButton::clicked, this, [=]() {
                    game_logic(game_level);
                    restore_play_screen();
                });

                connect(menu_btn, &CustomButton::clicked, this, [=]() { emit switch_to_menu(); });
            }
            else {
                scene->removeItem(blur_pix);
                delete blur_pix;

                blur_pix = new QGraphicsPixmapItem(screen);
                blur_pix->setPos(view->mapToScene(0,0));

                blur_for_lose = new QGraphicsBlurEffect;
                blur_for_lose->setBlurRadius(16);
                blur_pix->setGraphicsEffect(blur_for_lose);

                scene->addItem(blur_pix);

                QTextDocument* doc(set_text());
                lose_score_text->setDocument(doc);
                lose_score_text->setTextWidth(doc->idealWidth());

                blur_pix->setZValue(0);
                lose_rect->setZValue(1);
                lose_score_text->setZValue(2);
                lose_widget->setZValue(2);

                lose_widget->setOpacity(1);
            }

            lose_screen();
        });
    }
    // if pressed cube is right
    else {
        current_cube->correct_target();

        ++total_score;
        ++vector_counter;

        score_text->setPlainText("SCORE " + QString::number(total_score));

        // go to the next stage
        if(vector_counter == current_rounds) {
            for(int i{}; i < cubes.size(); ++i)
                cubes[i]->correct_target();

            // increasing speed
            if(current_speed == easy_speed && current_speed > 700)
                current_speed -= 35;
            else if(current_speed == medium_speed && current_speed > 500)
                current_speed -= 15;
            else if(current_speed == hard_speed && current_speed > 300)
                current_speed -= 10;

            vector_counter = 0;
            ++current_rounds;

            std::shuffle(std::begin(combination), std::end(combination), myRandom);
            timer_before_start();
        }
    }
}

void Game::setCubesEnable(bool value)
{
    for(int i{}; i < cubes.size(); ++i)
        cubes[i]->setCubeEnabled(value);
}

void Game::timer_before_start()
{
    QTimer* timer = new QTimer(this);

    QFont text("Univers Condensed Bold", 75, 4);
    text.setBold(true);
    auto* time_text = new QGraphicsTextItem;
    time_text->setFont(text);
    time_text->setDefaultTextColor(QColor(255,255,255));
    time_text->setOpacity(0);
    time_text->hide();
    scene->addItem(time_text);

    setCubesEnable(false);

    timer->start(500);
    connect(timer, &QTimer::timeout, this, [=]() {
        time_text->setPlainText(QString::number(time_before_start));
        time_text->setPos(scene->sceneRect().center().x() - time_text->boundingRect().width() / 2,
                          scene->sceneRect().center().y() - time_text->boundingRect().height() / 2);
        time_text->show();

        if(time_before_start++ == 3) {
            time_before_start = 1;
            timer->stop();

            emit start();
        }

        time_text->setTransformOriginPoint(time_text->boundingRect().center());

        auto* scale_anim = new QVariantAnimation;
        scale_anim->setDuration(500);
        scale_anim->setEasingCurve(QEasingCurve::OutSine);
        scale_anim->setStartValue(0.0);
        scale_anim->setEndValue(1.0);
        connect(scale_anim, &QVariantAnimation::valueChanged, this, [=](const QVariant &value) {
            time_text->setScale(value.toDouble());
        });

        auto* opacity_anim = new QVariantAnimation;
        opacity_anim->setDuration(500);
        opacity_anim->setEasingCurve(QEasingCurve::OutSine);
        opacity_anim->setStartValue(1.0);
        opacity_anim->setEndValue(0.0);
        connect(opacity_anim, &QVariantAnimation::valueChanged, this, [=](const QVariant &value) {
            time_text->setOpacity(value.toDouble());
        });

        auto* anim_group = new QParallelAnimationGroup(this);
        anim_group->addAnimation(opacity_anim);
        anim_group->addAnimation(scale_anim);
        anim_group->start(QAbstractAnimation::DeleteWhenStopped);

        timer->setInterval(1000);
    });
}

void Game::lose_screen()
{
    lose_score_text->show();
    lose_rect->show();

    auto* blur_anim = new QVariantAnimation;
    blur_anim->setDuration(700);
    blur_anim->setEasingCurve(QEasingCurve::InQuart);
    blur_anim->setStartValue(0.0);
    blur_anim->setEndValue(1);
    connect(blur_anim, &QVariantAnimation::valueChanged, this, [=](const QVariant& value) {
        blur_pix->setOpacity(value.toDouble());
    });

    auto* opacity_anim = new QVariantAnimation;
    opacity_anim->setDuration(700);
    opacity_anim->setEasingCurve(QEasingCurve::OutQuart);
    opacity_anim->setStartValue(0.0);
    opacity_anim->setEndValue(0.6);
    connect(opacity_anim, &QVariantAnimation::valueChanged, this, [=](const QVariant& value) {
        lose_rect->setOpacity(value.toDouble());
    });

    auto* anim_group = new QParallelAnimationGroup(this);
    anim_group->addAnimation(opacity_anim);
    anim_group->addAnimation(blur_anim);
    anim_group->start(QAbstractAnimation::DeleteWhenStopped);
}

