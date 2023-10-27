#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QWidget>
#include <QChartView>
#include <QLabel>
#include <QMenu>

class ChartView : public QChartView{
    Q_OBJECT
public:
    ChartView(QWidget* parent = nullptr) :  QChartView(parent){
        //setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        m_label = new QLabel(this);
        QFont font;
        font.setPointSize(10);
        m_label->setFont(font);
        m_label->setText("0 , 0");

        int labelX = width()/2-100;
        int labelY = 12;
        m_label->setFixedWidth(250);
        m_label->move(labelX, labelY);
        m_label->setAlignment(Qt::AlignCenter);

        m_label->setStyleSheet("background-color: transparent; color:white");

        rectangle = new QGraphicsRectItem(rect_);
        QColor colorRec(Qt::gray);
        colorRec.setAlphaF(0.5);
        rectangle->setBrush(QBrush(colorRec));
        rectangle->setPen(QPen(colorRec));

        chart()->scene()->addItem(rectangle);


        contextMenu = new QMenu(this);
        clearRectAction = new QAction("Clear Rectangle", nullptr);
        setCenter = new QAction("Set Center", nullptr);
        contextMenu->addAction(clearRectAction);
        contextMenu->addSeparator();
        contextMenu->addAction(setCenter);
        contextMenu->addSeparator();

        connect(clearRectAction, &QAction::triggered, this, [=](bool x){
            rectangle->setVisible(false);
            chart()->scene()->removeItem(rectangle);
            rect_ = QRectF(QPointF(0,0), QSizeF(0,0));
            rectangle = new QGraphicsRectItem(rect_);
            chart()->scene()->addItem(rectangle);
            rectangle->setVisible(false);
            emit sigStopPayeshThuraya();
        });

        connect(setCenter, &QAction::triggered, this, [=](bool x){
            //contextMenu.g
            emit setCenterFromChartView((endValue.x()+ startValue.x())/2);
            clearRectAction->trigger();
        });


    }
    double minRangeX=0;
    double maxRangeX=1;
    double minRangeY=0;
    double maxRangeY=1;
    bool isMonitoring = false;

public slots:
    void clearRectangleFromMain(){
        clearRectAction->trigger();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QChartView::resizeEvent(event);
        int labelX = this->width()/2-100;
        int labelY = 12;
        m_label->move(labelX, labelY);

        if(!rectangle->isVisible())
            return;

        qreal width_rect = chart()->mapToPosition(endValue).x() - chart()->mapToPosition(startValue).x();

        if(width_rect>=0)
            rectangle->setRect(QRectF( QPointF(chart()->mapToPosition(startValue).x(),  10), QSizeF(width_rect, this->height()-20)));
        else
            rectangle->setRect(QRectF( QPointF(chart()->mapToPosition(endValue).x(),  10), QSizeF(-width_rect, this->height()-20)));

    }
    void mouseMoveEvent(QMouseEvent* event) override
    {
        QPoint pos = event->pos();
        QPointF chartPos = this->chart()->mapToValue(pos);

        updateLabelPosition(chartPos.x() ,  chartPos.y());

        if(isPressed && chartPos.x() <= maxRangeX && chartPos.x() >= minRangeX){
            endValue = this->chart()->mapToValue(pos);

            QPointF releasPoint = event->pos();
            qreal width_rect = releasPoint.x() - startPoint.x();

            chart()->scene()->removeItem(rectangle);
            rectangle = new QGraphicsRectItem(rect_);
            QColor colorRec(Qt::gray);
            colorRec.setAlphaF(0.5);
            rectangle->setBrush(QBrush(colorRec));
            rectangle->setPen(QPen(colorRec));
            chart()->scene()->addItem(rectangle);
            if(width_rect>=0)
                rectangle->setRect(QRectF( QPointF(rectangle->rect().right(),  10), QSizeF(width_rect, this->height()-20)));
            else
                rectangle->setRect(QRectF( QPointF(chart()->mapToPosition(endValue).x(),  10), QSizeF(-width_rect, this->height()-20)));
        }


    }
    void mousePressEvent(QMouseEvent* event) override{

        if(event->button() == Qt::LeftButton){
            clearRectAction->trigger();
            startPoint = event->pos();
            startValue = chart()->mapToValue(startPoint);
            rect_ = QRectF(startPoint, QSizeF(0,0));
            if(startValue.x() >= minRangeX && startValue.x() <= maxRangeX && isMonitoring){
                rectangle->setVisible(true);
                isPressed = true;
            }
        }


    }
    void mouseReleaseEvent(QMouseEvent* event)override{
        isPressed = false;
        if(event->button() == Qt::LeftButton){
            endPoint = event->pos();
        }

        if(std::abs( endPoint.x()  - startPoint.x()) < 3){
            rectangle->setVisible(false);
            clearRectAction->trigger();
        }
    }
    void updateLabelPosition(double x, double y){
        if(x>maxRangeX) x= maxRangeX;
        if(x<minRangeX) x=minRangeX;
        if(y>maxRangeY) y= maxRangeY;
        if(y<minRangeY) y=minRangeY;
        m_label->setText("Freq(MHz): "+QString::number(x,'f',3) + " , Power(dB): " + QString::number(y,'f',3));
    }
    void contextMenuEvent(QContextMenuEvent* event) override
    {

        contextMenu->exec(event->globalPos());
    }

private:
    bool isPressed=false;
    QLabel* m_label;
    QPointF startPoint;
    QPointF startValue;
    QPointF endPoint;
    QPointF endValue;
    QRectF rect_;
    QGraphicsRectItem* rectangle;
    QMenu* contextMenu;
    QAction* clearRectAction;

    QAction* setCenter;

signals:
    void sigStartPayeshMonitoring(double ileft, double iright);
    void sigStopPayeshThuraya();
    void setCenterFromChartView(double center);
};

#endif // CHARTVIEW_H
