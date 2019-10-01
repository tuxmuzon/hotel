#ifndef PROFILEEDIT_H
#define PROFILEEDIT_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include "QDialogButtonBox"
#include "QMessageBox"
#include "QCheckBox"
#include "QSpinBox"

class profileEdit : public  QDialog
{
    Q_OBJECT
public:
    explicit profileEdit(int prodileId = -1 ,QWidget *parent = nullptr);
    int currentId;

signals:

public slots:
    void update();
    void slot_accept();

private:


    QLineEdit *firstName, *secondName, *lastName;
    QDateEdit *beathDay;
    //, *docCreateDay;
    QLineEdit *beathPlace;
    QLineEdit *citizenship;
    QComboBox *sex;
    QLineEdit *beathCountry, *residentCountry, *address;
    QLineEdit *docType, *docSerial, *docNumber, *docCreateOrg, *docCreateOrgCode;
    QDateEdit *docCreateDate;
    //, *docCreateOrg;
    QComboBox *visitDocType;
    QLineEdit *visitDocSeries, *visitDocNumber;
    QLineEdit *eMail, *mobilePhone;

    QCheckBox *isVip;
    QSpinBox *vipPrice;

    QDialogButtonBox *buttonBox;


};

#endif // PROFILEEDIT_H
