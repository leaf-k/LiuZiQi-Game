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

    setContextMenuPolicy(Qt::DefaultContextMenu);


    ui->pushButton->setStyleSheet("background-color: rgb(255, 255, 255,150);");     //设置按钮颜色
    ui->pushButton_2->setStyleSheet("background-color: rgb(255, 255, 255,150);");
    ui->pushButton_ai->setStyleSheet("background-color: rgb(255, 255, 255,150);");

    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->NetplayerLabel->hide();
    ui->label_5->hide();
    ui->pushButton_ai->hide();

    blackqizi=true;                                              //设置默认值
    mode=Normal;

    menu1 = new QMenu(this);
    QAction *normalmode=menu1->addAction("双人对战模式");
    QAction *AImode=menu1->addAction("人机对战模式");
    QAction *aimode=menu1->addAction("机机对战模式");
    QAction *netmode=menu1->addAction("网络对战模式");

    connect(normalmode,&QAction::triggered,this,&MainWindow::triggeredNormal);
    connect(AImode,&QAction::triggered,this,&MainWindow::triggeredAI);
    connect(aimode,&QAction::triggered,this,&MainWindow::triggeredai);
    connect(netmode,&QAction::triggered,this,&MainWindow::triggeredNet);


    netlable= new QLabel(this);
    lable= new QLabel("  当前游戏模式：双人对战  ");
    ui->statusbar->setFixedHeight(40);
    ui->statusbar->addWidget(lable);


    connect(ui->actionnormal,&QAction::triggered,this,&MainWindow::triggeredNormal);      //双人对战模式
    connect(ui->actionAI,&QAction::triggered,this,&MainWindow::triggeredAI);              //人机对战模式
    connect(ui->actionai,&QAction::triggered,this,&MainWindow::triggeredai);              //机机对战模式
    connect(ui->actionnetgame,&QAction::triggered,this,&MainWindow::triggeredNet);        //网络对战模式



    connect(ui->pushButton,&QPushButton::clicked,this,[=](){         //重新开始
        if(board.getbnum()!=0||board.getwnum()!=0)
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
        }

    });

    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){        //悔棋
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


void MainWindow::receivemessage()                         //网络对战接收客户端棋盘信息
{
    char bufferReceive[445];
    for(int i=0;i<442;i++)
        bufferReceive[i]='0';
    socket->read(bufferReceive,socket->bytesAvailable());

    if(bufferReceive[0]=='w')                              //bufferReceive[0] 接收客户端选择的先后手顺序
    {
        ui->NetplayerLabel->setText("执黑棋，先落子");
        netplayer=BLACK;
        bufferReceive[0]='0';

    }
    else if(bufferReceive[0]=='b')
    {
        ui->NetplayerLabel->setText("执白棋，后落子");
        netplayer=WHITE;
        bufferReceive[0]='0';

    }

    else
    {
        if(bufferReceive[441]=='r')                          //客户端要求重新开始
        {
            Nettimer->stop();
            ui->nettime->setText("15");
            renew();
        }

        else if(bufferReceive[441]=='c')                     //客户端落子超时
        {
            Nettimer->stop();
            ui->nettime->setText("15");
            if(netplayer==WHITE)
                QMessageBox::warning(NULL,"超时：","白棋胜！");
            else
                QMessageBox::warning(NULL,"超时：","黑棋胜！");
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
                if(netplayer==WHITE)
                {
                    bufferReceive[441]='b';                             //发送获胜信息
                    socket->write(bufferReceive);
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                }
                else
                {
                    bufferReceive[441]='w';
                    socket->write(bufferReceive);
                    QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                }
                renew();
            }

            else if(netplayer==WHITE&&judger.FLAG)
            {
                bufferReceive[441]='j';                                  //发送出现禁手信息
                socket->write(bufferReceive);
                QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
                renew();
            }
            else
            nettimer();
        }
    }


}




