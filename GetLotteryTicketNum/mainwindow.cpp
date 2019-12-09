#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore>
#include <iostream>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std ;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int PotStep;  //定义偏移指针，用于指定需要获取数据的地址
    QString ssqUrl ="http://www.17500.cn/ssq/details.php?issue=";
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl("http://www.17500.cn/ssq/details.php?issue=2003001")));
    QByteArray responseData;
    QEventLoop eventLoop;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    responseData = reply->readAll();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);                                   //连接数据库端口号
    db.setDatabaseName("lotteryticket");       //这里输入你的数据库名
    db.setUserName("root");
    db.setPassword("zaq!@#WSX556");   //这里输入你的密码
    if (!db.open()) {
        QMessageBox::critical(this, QObject::tr("无法打开数据库"),"无法创建数据库连接！ ", QMessageBox::Cancel);
    }
    QSqlQuery query(db);
    //query.exec("select * from student");
    query.prepare("INSERT INTO ssq (issuenum,   red1,  red2,  red3,  red4,  red5,  red6,  blue1,  datatime) "
                  "         VALUES (:issuenum, :red1, :red2, :red3, :red4, :red5, :red6, :blue1, :datatime)"); //准备执行
    //ui->textBrowser->setText(QString(responseData));
    //cout << QString(responseData).toStdString() <<endl; //调试输出获取到的网页内容
    //int issue= 2003001;
    for(int issue= 2003001;;issue++){

        reply = manager->get(QNetworkRequest(QUrl( ssqUrl + QString::number(issue) )));
        eventLoop.exec();       //block until finish
        responseData = reply->readAll();

        PotStep = QString(responseData).indexOf("align=\"right\">",0); //获取开奖时间的首地址
        //qDebug() << "DatatimeAddre = " << PotStep;
        if(-1 != PotStep){
            qDebug() << QString(responseData).mid(PotStep+14,10); //调试输出
            query.bindValue(":datatime",QDateTime::fromString(QString(responseData).mid(PotStep+14,10)
                                                              + " 22:00:00",
                                                              "yyyy-MM-dd hh:mm:ss"));   //绑定开奖日期
            query.bindValue(":issuenum", issue);   //绑定彩票期号
            qDebug() << "issuenum = " << issue;
            //red1
            PotStep = QString(responseData).indexOf("%\"><font color=red>",0); //获取第1个红球号码
            //ui->label->setText(QString(responseData).mid(PotStep+19,2)+" ");
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red1", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第1个红球号码
            //red2
            PotStep = QString(responseData).indexOf("%\"><font color=red>",PotStep+20);//获取第2个红球号码
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red2", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第2个红球号码
            //red3
            PotStep = QString(responseData).indexOf("%\"><font color=red>",PotStep+20);//获取第3个红球号码
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red3", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第3个红球号码
            //red4
            PotStep = QString(responseData).indexOf("%\"><font color=red>",PotStep+20);//获取第4个红球号码
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red4", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第4个红球号码
            //red5
            PotStep = QString(responseData).indexOf("%\"><font color=red>",PotStep+20);//获取第5个红球号码
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red5", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第5个红球号码
            //red6
            PotStep = QString(responseData).indexOf("%\"><font color=red>",PotStep+20);//获取第6个红球号码
            qDebug() << QString(responseData).mid(PotStep+19,2); //调试输出
            query.bindValue(":red6", QString(responseData).mid(PotStep+19,2).toInt());   //绑定第6个红球号码

            //blue1
            PotStep = QString(responseData).indexOf("align=center><font color=blue>",0);//获取第1个蓝球号码首地址
            query.bindValue(":blue1", QString(responseData).mid(PotStep+30,2).toInt());   //绑定第1个蓝球号码
            query.exec();

        }else{
            issue /= 1000;
            issue *= 1000;
            issue += 1001;//计算下一期期号
            qDebug() << issue;
        }

        if(issue > 2020000){qDebug() << "完成";break;}  //如果超期了，就终止
        //延个时
//        QTime dieTime = QTime::currentTime().addMSecs(1000);
//        while( QTime::currentTime() < dieTime )
//            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


    }




    db.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

