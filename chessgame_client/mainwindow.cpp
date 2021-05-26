#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_2->setStyleSheet("background-color: rgb(255, 255, 255,150);");

    for(int i=0;i<442;i++)
        bufferReceive[i]='0';

    netlabel=new QLabel;

}


MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::paintEvent(QPaintEvent *event)
{
    const int size=20;
    const int width=40;
    const int x=40,y=70;

    QPainter paint(this);
    paint.setPen(QPen(Qt::black,1,Qt::SolidLine));

    for(int i=0;i<=size;i++)
    {
        paint.drawLine(x,y+width*i,x+size*width,y+width*i);
    }
    for(int i=0;i<=size;i++)
    {
        paint.drawLine(x+width*i,y,x+width*i,y+size*width);
    }

    for(int i=0;i<441;i++)
    {
        int t=i/21,s=i%21;
            if(bufferReceive[i]=='1')
            {
                paint.setBrush(QBrush(Qt::black,Qt::SolidPattern));
                paint.drawEllipse(x+width*s-15,y+width*t-15,32,32);
            }
            if(bufferReceive[i]=='2')
            {
                paint.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                paint.drawEllipse(x+width*s-15,y+width*t-15,32,32);
            }
    }
    paint.end();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int mousex,mousey;
    mousex=event->x();
    mousey=event->y();

    if(mousex>30&&mousex<850&&mousey>60&&mousey<880)
    {

        int i,j;
        j=(mousex-20)/40;
        i=(mousey-50)/40;

        if(bnum==0)
        {
            for(int i=0;i<442;i++)
                bufferReceive[i]='0';
        }


        if(Netflag==-1)
            QMessageBox::warning(NULL,"提示：","请连接服务端！");
        else if(bnum>wnum)
            QMessageBox::warning(NULL,"提示：","请等待服务端落子！");
        else
        {
            if(bufferReceive[i*21+j]!='1'&&bufferReceive[i*21+j]!='2')
            {
                qDebug()<<bnum;
                if(bnum!=0)
                {
                    Nettimer->stop();
                    ui->nettime->setText("15");
                }

                bufferReceive[i*21+j]='1';
                bnum++;
                update();

                char bufferSend[445];    //bufferSend[442]黑子x坐标 [443]黑子y坐标
                bufferSend[442]=i+'0';
                bufferSend[443]=j+'0';

                for(int i=0;i<442;i++)
                    bufferSend[i]=bufferReceive[i];
                socket->write(bufferSend);

            }

        }

    }
}


void MainWindow::on_pushButton_clicked()
{
    socket=new QTcpSocket(this);
    socket->connectToHost(ui->lineEdit->text(),ui->lineEdit_2->text().toInt());

    connect(socket,&QTcpSocket::connected,this,[=](){
        netlabel->setText("  连接成功！ ");
        ui->statusbar->addWidget(netlabel);
        Netflag=6;
    });

    connect(socket,SIGNAL(readyRead()),this,SLOT(receivemessage()));


}

void MainWindow::receivemessage()
{
      socket->read(bufferReceive,socket->bytesAvailable());
      if(bufferReceive[441]=='b')
      {
          QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
          renew();
      }
      else
      {
      update();

      if(bufferReceive[441]=='w')
      {
          QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
          renew();
      }
      else if(bufferReceive[441]=='j')
      {
          QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
          renew();
      }
      else
      {
          bnum=0;
          wnum=0;
          for(int i=0;i<441;i++)
          {
              if(bufferReceive[i]=='1')
                  bnum++;
              if(bufferReceive[i]=='2')
                  wnum++;
          }
          nettimer();
      }

      }
}


void MainWindow::renew()
{
    TIME=15;
    for(int i=0;i<442;i++)
        bufferReceive[i]='0';
    bnum=0;
    wnum=0;
    update();
}



void MainWindow::on_pushButton_2_clicked()
{
    if(bnum==wnum)
    {
        Nettimer->stop();
        ui->nettime->setText("15");
    }

    char buffersend[443];
    buffersend[441]='r';
    for(int i=0;i<441;i++)
        buffersend[i]='0';
    socket->write(buffersend);
    renew();
}


void MainWindow::nettimer()     //网络倒计时
{
    TIME=15;
    Nettimer = new QTimer(this);
    connect(Nettimer, SIGNAL(timeout()), this, SLOT(showNettimelimit()));
    Nettimer->setInterval( 1000 );
    Nettimer->start();
}


void MainWindow::showNettimelimit()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->nettime->setText(num);
    }
    else
    {
        char buffertime[443];
        for(int i=0;i<441;i++)
            buffertime[i]='0';
        buffertime[441]='w';
        socket->write(buffertime);
        QMessageBox::warning(NULL,"超时：","白棋胜！");
        renew();
        Nettimer->stop();
        ui->nettime->setText("15");
        TIME=15;
    }
}
