#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H
#include <QPainter>
#include <QWidget>

class canvasWidget: public QWidget
{
    Q_OBJECT
public:

    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event) override;
signals:
    clickedSignal( );
};

#endif // CANVASWIDGET_H
