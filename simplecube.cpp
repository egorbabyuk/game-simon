#include "simplecube.h"

#include <QPainter>

SimpleCube::SimpleCube(const QPainterPath &path)
    : QGraphicsPathItem(path)
{
    setAcceptHoverEvents(true);

    start_color = QColor(0,102,204);
    color = start_color;
    setBrush(QBrush(color));
    setPen(Qt::NoPen);

    setTransformOriginPoint(boundingRect().center());

    connect(this, &SimpleCube::enabledChanged, this, [=](bool isEnable) {
        if(!isEnable) {
            animateScale(1);
            setBrush(QBrush(QColor(0,102,204)));
        }
    });
}

void SimpleCube::setCubeEnabled(bool enabled) {
    QGraphicsPathItem::setEnabled(enabled);
    emit enabledChanged(enabled);
}

void SimpleCube::target_backlight()
{
    animateScale(1.1);
    animateColor(QColor(0,255,255));

    blick_timer.start(400);
    blick_timer.setSingleShot(true);

    connect(&blick_timer, &QTimer::timeout, this, [this]() {
        animateScale(1);
        animateColor(start_color);
    });
}

void SimpleCube::wrong_target()
{
    animateColor(QColor(255,0,0,155));
    connect(anim_color, &QPropertyAnimation::finished, this, [=]() { animateColor(start_color); });
}

void SimpleCube::correct_target()
{
    animateColor(QColor(0,255,0,155));
    connect(anim_color, &QPropertyAnimation::finished, this, [=]() { animateColor(start_color); });
}

void SimpleCube::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit clicked(this);
    QGraphicsPathItem::mousePressEvent(event);
}


void SimpleCube::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    animateScale(1.2);
    setBrush(QBrush(QColor(51,153,255)));
}

void SimpleCube::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    animateScale(1);
    setBrush(QBrush(QColor(0,102,204)));
}

void SimpleCube::animateScale(qreal target) {
    if(!anim_scale) {
        anim_scale = new QPropertyAnimation(this, "scale");
        anim_scale->setDuration(300);
        anim_scale->setEasingCurve(QEasingCurve::OutBack);
    }
    anim_scale->stop();
    anim_scale->setEndValue(target);
    anim_scale->start();
}

void SimpleCube::animateColor(QColor target)
{
    if(!anim_color) {
        anim_color = new QPropertyAnimation(this, "color");
        anim_color->setDuration(500);
    }
    anim_color->stop();
    anim_color->setEndValue(target);
    anim_color->start();
}
