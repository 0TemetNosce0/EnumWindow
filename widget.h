#ifndef WIDGET_H
#define WIDGET_H

#include <QKeyEvent>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void keyPressEvent(QKeyEvent *ev);
    void paintEvent(QPaintEvent *e);
private:
    Ui::Widget *ui;
    QList<QRect> m_windowInfoList;//窗口信息
};

#endif // WIDGET_H
