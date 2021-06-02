#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <QMessageBox>
#include <QDebug>
#include <iostream>
using namespace std;

#define BLACK 1
#define WHITE 2

enum gamemode{ Normal,AI,ai,Net };          //游戏模式


struct Node{                             //每个棋子坐标
    int x=-1,y=-1;
};


class qizi{                              //棋子类
    int locationx,locationy,player;      //x坐标，y坐标，棋子颜色

    public:
        qizi()
        {
            locationx=0;
            locationy=0;
            player=-1;
        }

        void set(int a,int b,int c)
        {
            locationx=a;
            locationy=b;
            player=c;
        }

        void setplayer(int x)
        {
            player=x;
        }

        int getx(){return locationx;}
        int gety(){return locationy;}
        int getplayer(){return player;}

};


class checkerboard{                           //棋盘类
    int size=20;
    qizi qz[22][22];
    Node nodeblack[250],nodewhite[250];       //每下一个棋子便将坐标存入相应的数组
    int ib=0,iw=0;                            //记录数组中棋子个数

    public:
        int value[22][22];                    //每个空位的评分记录


        checkerboard()
        {
            for(int i=0;i<22;i++)
            {
                for(int j=0;j<22;j++)
                {
                    value[i][j]=0;
                    qz[i][j].set(i,j,-1);          //初始化棋盘
                }
            }
        }

        void placeqizi(int x,int y,int p)          //下棋函数
        {
            if(qz[x][y].getplayer()!=-1)
            return;
            if(x<0||x>20||y<0||y>20)
            return;

            qz[x][y].setplayer(p);
            value[x][y]=0;

            if(p==BLACK)                          //放入棋子路径数组
            {
                ib++;
                nodeblack[ib].x=x;
                nodeblack[ib].y=y;
            }
            if(p==WHITE)
            {
                iw++;
                nodewhite[iw].x=x;
                nodewhite[iw].y=y;
            }
        }

        int showqizi(int x,int y)                 //展示指定点棋子的player
        {
            if(x<0||x>20||y<0||y>20)
            return -1;            
            return qz[x][y].getplayer();
        }

        void setundoqz(int x,int y)               //悔棋后设置Player为-1
        {
            qz[x][y].setplayer(-1);
        }

        void undo(int p)                          //悔棋
        {
            if(p==BLACK)
            {
                setundoqz(nodeblack[ib].x,nodeblack[ib].y);
                nodeblack[ib].x=-1;
                nodeblack[ib].y=-1;
                ib--;
            }
            if(p==WHITE)
            {
                setundoqz(nodewhite[iw].x,nodewhite[iw].y);
                nodewhite[iw].x=-1;
                nodewhite[iw].y=-1;
                iw--;
            }
        }

        int getbnum()                       //获取已下棋子个数
        {
            return ib;
        }
        int getwnum()
        {
            return iw;
        }

};


class judgement{

    public:
        bool judgedraw(checkerboard board)                  //判断和棋
        {
            if(board.getbnum()+board.getwnum()==441)
                return true;
            return false;
        }

