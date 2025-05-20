#include "custombutton.h"

#include <QPainter>

CustomButton::CustomButton(const QString &text, QSize size, QWidget *prnt)
    : QWidget(prnt), btn_size(size), btn_text(text)
{
    setFixedSize(size.width() + 8, size.height() + 8);

    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    start_rect = QRect(rect().center().x() - btn_size.width() / 2, rect().center().y() - btn_size.height() / 2,
                       btn_size.width(), btn_size.height());
    current_rect = start_rect;

    start_color = QColor(0,102,204);
    current_color = start_color;
}

void CustomButton::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing);

    paint.setPen(Qt::NoPen);
    paint.setBrush(current_color);
    paint.drawRoundedRect(current_rect, 15,15);

    QFont font("Intro", 22);
    font.setBold(true);
    paint.setPen(QPen(Qt::white));
    paint.setFont(font);

    paint.drawText(rect(), Qt::AlignCenter, btn_text);
}

void CustomButton::mousePressEvent(QMouseEvent*)
{
    emit clicked();
    if(switch_mode) {
        ++switcher;

        if(switcher == easy)
            btn_text = "EASY";
        else if(switcher == medium)
            btn_text = "MEDIUM";
        else if(switcher == hard) {
            btn_text = "HARD";
            switcher = 0;
        }
        emit level_changed();
        update();
    }
}

void CustomButton::enterEvent(QEnterEvent*)
{
    animate_rect(rect());
    animate_color(QColor(8,132,255));
}

void CustomButton::leaveEvent(QEvent*)
{
    animate_rect(start_rect);
    animate_color(start_color);
}

void CustomButton::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    animate_rect(start_rect);
    animate_color(start_color);
}

void CustomButton::animate_rect(QRect target)
{
    if(!rect_anim) {
        rect_anim = new QPropertyAnimation(this, "btn_rect");
        rect_anim->setDuration(100);
    }
    rect_anim->stop();
    rect_anim->setEndValue(target);
    rect_anim->start();
}

void CustomButton::animate_color(QColor target)
{
    if(!color_anim) {
        color_anim = new QPropertyAnimation(this, "btn_color");
        color_anim->setDuration(100);
    }
    color_anim->stop();
    color_anim->setEndValue(target);
    color_anim->start();
}
