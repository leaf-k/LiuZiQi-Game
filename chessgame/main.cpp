#include "mainwindow.h"

#include <QApplication>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.setWindowIcon(QIcon(":/icon1.jpg"));              //设置窗口图标

    w.autoFillBackground();
    QPalette pal;
    QImage image=QImage(":/background3.jpg");           //设置背景图片
    image=w.ConvertImageToTransparent(image);
    pal.setBrush(QPalette::Background,QBrush(image));
    w.setPalette(pal);

    return a.exec();
}
