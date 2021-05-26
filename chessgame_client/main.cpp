#include "mainwindow.h"

#include <QApplication>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.setWindowIcon(QIcon(":/icon1.jpg"));               //窗口图标

    w.autoFillBackground();
    QPalette pal;
    QImage image=QImage(":/background3.jpg");            //窗口背景图片
    pal.setBrush(QPalette::Background,QBrush(image));
    w.setPalette(pal);


    return a.exec();
}
