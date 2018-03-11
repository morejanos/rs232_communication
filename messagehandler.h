#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QByteArray>
#include <QString>

enum class PSU_TYPE {NONE, POPEYE, BFU};
enum class MESSAGE_TYPE {NONE, PID, LED, GET_MEASUREMENTS};
enum class DATA_TYPE {NONE, SERIALNUMBER, PRODUCT_NUMBER, PRODUCT_REVISION, PRODUCT_NAME, PRODUCT_DATE,
                     LED_BUSY, LED_FAULT, LED_NO_FAULT, VOLTAGE, CURRENT, TEMPERATURE, POWER};

class MessageHandler
{
    QString m_message = "";

    PSU_TYPE PSUType;

    char frameStart = 0x7E;
    char frameEnd = 0x7E;
    char address = 0x10;
    char command = 0x12;

    QByteArray Data();

    char32_t crc16;

public:

    MessageHandler(PSU_TYPE);

    void read_message(QString &output_response, DATA_TYPE dt, const QByteArray data);
    void create_message(QByteArray &data, MESSAGE_TYPE mt, DATA_TYPE dt = DATA_TYPE::NONE);

};

#endif // MESSAGEHANDLER_H
