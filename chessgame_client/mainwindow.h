#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    char bufferReceive[443];
    int I=-1,bnum=0,wnum=0;
    bool Netflag=false;
    QLabel *netlabel;

private slots:
    void on_pushButton_clicked();

    void receivemessage();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void renew();


    QTcpSocket *socket;

};
#endif // MAINWINDOW_H