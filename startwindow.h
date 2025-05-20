#pragma once

#include <QWidget>

class StartWindow : public QWidget
{
    Q_OBJECT
public:
    StartWindow(QSize size, QWidget* prnt = nullptr, QWidget* main = nullptr);

signals:
    void start_pressed();

protected:
    virtual void paintEvent(QPaintEvent*) override;
};

