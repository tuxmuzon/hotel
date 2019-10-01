#ifndef CASHSERVICEDIALOG_H
#define CASHSERVICEDIALOG_H
#include "QDialog"
#include "QPushButton"
#include "fiscal/atoldev9fiscalobject.h"

class CashServiceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CashServiceDialog(QWidget* parent = Q_NULLPTR);
    ~CashServiceDialog();
    void  static exeption(QString returnString);
private:
    atoldev9fiscalobject *fiscal;
    QPushButton* openShift;
    QPushButton *zReport;
    QPushButton *xReport;
    QPushButton *inCash;
    QPushButton *outCash;
    QPushButton *cancel;
    void runOpenShift();
    void runZReport();
    void runXReport();
    void runInCash();
    void runOutCash();
    void cancelDoc();




};

#endif // CASHSERVICEDIALOG_H
