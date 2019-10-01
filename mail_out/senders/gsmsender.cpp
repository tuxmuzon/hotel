#include "gsmsender.h"
#include "QSqlQuery"
#include <QDebug>
#include <QDateTime>
#include <QStringList>

GSMSender::GSMSender(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    waitSmsInvite = true;
    isOpened = false;
    setEndline(EL_CRLF);
    endMessage.append(static_cast<char>(0x1A));
    codec = QTextCodec::codecForName("UTF-16BE");
    readReady = false;
}

GSMSender::~GSMSender()
{
    closePort();
}

void GSMSender::setEndline(const int &type)
{
    endLine.clear();
    switch(type)
    {
    case EL_CR: endLine.append(static_cast<char>(0x0D)); break;
    case EL_LF: endLine.append(static_cast<char>(0x0A)); break;
    default: endLine.append(static_cast<char>(0x0D)); endLine.append(static_cast<char>(0x0A)); break;
    }
}

void GSMSender::closePort()
{
    serial->clear();
    serial->close();
    isOpened = false;
}

bool GSMSender::sendSms(const QString &number, const QString &message)
{
    if(!isOpened)
    {
        errorString = "Порт не открыт";
        return false;
    }

    if(readReady)
    {
        readReady = false;
        readAndClear();
    }

    QString result;
    QByteArray fMessage(messageToUCS2(message,number));
    QByteArray id = QString("AT+CMGS="+QString::number((fMessage.length()-2)/2)).toLocal8Bit();
    serial->write(id);
    serial->write(endLine);
//    qDebug() << "AT+CMGS=" << id << ": "  << serial->readAll();
    if(waitSmsInvite && !serial->waitForReadyRead(500))
    {
        errorString = "Ошибка при оправке: модем не отвечает";
        qDebug() << "Ошибка при оправке: модем не отвечает";
        return false;
    }
    serial->write(fMessage);
    serial->write(endLine);
    serial->write(endMessage);
    serial->waitForBytesWritten(1000);
    serial->waitForReadyRead(3000);
//    if(serial->waitForReadyRead(5000))
//    {
        result = serial->readAll();
        qDebug() << "RESULT SEND: " << result;
//    }
//    QStringList rs = result.split(QRegExp("[\\r\\n\\s]+"));
//    if(!rs.contains("OK") && !rs.contains("+CMGS:"))
//    {
//        errorString = "Ошибка при оправке: "+result;
//        return false;
//    }

    return true;
}

bool GSMSender::removeSms(const int &id)
{
    serial->write(QString("AT+CMGD="+QString::number(id)).toLocal8Bit());
    serial->write(endLine);

    QByteArray result;
    if(serial->waitForReadyRead(500))
        result = serial->readAll();
    else
        return false;

    QStringList rs = QString(result).split(QRegExp("[\\r\\n\\s]+"));
    if(!rs.contains("OK"))
    {
        errorString = "Ошибка при удалении смс: "+result;
        return false;
    }

    return true;
}

bool GSMSender::readAndClear()
{
    if(!isOpened)
    {
        errorString = "Порт не открыт";
        return false;
    }

    // TODO : входящие читаем в талбицу incoming_sms, затем удаляем все входящие и отправленных
    serial->write("AT+CMGL=\"ALL\"");
    serial->write(endLine);
    QString buffer;
    while(serial->waitForReadyRead(500))
        buffer.append(serial->readAll());
    QStringList messages = buffer.split("+CMGL: ",QString::SkipEmptyParts);

    foreach (QString message, messages)
    {
        QStringList lines = message.split(QRegExp("[\\r\\n]+"),QString::SkipEmptyParts);
        if(lines.size()>=2)
        {
            QString header = lines.at(0);
            QString body = lines.at(1);
            QStringList params = header.split(",",QString::KeepEmptyParts);
            if(header.contains("\"REC"))
            {
                if(!params.value(2).isEmpty() && !params.value(4).isEmpty() && params.value(0).toInt()>0)
                {
                    QSqlQuery query;
                    query.prepare("INSERT INTO incoming_sms (originator, created, message) VALUES(?,?,?);");
                    query.addBindValue(params.value(2).trimmed());
                    query.addBindValue(QDateTime::fromString(params.value(4),"dd/MM/yy,hh:mm:ss"));
                    query.addBindValue(messageFromUCS2(body));
                    query.exec();

                    removeSms(params.value(0).toInt());
                }
            }
            else if(header.contains(" SENT"))
            {
                if(params.value(0).toInt()>0)
                    removeSms(params.value(0).toInt());
            }
        }
    }
    return true;
}