        int FLAG=0;                                         //是否产生禁手标志
        bool judgewin(checkerboard board,int x,int y)       //判断禁手
        {
            FLAG=0;
            int directionx[8]={-1,1,0,0,-1,1,-1,1};         //上下，左右，左上右下，右上左下
            int directiony[8]={0,0,1,-1,-1,1,1,-1};
            int num[4]={1,1,1,1},i=0,j=0,n4=0,n5=0,J4[4],J5[4];
            int p=board.showqizi(x,y);

            for(i=0,j=0;i<8;i++)                            //八个方向朴素遍历
            {
                int X=x+directionx[i];
                int Y=y+directiony[i];
                while(true)
                {
                    if(X<0||X>20||Y<0||Y>20)
                    break;
                    if(board.showqizi(X,Y)==p)
                    {
                    num[j]++;
                    X+=directionx[i];
                    Y+=directiony[i];
                    }
                    else
                    break;
                }
                if((i+1)%2==0)
                {
                    if(board.showqizi(x,y)==BLACK&&num[j]==4)       //标记可能产生四四禁手
                    {
                        J4[n4]=j;
                        n4++;
                    }
                    if(board.showqizi(x,y)==BLACK&&num[j]==5)       //标记可能产生五五禁手
                    {
                        J5[n5]=j;
                        n5++;
                    }
                    j++;
                }             
            }

            for(int i=0;i<4;i++)           //赢棋
            {                
                if(num[i]==6)
                {
                    return true;
                }
            }

            for(int i=0;i<4;i++)           //长连禁手
            {
                if(num[i]>6)
                    FLAG=1;
            }

            //四四禁手
            if(n4>1)
            {
                int count=0;
                for(int i=0;i<n4;i++)
                {
                    int X=x+directionx[J4[i]*2];
                    int Y=y+directiony[J4[i]*2];
                    int flag=0;
                    while(true)
                    {
                        if(X<0||X>20||Y<0||Y>20)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showqizi(X,Y)==WHITE)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showqizi(X,Y)==-1)
                        {
                            break;
                        }
                        else
                        {
                            X+=directionx[J4[i]*2];
                            Y+=directiony[J4[i]*2];
                        }
                    }
                    if(flag)
                        continue;
                    else
                    {
                        int X=x+directionx[J4[i]*2+1];
                        int Y=y+directiony[J4[i]*2+1];
                        int flag=0;
                        while(true)
                        {
                            if(X<0||X>20||Y<0||Y>20)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showqizi(X,Y)==WHITE)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showqizi(X,Y)==-1)
                            {
                                break;
                            }
                            else
                            {
                                X+=directionx[J4[i]*2+1];
                                Y+=directiony[J4[i]*2+1];
                            }
                        }
                        if(flag)
                            continue;
                        else
                            count++;
                    }
                }
                if(count>1)
                {
                    FLAG=1;
                }
            }

            //五五禁手
            if(n5>1)
            {
                int count2=0;
                for(int i=0;i<n5;i++)
                {
                    int X=x+directionx[J5[i]*2];
                    int Y=y+directiony[J5[i]*2];
                    int flag=0;
                    while(true)
                    {
                        if(X<0||X>20||Y<0||Y>20)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showqizi(X,Y)==WHITE)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showqizi(X,Y)==-1)
                        {
                            break;
                        }
                        else
                        {
                            X+=directionx[J5[i]*2];
                            Y+=directiony[J5[i]*2];
                        }
                    }
                    if(flag)
                        continue;
                    else
                    {
                        int X=x+directionx[J5[i]*2+1];
                        int Y=y+directiony[J5[i]*2+1];
                        int flag=0;
                        while(true)
                        {
                            if(X<0||X>20||Y<0||Y>20)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showqizi(X,Y)==WHITE)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showqizi(X,Y)==-1)
                            {
                                break;
                            }
                            else
                            {
                                X+=directionx[J5[i]*2+1];
                                Y+=directiony[J5[i]*2+1];
                            }
                        }
                        if(flag)
                            continue;
                        else
                            count2++;
                    }
                }
                if(count2>1)
                {
                    FLAG=1;
                }
            }

            return false;
        }


        void jugdevalue(checkerboard *board)            //AI评分
        {
            int directionx[8]={-1,1,0,0,-1,1,-1,1};     //上下，左右，左上右下，右上左下
            int directiony[8]={0,0,1,-1,-1,1,1,-1};
            int num1,num2,empty1,empty2;

            for(int i=0;i<21;i++)
            {
                for(int j=0;j<21;j++)
                {
                    if(board->showqizi(i,j)==BLACK)      //黑棋评分
                    {
                        for(int k=0;k<8;k++)
                        {
                            int X=i,Y=j,X1,X2,Y1,Y2;
                            if(k%2==0)
                            {   num1=1;
                                empty1=0;
                            }
                            X=i+directionx[k];
                            Y=j+directiony[k];
                            while(true)
                            {
                                if(X<0||X>20||Y<0||Y>20)
                                    break;
                                if(board->showqizi(X,Y)==BLACK)
                                {
                                    num1++;
                                    X+=directionx[k];
                                    Y+=directiony[k];
                                }
                                else if(board->showqizi(X,Y)==-1)
                                {
                                    empty1++;
                                    if(empty1==1)
                                    {
                                        X1=X;                //记录空格坐标
                                        Y1=Y;
                                    }
                                    else
                                    {
                                        X2=X;
                                        Y2=Y;
                                    }
                                    break;
                                }
                                else
                                break;
                            }

                            if(k%2)
                            {
                                if(num1==1&&empty1!=0)
                                {
                                    if(empty1==1)
                                        board->value[X1][Y1]+=10;          //单棋
                                    else
                                    {
                                        board->value[X1][Y1]+=10;
                                        board->value[X2][Y2]+=10;
                                    }
                                }

                                if(num1==2&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=10;       //死二
                                    else
                                    {
                                            board->value[X1][Y1]+=20;       //活二
                                            board->value[X2][Y2]+=20;
                                    }
                                }

                                if(num1==3&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=30;        //死三
                                    else
                                    {
                                            board->value[X1][Y1]+=50;        //活三
                                            board->value[X2][Y2]+=50;
                                    }
                                }

                                if(num1==4&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=50;        //死四
                                    else
                                    {
                                            board->value[X1][Y1]+=1000;      //活四
                                            board->value[X2][Y2]+=1000;
                                    }
                                }

                                if(num1==5&&empty1!=0)                        //活五
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=10000;
                                    else
                                    {
                                            board->value[X1][Y1]+=10000;
                                            board->value[X2][Y2]+=10000;
                                    }
                                }
                            }
                        }
                    }


                    if(board->showqizi(i,j)==WHITE)               //白棋评分
                    {
                        for(int k=0;k<8;k++)
                        {
                            int X=i,Y=j,X1,X2,Y1,Y2;
                            if(k%2==0)
                            {   num2=1;
                                empty2=0;
                            }
                            X=i+directionx[k];
                            Y=j+directiony[k];
                            while(true)
                            {
                                if(X<0||X>20||Y<0||Y>20)
                                    break;
                                if(board->showqizi(X,Y)==WHITE)
                                {
                                    num2++;
                                    X+=directionx[k];
                                    Y+=directiony[k];
                                }
                                else if(board->showqizi(X,Y)==-1)
                                {
                                    empty2++;
                                    if(empty2==1)
                                    {
                                        X1=X;                    //记录空格坐标
                                        Y1=Y;
                                    }
                                    else
                                    {
                                        X2=X;
                                        Y2=Y;
                                    }
                                    break;
                                }
                                else
                                break;
                            }

                            if(k%2)
                            {
                                if(num2==1&&empty2!=0)
                                {
                                    if(empty2==1)                         //单棋
                                        board->value[X1][Y1]+=10;
                                    else
                                    {
                                            board->value[X1][Y1]+=10;
                                            board->value[X2][Y2]+=10;
                                    }
                                }

                                if(num2==2&&empty2!=0)
                                {
                                    if(empty2==1)
                                            board->value[X1][Y1]+=15;      //死二
                                    else
                                    {
                                            board->value[X1][Y1]+=20;      //活二
                                            board->value[X2][Y2]+=20;
                                    }
                                }

                                if(num2==3&&empty2!=0)
                                {
                                    if(empty2==1)
                                            board->value[X1][Y1]+=30;       //死三
                                    else
                                    {
                                            board->value[X1][Y1]+=50;       //活三
                                            board->value[X2][Y2]+=50;
                                    }
                                }

                                if(num2==4&&empty2!=0)
                                {
                                    if(empty2==1)
                                            board->value[X1][Y1]+=200;      //死四
                                    else
                                    {
                                            board->value[X1][Y1]+=2000;     //活四
                                            board->value[X2][Y2]+=2000;
                                    }
                                }

                                if(num2==5&&empty2!=0)                      //活五
                                {
                                    if(empty2==1)
                                            board->value[X1][Y1]+=20000;
                                    else
                                    {
                                            board->value[X1][Y1]+=20000;
                                            board->value[X2][Y2]+=20000;
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
};

#endif // CHECKERBOARD_H
