#include "widget.h"
#include "ui_widget.h"
#include "GetWindowHelpers.h"
#include <QDesktopWidget>
#include <QPainter>
Widget::Widget(QWidget *parent) :
                                  QWidget(parent),
                                  ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() |Qt::FramelessWindowHint |Qt::Widget |Qt::WindowStaysOnTopHint/*|Qt::Tool*/);
    setAttribute(Qt::WA_TranslucentBackground, true);//透明，如果标题栏隐藏没有影藏，则是透明是黑色的
    this->setMouseTracking(true);
    setGeometry(QRect(QApplication::desktop()->pos(),QApplication::desktop()->size()));

    GetWindowHelpers getWindowHelpers;
    //    QDesktopWidget * deskwidget = QApplication::desktop();

    getWindowHelpers.FillWindowList(m_windowInfoList,GetWindowHelpers::EXCLUDE_MINIMIZED);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
//    if(operationType == OPERATION_FIVE){
//        QPen pen;
//        pen.setColor(PEN_COLOR);
//        pen.setWidth(UIBaseData::ratioFit(2));
//        painter.setPen(pen);
//        painter.drawRect(QRect(startPoint,endPoint));
//    }else{
//        //画透明度为1的背景
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QColor(0,0,0,1));
//        painter.drawRect(QRect(QPoint(0,0),QApplication::desktop()->size()));

//        paintSlectArea(painter);
//    }
    QWidget::paintEvent(e);
}
void Widget::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Escape){
        close();
    }
}
