#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QTimer>
#include <stdlib.h>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    ui->pushButton->setStyleSheet("background-color: rgb(255, 255, 255,150);");
    ui->pushButton_2->setStyleSheet("background-color: rgb(255, 255, 255,150);");

    blackqizi=true;
    mode=Normal;

    QLabel *netlable= new QLabel;
    QLabel *lable= new QLabel("  当前游戏模式：双人对战  ");
    ui->statusbar->setFixedHeight(40);
    ui->statusbar->addWidget(lable);

    connect(ui->actionnormal,&QAction::triggered,this,[=]()
    {
        mode=Normal;
        ui->pushButton_2->show();
        ui->pushButton->show();
        ui->label->show();
        ui->label_3->show();
        ui->label2->show();
        ui->label_2->show();
        lable->setText("  当前游戏模式：双人对战  ");
        ui->statusbar->addWidget(lable);
        netlable->setText(" ");
        ui->statusbar->addPermanentWidget(netlable);


    });

    connect(ui->actionAI,&QAction::triggered,this,[=]()
    {
        mode=AI;
        ui->pushButton_2->hide();
        ui->pushButton->show();
        ui->label->show();
        ui->label_3->show();
        ui->label2->show();
        ui->label_2->show();
        lable->setText("  当前游戏模式：人机对战  ");
        ui->statusbar->addWidget(lable);
        netlable->setText(" ");
        ui->statusbar->addPermanentWidget(netlable);
    });

    connect(ui->actionnetgame,&QAction::triggered,this,[=]()
    {
        mode=Net;
        ui->pushButton_2->hide();
        ui->pushButton->hide();
        ui->label->hide();
        ui->label_3->hide();
        ui->label2->hide();
        ui->label_2->hide();
        lable->setText("  当前游戏模式：网络对战  ");
        ui->statusbar->addWidget(lable);

        gameserver=new QTcpServer;

        gameserver->listen(QHostAddress::Any,6666);

        connect(gameserver,&QTcpServer::newConnection,this,[=](){
                netlable->setText("客户端连接成功！");
                ui->statusbar->addPermanentWidget(netlable);
                NetFlag=true;

                socket=gameserver->nextPendingConnection();
                connect(socket,SIGNAL(readyRead()),this,SLOT(receivemessage()));

        });

        connect(gameserver,&QTcpServer::destroyed,this,[=](){
                netlable->setText("客户端未连接！");
                ui->statusbar->addPermanentWidget(netlable);
                NetFlag=false;
                renew();

        });
    });


    connect(ui->pushButton,&QPushButton::clicked,this,[=]()
    {
        if(blackqizi)
        {
            Time2->stop();
            ui->label2->setText("15");
        }
        else
        {
            Time1->stop();
            ui->label->setText("15");
        }
        renew();
    });

    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        if(mode==Normal)
        {
            if(blackqizi)
            {
                if(board.getwnum()!=0)
                {
                    board.undo(WHITE);
                    blackqizi=false;
                    Time2->stop();
                    ui->label2->setText("15");
                    timer1();
                }
                else
                    QMessageBox::warning(NULL,"提示：","无棋可悔");
            }
            else
            {
                if(board.getwnum()!=0)
                {
                    board.undo(BLACK);
                    blackqizi=true;
                    Time1->stop();
                    ui->label->setText("15");
                    timer2();
                }
                else
                    QMessageBox::warning(NULL,"提示：","无棋可悔");
            }
        }
        update();
    });


}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::receivemessage()
{
    char bufferReceive[445];
    socket->read(bufferReceive,socket->bytesAvailable());
    if(bufferReceive[441]=='r')
    {
        renew();
    }

    else
    {
        for(int i=0;i<441;i++)
        {
            if(bufferReceive[i]=='1')
            {
                board.placeqizi(i/21,i%21,BLACK);
            }
            if(bufferReceive[i]=='2')
            {
                board.placeqizi(i/21,i%21,WHITE);
            }
        }
        update();

        if(judger.judgewin(board,bufferReceive[442]-'0',bufferReceive[443]-'0'))
        {

            bufferReceive[441]='b';
            socket->write(bufferReceive);
            QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
            renew();
        }
        else if(judger.FLAG)
        {
            bufferReceive[441]='b';
            socket->write(bufferReceive);
            QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
            renew();
        }
    }


}