void MainWindow::paintEvent(QPaintEvent *event){
    const int size=20;
    const int width=40;         //每格的宽度
    const int x=40,y=70;        //棋盘上(0,0)点坐标为(40,70)


    //画棋盘
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


    //画棋子
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


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
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
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                    renew();
                }
                if(p==WHITE)
                {                    
                    Time2->stop();
                    QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                    renew();
                }
            }
            update();
        }

        if(judger.FLAG)
        {
            Time1->stop();
            QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
            renew();
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
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                    renew();
                }
                else if(judger.FLAG)
                {
                    QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
                    renew();
                }
                else
                {
                    TIME=15;
                    timer2();
                    QTimer::singleShot(600,this,SLOT(AIplacenode()));      //AI 停顿0.6s后下棋
                }
                update();
            }
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
                if(netplayer==WHITE&&board.getbnum()<=board.getwnum())
                    QMessageBox::warning(NULL,"提示：","请等待客户端落子！");
                else if(netplayer==BLACK&&board.getbnum()!=board.getwnum()&&board.getbnum()!=0)
                    QMessageBox::warning(NULL,"提示：","请等待客户端落子！");
                else if(board.showqizi(i,j)==-1)
                {
                    if(netplayer==BLACK&&board.getbnum()!=0)
                    {
                        Nettimer->stop();
                        ui->nettime->setText("15");
                    }
                    else if(netplayer==WHITE)
                    {
                        Nettimer->stop();
                        ui->nettime->setText("15");
                    }

                    char buffer[443];              //buffer[441]=w/b/j/c  白棋赢/黑棋赢/禁手/超时标志
                    int k=0;

                    for(int i=0;i<21;i++)
                    {
                        for(int j=0;j<21;j++)
                        {
                            buffer[k]='0';
                            k++;
                        }
                    }

                    board.placeqizi(i,j,netplayer);
                    update();

                    if(judger.judgewin(board,i,j))
                    {
                        if(netplayer==WHITE)
                            buffer[441]='w';
                        else
                            buffer[441]='b';

                        socket->write(buffer);

                        if(netplayer==WHITE)
                            QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                        else
                            QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
                        renew();
                    }
                    else if(netplayer==BLACK&&judger.FLAG)
                    {
                        buffer[441]='j';
                        socket->write(buffer);
                        QMessageBox::warning(NULL,"出现禁手：","白棋胜！");
                        renew();
                    }

                    else
                    {
                        int k=0;

                        for(int i=0;i<21;i++)
                        {
                            for(int j=0;j<21;j++)
                            {
                                buffer[k]=board.showqizi(i,j)+'0';
                                k++;
                            }
                        }
                        buffer[441]='0';

                        socket->write(buffer);
                    }

                }
            }

        }

        //和棋
        if(judger.judgedraw(board))
        {            
            Time2->stop();
            Time1->stop();
            QMessageBox::warning(NULL,"比赛结果：","和棋");
            renew();
        }
    }
}


void MainWindow::on_pushButton_ai_clicked()
{
    //机机对战
    if(mode==ai)
    {
        if(board.getbnum()==0)
        {
            board.placeqizi(10,10,BLACK);
            blackqizi=false;
            update();
        }

                timer = new QTimer(this);
                connect(timer, SIGNAL(timeout()), this, SLOT(AIplacenode()));
                timer->start(700);
        }


}



void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    menu1->move(cursor().pos());                               //让菜单显示的位置在鼠标的坐标上
    if(board.getbnum()==0&&board.getwnum()==0)
    menu1->show();
}



