#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "masterthread.h"
#include "messagehandler.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void transaction();
    void showResponse(const QString &s);
    void processError(const QString &s);
    void processTimeout(const QString &s);

    void rb_messageTypeChanged(bool checked);
    void dataTypeChanged(int index);

    void psuTypeChanged(int index);

private:
    void setControlsEnabled(bool enable);

private:
    Ui::MainWindow *ui;
    MasterThread m_thread;
    int m_transactionCount = 0;
    MessageHandler mh;

    PSU_TYPE actualPsuType = PSU_TYPE::POPEYE;
    MESSAGE_TYPE actualMessageType = MESSAGE_TYPE::PID;
    DATA_TYPE actualDataType = DATA_TYPE::SERIALNUMBER;

};

#endif // MAINWINDOW_H
