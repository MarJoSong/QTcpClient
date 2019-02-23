#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTcpSocket *tcp = new QTcpSocket(this);
    QThread* subThread = new QThread(this);
    DoWork *work = new DoWork;
    work->moveToThread(subThread);
    // 启动子线程
    subThread->start();

    // init
    ui->s_IP->setText("192.168.1.103");
    ui->s_port->setText("8888");

    // 连接server
    connect(ui->btn_connect, &QPushButton::clicked, this, [=]()
    {
        tcp->connectToHost(QHostAddress(ui->s_IP->text()), ui->s_port->text().toInt());
    });
    // 判断是否连接成功
    connect(tcp, &QTcpSocket::connected, this, [=]()
    {
        ui->btn_connect->setEnabled(false);
        ui->msg->setTextColor(Qt::blue);
        ui->msg->setFont(QFont("仿宋", 16));
        ui->msg->append("成功连接服务器 ^_^ 。。。");
    });
    // 向服务器发送数据
    connect(ui->btn_send, &QPushButton::clicked, work, &DoWork::Working);
    connect(ui->btn_send, &QPushButton::clicked, this, [=]()
    {
        ui->btn_send->setEnabled(false);
    });
    // 处理子线程数据
    connect(work, &DoWork::showInfo, this, [=](QString text)
    {
        tcp->write(text.toUtf8());
        // 添加到显示列表
        ui->msg->setTextColor(Qt::red);
        ui->msg->append("Client: " + text);
    });
    connect(work, &DoWork::workDone, this, [=]()
    {
        ui->btn_send->setEnabled(true);
    });
    // 接收数据
    connect(tcp, &QTcpSocket::readyRead, this, [=]()
    {
        QByteArray all = tcp->readAll();
        // 显示
        ui->msg->setTextColor(Qt::green);
        ui->msg->append("Server: " + all);
    });
    // 关闭子线程
    connect(this, &MainWindow::destroyed, this, [=]()
    {
       subThread->quit();
       subThread->wait();
       delete work;
    });
    qDebug() << "main:" << QThread::currentThread();
}

MainWindow::~MainWindow()
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
    QFile file("english.txt");
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
