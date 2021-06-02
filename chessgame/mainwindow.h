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
    void showTimelimit1();                         //倒计时槽函数
    void showTimelimit2();
    void showNettimelimit();
    void triggeredNormal();
    void triggeredAI();
    void triggeredai();
    void triggeredNet();

private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);           //绘图事件
    void mouseReleaseEvent(QMouseEvent *event);    //鼠标点击释放事件
    void contextMenuEvent(QContextMenuEvent *event);
    void renew();                                  //更新棋盘
    void timer1();                                 //启动倒计时
    void timer2();
    void nettimer();

    QLabel *netlable;
    QLabel *lable;
    QMenu *menu1;


    QTimer *Time1,*Time2,*Nettimer;
    QTimer *timer;
    int TIME=15;

    gamemode mode;                                  //游戏模式
    checkerboard board;
    judgement judger;

    bool blackqizi;                                 //当前执子颜色
    int aiflag=-1;


    QTcpServer *gameserver;                         //服务端、套接字
    QTcpSocket *socket;
    bool NetFlag=false;

    int netplayer=WHITE;                            //网络对战先后手顺序标记


private slots:
    void AIplacenode();                             //AI下棋
    void receivemessage();                          //接收客户端信息

    void on_pushButton_ai_clicked();
};
#endif // MAINWINDOW_H
