#ifndef ATOLDEV9FISCALOBJECT_H
#define ATOLDEV9FISCALOBJECT_H
#include "abstractdevfiscalobject.h"
using namespace fiscalSpace;
#include <QObject>
#include "QDebug"
// Заголовочные файлы ДТО9
#ifdef __linux__
#include "libfptr10.h"

#endif

class atoldev9fiscalobject : public AbstractDevFiscalObject
{
    Q_OBJECT
public:
    explicit atoldev9fiscalobject(QObject *parent = nullptr);
    // разрушаем обьект кассы
    ~atoldev9fiscalobject() override {
        qDebug() << "destroy";
        #ifndef Q_OS_MAC
        libfptr_destroy(&fptr);
#endif
    }

public:
    QString fiscalStatus() override;
    QString openFiscalDocument(operationType fiscalDoc,double sum=-1) override;
    QString openShift() override;
    QString closeShift() override;
    QString setAtributeSubjectCalculation() override;
    QString printFiscalString(FISCALSTRING str) override;
    QString printNoFiscalString(QString text) override;
    QString cancelFiscalDocument() override;
    QString closeFiscalDocument() override;
    QString payFiscalDocument(double sum, payType type) override;
    QString showProperties() override;
    QString printXReport() override;
    QString getShiftNumber() override;
    QString setOperator(QString p_shortName, QString p_vatNumber)  override;
    QString lastError() override;
    QString lastDocumet() override;

private:
    #ifndef Q_OS_MAC
    libfptr_handle fptr;
#endif
    wchar_t *strTowchar(QString str);


    // AbstractDevFiscalObject interface
public:


    // AbstractDevFiscalObject interface


};

#endif // ATOLDEV9FISCALOBJECT_H
