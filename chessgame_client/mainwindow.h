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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    char bufferReceive[444];
    int bnum=0,wnum=0;
    int Netflag=-1;
    QLabel *netlabel;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void receivemessage();

    void showNettimelimit();


private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void renew();
    void nettimer();

    QTimer *Nettimer;
    int TIME=15;

    QTcpSocket *socket;

};
#endif // MAINWINDOW_H
