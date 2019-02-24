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

        connect(ui->pushBSend, &QPushButton::clicked, this, [=]()
        {
            QString msg = ui->lineEMsg->text();
            tcp->write(msg.toUtf8());
            // 添加到显示列表,处理子线程数据
            ui->textEditMsg->setTextColor(Qt::red);
            ui->textEditMsg->append("客户端: " + msg);
            ui->lineEMsg->clear();
            ui->lineEMsg->focusProxy();
        });

        // 接收数据
        connect(tcp, &QTcpSocket::readyRead, this, [=]()
        {
            QByteArray all = tcp->readAll();
            // 显示
            ui->textEditMsg->setTextColor(Qt::green);
            ui->textEditMsg->append("服务器: " + all);
        });
}

Widget::~Widget()
{
    delete ui;
}
