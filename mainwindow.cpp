#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),mh(PSU_TYPE::POPEYE)
{
    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->serialPort->addItem(info.portName());

    ui->timeout->setRange(0, 10000);
    ui->timeout->setValue(1000);

    setWindowTitle(tr("Ericsson Communication Tester"));
    ui->serialPort->setFocus();

    connect(ui->sendMessage, &QPushButton::clicked, this, &MainWindow::transaction);
    connect(&m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(&m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);

    connect(ui->tabTypes, &QTabWidget::tabBarClicked, this, &MainWindow::psuTypeChanged);
    connect(ui->rb_PID, &QRadioButton::clicked, this, &MainWindow::rb_messageTypeChanged);
    connect(ui->rb_GetMeasurements, &QRadioButton::clicked, this, &MainWindow::rb_messageTypeChanged);
    connect(ui->rb_LED_Check, &QRadioButton::clicked, this, &MainWindow::rb_messageTypeChanged);

    connect(ui->comboBox_LED, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::dataTypeChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::transaction()
{
    switch (actualPsuType) {
    case PSU_TYPE::POPEYE:

        switch (actualMessageType) {

        case MESSAGE_TYPE::PID:
        {
            setControlsEnabled(false);
            ui->statusLabel->setText(tr("Status: Running, connected to port %1.")
                                   .arg(ui->serialPort->currentText()));
            QByteArray q;

            mh.create_message(q, MESSAGE_TYPE::PID);

            ui->message->setText(QString(q.toHex().data()));
            qDebug() << q.toHex();

            m_thread.transaction(ui->serialPort->currentText(),
                                 ui->timeout->value(),
                                 ui->message->text());
        }
            break;
        case MESSAGE_TYPE::LED:
        {
            setControlsEnabled(false);
            ui->statusLabel->setText(tr("Status: Running, connected to port %1.")
                                   .arg(ui->serialPort->currentText()));
            QByteArray q;

            mh.create_message(q, MESSAGE_TYPE::LED, actualDataType);

            ui->message->setText(QString(q.toHex().data()));
            qDebug() << q.toHex();

            m_thread.transaction(ui->serialPort->currentText(),
                                 ui->timeout->value(),
                                 ui->message->text());
        }
            break;
        case MESSAGE_TYPE::GET_MEASUREMENTS:
        {
            setControlsEnabled(false);
            ui->statusLabel->setText(tr("Status: Running, connected to port %1.")
                                   .arg(ui->serialPort->currentText()));
            QByteArray q;

            mh.create_message(q, MESSAGE_TYPE::GET_MEASUREMENTS, actualDataType);

            ui->message->setText(QString(q.toHex().data()));
            qDebug() << q.toHex();

            m_thread.transaction(ui->serialPort->currentText(),
                                 ui->timeout->value(),
                                 ui->message->text());
        }
            break;
        default:
            setControlsEnabled(true);
            ui->textEdit->append("Nem támogatott választás!!");
            break;
        }
        break;
    default:
        setControlsEnabled(true);
        ui->textEdit->append("Nem támogatott választás!!");
        break;
    }
}

void MainWindow::showResponse(const QString &s)
{
    setControlsEnabled(true);
    ui->textEdit->setText(tr("Traffic, transaction #%1:"
                               "\n\r-request: %2"
                               "\n\r-response: %3")
                            .arg(++m_transactionCount)
                            .arg(ui->message->text())
                            .arg(s));

    switch (actualPsuType) {
    case PSU_TYPE::POPEYE:

        switch (actualMessageType) {

        case MESSAGE_TYPE::PID:
        {  QString response;

            mh.read_message(response, DATA_TYPE::SERIALNUMBER, QByteArray().insert(0, s.trimmed()));
            ui->text_SerialNumber->setText(response);

            mh.read_message(response, DATA_TYPE::PRODUCT_NUMBER, QByteArray().insert(0, s.trimmed()));
            ui->text_ProductNumber->setText(response);

            mh.read_message(response, DATA_TYPE::PRODUCT_DATE, QByteArray().insert(0, s.trimmed()));
            ui->text_ProductionDate->setText(response);

            mh.read_message(response, DATA_TYPE::PRODUCT_NAME, QByteArray().insert(0, s.trimmed()));
            ui->text_ProductName->setText(response);

            mh.read_message(response, DATA_TYPE::PRODUCT_REVISION, QByteArray().insert(0, s.trimmed()));
            ui->text_RevNmber->setText(response);

        }
            break;
        case MESSAGE_TYPE::GET_MEASUREMENTS:
        {
            QString response;

            mh.read_message(response, DATA_TYPE::VOLTAGE, QByteArray().insert(0, s.trimmed()));

            switch (actualDataType)
            {
            case DATA_TYPE::VOLTAGE:
                ui->textGetMeasurements->setText(response);
                break;
            case DATA_TYPE::CURRENT:
                ui->text_SerialNumber->setText(response);
                break;
            case DATA_TYPE::TEMPERATURE:
                ui->text_SerialNumber->setText(response);
                break;
            case DATA_TYPE::POWER:
                ui->text_SerialNumber->setText(response);
                break;
            default:
                break;
            }

        }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void MainWindow::processError(const QString &s)
{
    setControlsEnabled(true);
    ui->statusLabel->setText(tr("Status: Not running, %1.").arg(s));
    ui->trafficLabel->setText(tr("No traffic."));
}

void MainWindow::processTimeout(const QString &s)
{
    setControlsEnabled(true);
    ui->statusLabel->setText(tr("Status: Running, %1.").arg(s));
    ui->trafficLabel->setText(tr("No traffic."));
}

void MainWindow::setControlsEnabled(bool enable)
{
    ui->sendMessage->setEnabled(enable);
    ui->serialPort->setEnabled(enable);
    ui->timeout->setEnabled(enable);
    ui->message->setEnabled(enable);
    ui->tabTypes->setEnabled(enable);
}

void MainWindow::rb_messageTypeChanged(bool checked)
{
        if (ui->rb_PID->isChecked())
        {
            actualMessageType = MESSAGE_TYPE::PID;
            qDebug()<<"PID..";
        } else if(ui->rb_LED_Check->isChecked())
        {
            actualMessageType = MESSAGE_TYPE::LED;
            actualDataType = DATA_TYPE::LED_NO_FAULT;
            ui->comboBox_LED->setCurrentIndex(0);

            qDebug()<<"LED..";
        } else if(ui->rb_GetMeasurements->isChecked())
        {
            actualMessageType = MESSAGE_TYPE::GET_MEASUREMENTS;
            actualDataType = DATA_TYPE::VOLTAGE;
            qDebug()<<"GetMeasurements..";
        } else
        {
            actualMessageType = MESSAGE_TYPE::NONE;
            qDebug()<<"NONE!!!..";
        }
}

void MainWindow::psuTypeChanged(int index)
{
    switch (index) {
    case 0:
        actualPsuType = PSU_TYPE::POPEYE;
        break;
    default:
        actualPsuType = PSU_TYPE::NONE;
        break;
    }
}

void MainWindow::dataTypeChanged(int index)
{
    switch (index) {
    case 0:
        actualDataType = DATA_TYPE::LED_NO_FAULT;
        break;
    case 1:
        actualDataType = DATA_TYPE::LED_FAULT;
        break;
    case 2:
        actualDataType = DATA_TYPE::LED_BUSY;
        break;
    default:
        actualDataType = DATA_TYPE::NONE;
        break;
    }
}