void MainWindow::paintEvent(QPaintEvent *event){
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

    for(int i=0;i<=size;i++)
    {
        for(int j=0;j<=size;j++)
        {
            if(board.showqizi(i,j)==BLACK)
            {
                paint.setBrush(QBrush(Qt::black,Qt::SolidPattern));
                paint.drawEllipse(x+width*j-15,y+width*i-15,32,32);
            }
            if(board.showqizi(i,j)==WHITE)
            {
                paint.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                paint.drawEllipse(x+width*j-15,y+width*i-15,32,32);
            }
        }
    }
    paint.end();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){

    int mousex,mousey;
    mousex=event->x();
    mousey=event->y();

    if(mousex>30&&mousex<850&&mousey>60&&mousey<880)
    {
        int i,j,p;
        j=(mousex-20)/40;
        i=(mousey-50)/40;


        //双人对战
        if(mode==Normal)
        {
        if(board.showqizi(i,j)==-1)
        {
            TIME=15;

            if(blackqizi)
            {
                p=BLACK;
                blackqizi=false;
            }
            else
            {
                p=WHITE;
                blackqizi=true;
            }
            board.placeqizi(i,j,p);
            if(p==BLACK)
            {
                if(board.getwnum()!=0)
                {
                    Time2->stop();
                    ui->label2->setText("15");
                }
                timer1();
            }
            else
            {
                Time1->stop();
                ui->label->setText("15");
                timer2();
            }

            if(judger.judgewin(board,i,j))
            {
                if(p==BLACK)
                {                   
                    Time1->stop();
                    Time2->stop();
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                    renew();
                }
                if(p==WHITE)
                {                    
                    Time2->stop();
                    Time1->stop();
                    QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                    renew();
                }
            }
            update();
        }
        }


        //人机对战
        if(mode==AI)
        {
            if(blackqizi&&board.showqizi(i,j)==-1)
            {
                board.placeqizi(i,j,BLACK);
                if(board.getwnum()!=0)
                {
                    Time2->stop();
                    ui->label2->setText("15");
                }
                blackqizi=false;
                if(judger.judgewin(board,i,j))
                {
                    Time2->stop();
                    Time1->stop();
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                    renew();
                }
                else
                {
                   QTimer::singleShot(600,this,SLOT(AIplacenode()));
                   TIME=15;
                   timer2();
                }
                update();
            }
        }


        if(judger.FLAG)
        {
            Time2->stop();
            Time1->stop();
            QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
            renew();
        }



        //网络对战
        if(mode==Net)
        {
            if(!NetFlag)
            {
                QMessageBox::warning(NULL,"提示：","请等待客户端连接！");
            }
            else
            {
                if(board.getbnum()<=board.getwnum())
                    QMessageBox::warning(NULL,"提示：","请等待客户端落子！");
                else if(board.showqizi(i,j)==-1)
                {
                    char buffer[443];    //buffer[441]=w/b 白棋/黑棋赢标志

                    board.placeqizi(i,j,WHITE);
                    update();
                    if(judger.judgewin(board,i,j))
                    {
                        buffer[441]='w';
                        int k=0;

                        for(int i=0;i<21;i++)
                        {
                            for(int j=0;j<21;j++)
                            {
                                buffer[k]=board.showqizi(i,j)+'0';
                                k++;
                            }
                        }
                        socket->write(buffer);

                        QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                        renew();
                    }
                     else
                    {
                        buffer[441]='n';
                        int k=0;

                        for(int i=0;i<21;i++)
                        {
                            for(int j=0;j<21;j++)
                            {
                                buffer[k]=board.showqizi(i,j)+'0';
                                k++;
                            }
                        }

                        socket->write(buffer);
                    }

                }
            }

        }

        if(judger.judgedraw(board))
        {
            Time2->stop();
            Time1->stop();
            QMessageBox::warning(NULL,"比赛结果：","和棋");
            renew();
        }
    }
}



void MainWindow::renew(){
    TIME=15;

    if(board.getbnum()!=0)
    {
        while(board.getbnum())
        {
            board.undo(BLACK);
        }
    }

    if(board.getwnum()!=0)
    {
        while(board.getwnum())
        {
            board.undo(WHITE);
        }
    }

    blackqizi=true;
    for(int i=0;i<21;i++)
        for(int j=0;j<21;j++)
            board.value[i][j]=0;

    update();
}


void MainWindow::AIplacenode(){
        judger.jugdevalue(&board);
        int max=0,k=1;
        Node maxvalue[400];

        for(int i=0;i<21;i++)
        {
            for(int j=0;j<21;j++)
            {
                if(board.value[i][j]>max)
                    max=board.value[i][j];
            }
        }

        for(int i=0;i<21;i++)
        {
            for(int j=0;j<21;j++)
            {
                if(board.value[i][j]==max)
                {
                    maxvalue[k].x=i;
                    maxvalue[k].y=j;
                    k++;
                }
            }
        }

        srand(time(0));
        int n=(rand()%(k-1))+1;
        board.placeqizi(maxvalue[n].x,maxvalue[n].y,WHITE);
        blackqizi=true;

        if(judger.judgewin(board,maxvalue[n].x,maxvalue[n].y))
        {
            Time2->stop();
            Time1->stop();
            QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
            renew();
        }
        update();
}


QImage MainWindow::ConvertImageToTransparent(QImage image)
{
        image = image.convertToFormat(QImage::Format_ARGB32);
        union myrgb
        {
            uint rgba;
            uchar rgba_bits[4];
        };
        myrgb* mybits =(myrgb*) image.bits();
        int len = image.width()*image.height();
        while(len --> 0)
        {
            mybits->rgba_bits[3] = (mybits->rgba== 0xFF000000)?100:255;
            mybits++;
        }
        return image;
}


void MainWindow::timer1()     //白棋倒计时
{
    TIME=15;
    Time1 = new QTimer(this);
    connect(Time1, SIGNAL(timeout()), this, SLOT(showTimelimit1()));
    Time1->setInterval( 1000 );
    Time1->start();
}


void MainWindow::showTimelimit1()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->label->setText(num);
    }
    else
    {
        QMessageBox::warning(NULL,"超时：","黑棋胜！");
        renew();
        Time1->stop();
        ui->label->setText("15");
        TIME=15;
    }
}


void MainWindow::timer2()     //黑棋倒计时
{
    TIME=15;
    Time2 = new QTimer(this);
    connect(Time2, SIGNAL(timeout()), this, SLOT(showTimelimit2()));
    Time2->setInterval( 1000 );
    Time2->start();
}


void MainWindow::showTimelimit2()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->label2->setText(num);
    }
    else
    {
        QMessageBox::warning(NULL,"超时：","白棋胜！");
        renew();
        Time2->stop();
        ui->label2->setText("15");
        TIME=15;
    }
}
