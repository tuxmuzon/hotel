#ifndef ABSTRACTDEVFISCALOBJECT_H
#define ABSTRACTDEVFISCALOBJECT_H

#include <QObject>
#include <QString>

namespace fiscalSpace{
enum calculationMethod
{
    None = 0,
    PrepaymentOneHundredPercent = 1,
    Prepayment = 2,
    //Аванс
    PrepaidExpense = 3,
    FullPayment = 4,
    PartPaymnetAndCredit = 5,
    SendInCredit = 6,
    PayCredit = 7
};


enum payType
{
    payTypeCash = 0,
    payTypeElectronic = 1,
    //Аванс
    payTypeExpense = 2,
    //Кредит
    payTypCredit = 3
};

enum calculationSubject {
    calculationSubjectNone = 0,
    calculationSubjectProduct = 1,
    calculationSubjectService = 4
};

enum operationType{
    none = 0,
    sale = 1,
    returnSale = 2,
    inCash = 3,
    outCash = 4,
    zReport = 5,
    xReport = 6
};

struct FISCALSTRING
{
    QString goodCode;
    QString goodName;
    double goodPrice;
    double goodCount=1;
    int taxNumber=1;
    int positionSum= 1;
    calculationSubject calculationSubjectValue = calculationSubjectProduct ;
    calculationMethod calculationMethodValue = FullPayment;
};

struct CLOSESTRING
{
    const double &result;
    payType payTypeValue = payTypeCash;
    const double &paymentSum;
};

class AbstractDevFiscalObject : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDevFiscalObject(QObject *parent = nullptr);

    virtual ~AbstractDevFiscalObject() {}
    virtual QString showProperties() = 0;
    virtual QString fiscalStatus() = 0;
    virtual QString openFiscalDocument(operationType fiscalDoc, double sum=-1) = 0;
    virtual QString getShiftNumber() = 0;
    virtual QString openShift() = 0;
    virtual QString closeShift() = 0;
    virtual QString printXReport() = 0;
    virtual QString setAtributeSubjectCalculation () = 0;
    virtual QString printFiscalString(FISCALSTRING str) = 0;
    virtual QString printNoFiscalString(QString text) = 0;
    virtual QString payFiscalDocument(double sum, payType type = payTypeCash) = 0;
    virtual QString cancelFiscalDocument() = 0;
    virtual QString setOperator(QString shortName, QString vatNumber) = 0 ;
    virtual QString lastError() = 0 ;
    virtual QString lastDocumet() = 0;


    // передача хеша с видами оплат

    virtual QString closeFiscalDocument() = 0;
    //  передача


signals:

public slots:
};
}
#endif // ABSTRACTDEVFISCALOBJECT_H
