#include "canvaswidget.h"
#include <QMouseEvent>
#include <QLineEdit>
extern bool mode;
extern int *vibro_rad;
extern int vibro_rad_stat;
extern int** vibro_n;
extern int* vibro_x;
extern int* vibro_y;
extern QPoint mouse_p;
extern int Nx;
extern int Ny;
extern int checked_n;
extern int *vibro_state;
extern QLineEdit* PWM_le;

canvasWidget::canvasWidget()
{

}



void canvasWidget::paintEvent(QPaintEvent *event)
{
    QPen pen;


    //    pen.setStyle();
    QPainter* painter=new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing, true);

    if(mode==0)
    {
        QRect r=QRect(mouse_p,mouse_p+QPoint(100,100));
        for(int i=0;i<Ny;i++)
            for(int j=0;j<Nx;j++)
            {
                if(checked_n==vibro_n[i][j])
                {
                    pen.setWidth(6);
                    pen.setColor(QColor(0,150,0));
                    vibro_rad[checked_n]=vibro_rad_stat*1.15;
                }
                else
                {
                    //                pen.setStyle(Qt::SolidLine);
                    pen.setColor(QColor(0,0,0));
                    pen.setWidth(2);
                    vibro_rad[checked_n]=vibro_rad_stat;
                }
                painter->setPen(pen);

                QPainterPath path;
                QRadialGradient gradient=QRadialGradient(QPointF(vibro_x[j],vibro_y[i]),20,
                                                         QPointF(vibro_x[j],vibro_y[i])+QPointF(50,50));
                if(vibro_state[vibro_n[i][j]]!=0)
                {
                    gradient.setColorAt(1.0,QColor(170,170,120));
                    gradient.setColorAt(0.0,QColor(250,250,200));
                }
                else
                {
                    gradient.setColorAt(1.0,QColor(100,100,100));
                    gradient.setColorAt(0.0,QColor(200,200,200));
                }

                path.addEllipse ( QPointF(vibro_x[j],vibro_y[i]), vibro_rad[vibro_n[i][j]], vibro_rad[vibro_n[i][j]]);
                painter->fillPath(path,gradient);
                painter->drawPath(path);
            }

    }
    else
    {

    }
    delete painter;
}



void canvasWidget::mousePressEvent(QMouseEvent *event)
{
    mouse_p=event->pos();

    for(int i=0;i<Ny;i++)
        for(int j=0;j<Nx;j++)
        {
            float r2=vibro_rad[vibro_n[i][j]]*vibro_rad[vibro_n[i][j]];
            /////!!!!!!!!!!!!!!
            QPoint vibro_p=QPoint(vibro_x[j],vibro_y[i]);
            if(QPoint::dotProduct(vibro_p-mouse_p,vibro_p-mouse_p)<r2)
            {
                checked_n=vibro_n[i][j];
                vibro_state[checked_n]=!vibro_state[checked_n];
            }
        }
    update();
    emit clickedSignal();
}
