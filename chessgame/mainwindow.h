#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "checkerboard.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage ConvertImageToTransparent(QImage);



public slots:
    void showTimelimit1();
    void showTimelimit2();


private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void renew();
    void timer1();
    void timer2();

    QTimer *Time1,*Time2;
    int TIME=10;

    gamemode mode;    
    checkerboard board;

    bool blackqizi;   
    judgement judger;

    QLabel *lable2= new QLabel;


    QTcpServer *gameserver;
    QTcpSocket *socket;
    bool NetFlag=false;



private slots:
    void AIplacenode();
    void receivemessage();

};
#endif // MAINWINDOW_H
