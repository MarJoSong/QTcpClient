#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class DoWork : public QObject
{
    Q_OBJECT
public:
    DoWork(QObject* parent = 0);
    void Working();

signals:
    void showInfo(QString info);
    void workDone();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // QString m_record;
};

#endif // MAINWINDOW_H
