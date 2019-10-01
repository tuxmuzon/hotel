#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H


#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include "QDialogButtonBox"
#include "QMessageBox"
#include "QTextEdit"
#include "QSpinBox"
#include <QtWebEngine>
#include <QWebEngineView>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <core/esqlcombobox.h>

class BookingDialog : public  QDialog
{
    Q_OBJECT
public:
    explicit BookingDialog(int bookingId = 0, QDate startDate = QDate::currentDate(), int appartId = 0, QWidget *parent = nullptr);
    void setProfileId(int profileId);

signals:

public slots:
    void update();
    void afterUpdateDate();
    void afterUpdateAppartement();
    void accept();
    void printProfile();
    void printInvoice();
    void pay();
    

private:
    int currentId=-1;
    int profileId=0;
    int appartamentId=0;
    int status=0;

    QLabel *guestInfo;
    ESqlComboBox *accessUserCombo, *advertisingSourceCombo;
    ESqlComboBox *appartamentCombo, *statusCombo;
    ESqlComboBox *bookingType;
    QTextEdit *commentEdit;
    QDateEdit  *startDateEdit, *stopDateEdit;

    QLineEdit *costLable, *totalLable, *paydLable;
    QSpinBox *discontBox;
    QSpinBox *discontMoneyBox;

    QDialogButtonBox *buttonBox;
    bool isVip;
    double vipCost;


//    QDate startDate= QDate::currentDate();
    void guestUpdate();
private slots:
    void save();

};

#endif // BOOKINGDIALOG_H
