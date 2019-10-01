#ifndef GSMSENDER_H
#define GSMSENDER_H

#include <QObject>
#include <QTextCodec>
#include <QSerialPort>
#include <QSerialPortInfo>
/**
 * @brief Подключается к последовательному порту и отправляет СМС
 *  при помощи AT-команд (стандартно для всяких 3g-модемов и GSM-шлюзов)
 */
class GSMSender : public QObject
{
    Q_OBJECT
public:
    enum EndLineType{ EL_CR=0, EL_LF=1, EL_CRLF=2 };
    explicit GSMSender(QObject *parent = Q_NULLPTR);
    ~GSMSender() override;
    void setPort(const QString &portName){ serial->setPortName(portName); }
    void setEndline(const int &type);
    void setWaitForInvite(const bool &wait){ waitSmsInvite = wait; }
    bool openPort();
    void closePort();
    bool sendSms(const QString &number, const QString &message);
    QString lastError(){ return errorString; }
    QByteArray messageToUCS2(const QString &message, const QString &number);
    QString messageFromUCS2(const QString &message);
    void setReadReady(){ readReady = true; }
private:
    bool readAndClear();
    bool removeSms(const int &id);
    QSerialPort *serial;
    bool isOpened;
    QString errorString;
    QByteArray endLine;
    QByteArray endMessage;
    bool waitSmsInvite;
    QTextCodec *codec;
    bool readReady;
};

#endif // GSMSENDER_H



//ATZ : start the modem

//ATE1 : turn echo on

//AT+CMEE=1 : Show error (if 2 show text errors !)

//AT+GMM

//AT+CGMI :: get manufacturer id


//AT+CSCS=? : смотрим какие кодировки умеет модем

//AT+CSCS="GSM"

//AT+CMGF=0

//AT+CMGS=18 --номер

//ТЕКСТЫ
