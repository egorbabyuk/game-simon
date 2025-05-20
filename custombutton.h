#pragma once

#include <QWidget>
#include <QPropertyAnimation>

class CustomButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QRect btn_rect READ get_btn_rect WRITE set_btn_rect)
    Q_PROPERTY(QColor btn_color READ get_btn_color WRITE set_btn_color)
public:
    CustomButton(const QString& text, QSize size, QWidget* prnt = nullptr);

    void set_level_switch(bool value) { switch_mode = value; }
    int get_level() { return switcher; }

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void enterEvent(QEnterEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void hideEvent(QHideEvent*) override;

signals:
    void clicked();
    void level_changed();

private:
    void animate_rect(QRect target);
    void animate_color(QColor target);

    QRect get_btn_rect() const { return current_rect; }
    void set_btn_rect(QRect temp) { current_rect = temp; update(); }

    QColor get_btn_color() const { return current_color; }
    void set_btn_color(QColor temp) { current_color = temp; update(); }

    QRect start_rect;
    QRect current_rect;

    QColor start_color;
    QColor current_color;

    QSize btn_size;
    QString btn_text;

    QPropertyAnimation* rect_anim = nullptr;
    QPropertyAnimation* color_anim = nullptr;

    enum {easy = 1, medium, hard};

    bool switch_mode = false;
    int switcher{};
};

