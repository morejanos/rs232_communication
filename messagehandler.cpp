#include "messagehandler.h"
#include <QDebug>

MessageHandler::MessageHandler(PSU_TYPE type = PSU_TYPE::POPEYE)
{
    PSUType = type;
}

void MessageHandler::read_message(QString &output_response, DATA_TYPE dt, const QByteArray data)
{
    switch (PSUType) {
    case PSU_TYPE::POPEYE:
        {
        m_message = data.data();
        const QByteArray response = QByteArray::fromHex(data);
        switch (dt) {
        case DATA_TYPE::SERIALNUMBER:
        {
            output_response = response.mid(59, 14);
        }
            break;
        case DATA_TYPE::PRODUCT_NUMBER:
        {
            output_response = response.mid(5, 25);
        }
            break;
        case DATA_TYPE::PRODUCT_NAME:
        {
            output_response = response.mid(37, 13);
        }
            break;
        case DATA_TYPE::PRODUCT_DATE:
        {
            output_response = response.mid(50, 9);
        }
            break;
        case DATA_TYPE::PRODUCT_REVISION:
        {
            output_response = response.mid(30, 7);
        }
            break;
        case DATA_TYPE::VOLTAGE:
        {
            // TODO: Make voltage calculation
            output_response = response.mid(8, 2);
        }
            break;

        default:
            output_response = QString("NO PID DATA");
            break;
        }
    }
        break;
    default:
        output_response = QString("NO DATA");
        break;
    }

    output_response.trimmed();
}

void MessageHandler::create_message(QByteArray &output_data, MESSAGE_TYPE mt, DATA_TYPE dt)
{
    switch (PSUType) {
    case PSU_TYPE::POPEYE:
        switch (mt) {
        case MESSAGE_TYPE::PID:
        {
            static const char mydata[] = {0x7e, 0x30, 0x13, 0x08, 0x00, 0x1d, 0x14, 0x7e};
            output_data.resize(sizeof(mydata));
            output_data = QByteArray::fromRawData(mydata, sizeof(mydata));
        }
            break;
        case MESSAGE_TYPE::GET_MEASUREMENTS:
        {
        static const char mydata[] = {0x7e, 0x30, 0x13, 0x48, 0x00, 0x7b, 0x52, 0x7e};
        output_data.resize(sizeof(mydata));
        output_data = QByteArray::fromRawData(mydata, sizeof(mydata));
        }
        break;

        case MESSAGE_TYPE::LED:
            switch (dt) {
            case DATA_TYPE::LED_BUSY:
            {
                // Busy LED
                static const char mydata[] = {0x7e, 0x30, 0x13, 0x0c, 0x00, 0x22, 0x79, 0x5a, 0x7e};

                output_data.resize(sizeof(mydata));
                output_data = QByteArray::fromRawData(mydata, sizeof(mydata));
            }
                break;
            case DATA_TYPE::LED_FAULT:
            {
                // Fault LED
                static const char mydata[] = {0x7e, 0x30, 0x13, 0x0c, 0x00, 0x31, 0x63, 0x78, 0x7e};

                output_data.resize(sizeof(mydata));
                output_data = QByteArray::fromRawData(mydata, sizeof(mydata));
            }
                break;
            case DATA_TYPE::LED_NO_FAULT:
            {
                // No Fault LED
                static const char mydata[] = {0x7e, 0x30, 0x13, 0x0c, 0x00, 0x30, (char) 0xea, 0x69, 0x7e};

                output_data.resize(sizeof(mydata));
                output_data = QByteArray::fromRawData(mydata, sizeof(mydata));
            }
                break;
            default:
                break;
            }

        default:
            break;
        }
        break;
    default:
        break;
    }
}
