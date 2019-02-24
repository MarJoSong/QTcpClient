#include "widget.h"
#include "ui_widget.h"
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QHostAddress>
#include <QFile>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

        QTcpSocket *tcp = new QTcpSocket(this);
        QThread* subThread = new QThread(this);
        DoWork *work = new DoWork;
        work->moveToThread(subThread);
        // 启动子线程
        subThread->start();

        // init
        ui->lineEIP->setText("47.101.72.167");
        ui->lineEPort->setText("8888");

        // 连接server
        connect(ui->pushBConnect, &QPushButton::clicked, this, [=]()
        {
            tcp->connectToHost(QHostAddress(ui->lineEIP->text()), ui->lineEPort->text().toInt());
        });
        // 判断是否连接成功
        connect(tcp, &QTcpSocket::connected, this, [=]()
        {
            ui->pushBConnect->setEnabled(false);
            ui->textEditMsg->setTextColor(Qt::blue);
            ui->textEditMsg->setFont(QFont("仿宋", 16));
            ui->textEditMsg->append("成功连接服务器 ^_^ 。。。");
        });
        // 向服务器发送数据
        connect(ui->pushBSend, &QPushButton::clicked, work, &DoWork::Working);
        connect(ui->pushBSend, &QPushButton::clicked, this, [=]()
        {
            ui->pushBSend->setEnabled(false);
        });
        // 处理子线程数据
        connect(work, &DoWork::showInfo, this, [=](QString text)
        {
            tcp->write(text.toUtf8());
            // 添加到显示列表
            ui->textEditMsg->setTextColor(Qt::red);
            ui->textEditMsg->append("Client: " + text);
        });
        connect(work, &DoWork::workDone, this, [=]()
        {
            ui->pushBSend->setEnabled(true);
        });
        // 接收数据
        connect(tcp, &QTcpSocket::readyRead, this, [=]()
        {
            QByteArray all = tcp->readAll();
            // 显示
            ui->textEditMsg->setTextColor(Qt::green);
            ui->textEditMsg->append("Server: " + all);
        });
        // 关闭子线程
        connect(this, &Widget::destroyed, this, [=]()
        {
           subThread->quit();
           subThread->wait();
           delete work;
        });
        qDebug() << "main:" << QThread::currentThread();
}

Widget::~Widget()
{
    delete ui;
}

DoWork::DoWork(QObject *parent)
{
    Q_UNUSED(parent);
}

void DoWork::Working()
{

    qDebug() << "sub:" << QThread::currentThread();
    QFile file("/home/mars/Tcp-with-Qt/english.txt");
    bool bl = file.open(QFile::ReadOnly);
    if(!bl)
    {
        qDebug() << "open file failed";
        return;
    }
    // 读文件
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        emit showInfo(line);
        QThread::sleep(1);
        //qDebug() << line;
    }
    emit workDone();
}