QByteArray GSMSender::messageToUCS2(const QString &message, const QString &number)
{
    // https://habrahabr.ru/post/133085/
    QString nm(number);
    if (nm.length() % 2 == 1)
        nm.append("F");
    QString pnumber;

    for (int i = 0; i<nm.length(); i++)
        if (i % 2 == 1)
            pnumber = pnumber + nm.at(i) + nm.at(i-1);

    QByteArray text;
    if(codec)
        text = codec->fromUnicode(message).toHex().toUpper();

    QByteArray result;
    result.append("00"); // Длина и номер SMS центра. 0 - означает, что будет использоваться дефолтный номер.
    result.append("11"); // SMS-SUBMIT
    result.append("00"); // Длина и номер отправителя. 0 - означает что будет использоваться дефолтный номер.
    result.append(QString::number(number.length(),16).toUpper().rightJustified(2,QChar('0'))); // Длина номера получателя
    result.append("91"); // Тип-адреса. (91 указывает международный формат телефонного номера, 81 - местный формат).
    result.append(pnumber); // Телефонный номер получателя в международном формате.
    result.append("00"); // Идентификатор протокола
    result.append("08"); // Старший полубайт означает сохранять SMS у получателя или нет (Flash SMS),  Младший полубайт - кодировка(0-латиница 8-кирилица).
    result.append("C1"); // Срок доставки сообщения. С1 - неделя
    result.append(QString::number(text.length()/2,16).toUpper().rightJustified(2,QChar('0'))); // Длина текста сообщения.
    result.append(text);
    return result;
}

QString GSMSender::messageFromUCS2(const QString &message)
{
    if(codec)
        return codec->toUnicode(QByteArray::fromHex(message.toLocal8Bit()));
    return message;
}

bool GSMSender::openPort()
{
    if(isOpened)
        return true;
    if (serial->open(QIODevice::ReadWrite))
    {
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setParity(QSerialPort::NoParity);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->clear();
        isOpened = true;
        QString initResult="START";

        // ВОТ такой интересный костыль!
       while (!initResult.split(QRegExp("[\\r\\n\\s]+")).contains("OK"))
        {
            serial->write("ATZ"); //ATZ : start the modem
            serial->write(endLine);
            serial->waitForBytesWritten(1000);
            serial->waitForReadyRead(1000);

            initResult =  serial->readAll();
            qDebug() << "ATZ: " << initResult;
        }

//        serial->write("ATE1"); //ATE1 : turn echo on
//        serial->write(endLine);
//        serial->waitForBytesWritten(1000);
//        serial->waitForReadyRead(1000);
//        qDebug() << "ATE1 " << serial->readAll();

        serial->write("AT+CMEE=1"); //Show error (if 2 show text errors !)
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);
        qDebug() << "AT+CMEE=1 " << serial->readAll();

        serial->write("AT+GMM"); //XYZ what?
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);
        qDebug() << "AT+GMM " << serial->readAll();

        serial->write("AT+CGMI"); //get manufacturer id
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);
        qDebug() << "AT+CGMI " << serial->readAll();


        serial->write("AT+CSCS=?"); //code page enable
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);
        qDebug() << "AT+CSCS=? " << serial->readAll();
        //TODO проверить что умеет,

        serial->write("AT+CSCS=\"UCS2\"");
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);

        qDebug() << "AT+CSCS=\"UCS2\"" <<  serial->readAll();




        serial->write("AT+CMGF=0");
        serial->write(endLine);
        serial->waitForBytesWritten(1000);
        serial->waitForReadyRead(1000);
        qDebug() << "AT+CMGF=0" <<  serial->readAll();



        if(serial->waitForReadyRead(300))
        {
            initResult = serial->readAll();
            qDebug() << "initResult: " << initResult;
        }
        if(!initResult.split(QRegExp("[\\r\\n\\s]+")).contains("OK"))
        {
            closePort();
            errorString = "GSM-модуль не ответил при подключении";
        }
    } else
    {
        isOpened = false;
        errorString = "Ошибка подключения: "+serial->errorString();
    }
    return isOpened;
}

