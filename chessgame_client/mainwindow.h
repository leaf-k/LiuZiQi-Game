#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define BLACK 1
#define WHITE 2

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    char bufferReceive[444];                   //接收服务端信息
    int bnum=0,wnum=0;                         //已下棋子数
    int Netflag=-1;                            //网络连接状态
    int player=BLACK;                          //执棋颜色

    QLabel *netlabel;
    QLabel *playerlabel;

private slots:
    void on_pushButton_clicked();               //连接按钮
    void on_pushButton_2_clicked();             //重新开始按钮

    void on_radioButtonb_clicked();             //选择下棋先后手按钮
    void on_radioButtonw_clicked();

    void receivemessage();                      //接收信息槽函数

    void showNettimelimit();                    //显示倒计时



private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);         //绘画事件
    void mouseReleaseEvent(QMouseEvent *event);  //鼠标释放事件
    void renew();                                //更新棋盘
    void nettimer();                             //启动倒计时

    QTimer *Nettimer;
    int TIME=15;

    QTcpSocket *socket;

};
#endif // MAINWINDOW_H
