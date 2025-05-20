#pragma once

#include <QGraphicsPathItem>
#include <QPropertyAnimation>
#include <QWidget>
#include <QTimer>

class SimpleCube : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(QColor color READ get_color WRITE set_color)
public:
    SimpleCube(const QPainterPath& path);

    void setCubeEnabled(bool enabled);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;

public slots:
    void target_backlight();
    void wrong_target();
    void correct_target();

signals:
    void clicked(SimpleCube* self);
    void enabledChanged(bool enabled);

private:
    void animateScale(qreal target);
    void animateColor(QColor target);

    void set_color(QColor temp) { color = temp; setBrush(color); }
    QColor get_color() const { return color; }

    QColor color;
    QColor start_color;

    QPropertyAnimation* anim_scale = nullptr;
    QPropertyAnimation* anim_color = nullptr;

    QTimer blick_timer;
};