void MainWindow::renew()                               //更新棋盘，全部初始化
{
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


void MainWindow::AIplacenode()                                    //AI下棋
{
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

        int p;
        if(blackqizi)
            p=BLACK;
        else
            p=WHITE;

        srand(time(0));                                          //随机选择分数最大的一点落子
        int n=(rand()%(k-1))+1;
        board.placeqizi(maxvalue[n].x,maxvalue[n].y,p);

        if(p==BLACK)
            blackqizi=false;
        else
            blackqizi=true;

        if(judger.judgewin(board,maxvalue[n].x,maxvalue[n].y))
        {
            if(mode!=ai)
            {
                Time2->stop();
                QMessageBox::warning(NULL,"胜负已定：","白棋胜！");

            }
            else
            {
                timer->stop();
                if(blackqizi)
                    QMessageBox::warning(NULL,"胜负已定：","白棋胜！");
                else
                    QMessageBox::warning(NULL,"胜负已定：","黑棋胜！");
            }
            renew();
        }
        else if(judger.judgedraw(board))
        {
            if(mode!=ai)
            {
                Time2->stop();
                Time1->stop();
                QMessageBox::warning(NULL,"比赛结果：","和棋");
            }
            else
            {
                timer->stop();
                QMessageBox::warning(NULL,"比赛结果：","和棋");
            }

            renew();
        }
        update();
}



void MainWindow::timer1()                                           //白棋倒计时
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


void MainWindow::timer2()                                          //黑棋倒计时
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


void MainWindow::nettimer()                                             //网络倒计时
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
        buffertime[441]='c';
        socket->write(buffertime);
        if(netplayer==BLACK)
            QMessageBox::warning(NULL,"超时：","黑棋胜！");
        else
            QMessageBox::warning(NULL,"超时：","白棋胜！");
        renew();
        Nettimer->stop();
        ui->nettime->setText("15");
        TIME=15;
    }
}



void MainWindow::triggeredNormal()
{
    mode=Normal;
    ui->pushButton_2->show();
    ui->pushButton->show();
    ui->pushButton_ai->hide();
    ui->label->show();
    ui->label_3->show();
    ui->label2->show();
    ui->label_2->show();
    ui->label_4->show();
    ui->label_6->show();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->NetplayerLabel->hide();

    lable->setText("  当前游戏模式：双人对战  ");
    ui->statusbar->addWidget(lable);
    netlable->setText(" ");
    ui->statusbar->addPermanentWidget(netlable);
}


void MainWindow::triggeredAI()
{
    mode=AI;
    ui->pushButton_2->hide();
    ui->pushButton->show();
    ui->pushButton_ai->hide();
    ui->label->show();
    ui->label_3->show();
    ui->label2->show();
    ui->label_2->show();
    ui->label_4->show();
    ui->label_6->show();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->NetplayerLabel->hide();

    lable->setText("  当前游戏模式：人机对战  ");
    ui->statusbar->addWidget(lable);
    netlable->setText(" ");
    ui->statusbar->addPermanentWidget(netlable);
}


void MainWindow::triggeredai()
{
    mode=ai;
    ui->pushButton_2->hide();
    ui->pushButton->hide();
    ui->pushButton_ai->show();
    ui->label->hide();
    ui->label_3->hide();
    ui->label2->hide();
    ui->label_2->hide();
    ui->label_4->hide();
    ui->label_6->hide();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->NetplayerLabel->hide();

    lable->setText("  当前游戏模式：机机对战  ");
    ui->statusbar->addWidget(lable);
    netlable->setText(" ");
    ui->statusbar->addPermanentWidget(netlable);
}


void MainWindow::triggeredNet()
{
    mode=Net;
    ui->pushButton_2->hide();
    ui->pushButton_ai->hide();
    ui->pushButton->hide();
    ui->label->hide();
    ui->label_3->hide();
    ui->label2->hide();
    ui->label_2->hide();
    ui->label_4->hide();
    ui->label_6->hide();
    ui->label_5->show();
    ui->label_nettime->show();
    ui->nettime->show();

    lable->setText("  当前游戏模式：网络对战  ");
    ui->statusbar->addWidget(lable);

    gameserver=new QTcpServer;

    gameserver->listen(QHostAddress::Any,6666);                   //开端口6666

    connect(gameserver,&QTcpServer::newConnection,this,[=](){     //提示客户端已连接，准备读取信息
            netlable->setText("客户端连接成功！");
            ui->statusbar->addPermanentWidget(netlable);
            ui->NetplayerLabel->show();
            NetFlag=true;

            socket=gameserver->nextPendingConnection();
            connect(socket,SIGNAL(readyRead()),this,SLOT(receivemessage()));

    });

}



QImage MainWindow::ConvertImageToTransparent(QImage image)           //设置背景图片透明程度
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


