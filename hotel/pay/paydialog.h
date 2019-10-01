#ifndef PAYDIALOG_H
#define PAYDIALOG_H


#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include "QDialogButtonBox"
#include "QMessageBox"
#include "QTableView"
#include "QSqlTableModel"
#include "QRadioButton"
#include "QSpinBox"
#include "../fiscal/abstractdevfiscalobject.h"
#include "../fiscal/atoldev9fiscalobject.h"

class PayDialog : public  QDialog
{
    Q_OBJECT
public:
    explicit PayDialog(int bookingId = -1 ,QWidget *parent = nullptr);

    ~PayDialog();
    int personId;

public slots:

    void update();
    void slot_accept();
    void slot_reject();

private:

    bool savePayDocument();
    fiscalSpace::FISCALSTRING str;
    AbstractDevFiscalObject *fiscal;
    QRadioButton *sale;
    QRadioButton *saleReturn;

    QTableView *payDocument;

    QRadioButton *cash;
    QRadioButton *card;
    QRadioButton *inCard;
    QRadioButton *bank;


    QSqlTableModel *personsModel;
    QDialogButtonBox *buttonBox;


    QLineEdit *totalLable, *paydLable, *outCashLable;
    QSpinBox *inCashEdit;
    int currentBookingId =-1;
    double total;
    double payd;
    int personProfileId=0;
};

#endif // PERSONSELECTDIALOG_H
