#include "paydialog.h"
#include "QVBoxLayout"
#include "QLabel"
#include "QDebug"
#include "QModelIndex"
#include "QAbstractItemModel"
#include "QGroupBox"
#include "../core/esqlquery.h"
#include "QInputDialog"
#include "core/ecoreobject.h"
#include "QString"


PayDialog::PayDialog(int bookingId, QWidget *parent)
{
    QGroupBox *saleBox = new QGroupBox(tr("Вид операции"));
    QHBoxLayout *saleBoxLayout = new QHBoxLayout;
    sale = new QRadioButton("Продажа");
    sale->setChecked(true);
    sale->setIcon(QIcon(":/img/sale.png"));
    saleBoxLayout->addWidget(sale);
    saleReturn = new QRadioButton("Возврат");
    saleReturn->setIcon(QIcon(":/img/return_sale.png"));
    saleBoxLayout->addWidget(saleReturn);
    saleBox->setLayout(saleBoxLayout);

    connect(sale,&QRadioButton::clicked,this,&PayDialog::update);
    connect(saleReturn,&QRadioButton::clicked,this,&PayDialog::update);

    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    payDocument = new QTableView();

    mainLayout->addWidget(saleBox);
    mainLayout->addWidget(payDocument);

    cash = new QRadioButton("Наличные");
    cash->setIcon(QIcon(":/img/cash.png"));
    cash->setChecked(true);
    card = new QRadioButton("Карта");
    card->setIcon(QIcon(":/img/card.png"));
    inCard = new QRadioButton("На карту");
    inCard->setIcon(QIcon(":/img/card.png"));
    QGroupBox *payBox = new QGroupBox(tr("Вид оплаты"));
    QHBoxLayout *payBoxLayout = new QHBoxLayout;
    payBoxLayout->addWidget(cash);
    payBoxLayout->addWidget(card);
    card->setEnabled(false);
    payBoxLayout->addWidget(inCard);
    payBox->setLayout(payBoxLayout);

    mainLayout->addWidget(payBox);

    totalLable = new QLineEdit();
    totalLable->setReadOnly(true);
    paydLable = new QLineEdit();
    paydLable->setReadOnly(true);
    inCashEdit = new QSpinBox;
    inCashEdit->setRange(1, 30000);
    outCashLable = new QLineEdit();
    outCashLable->setReadOnly(true);

    QGridLayout *moneyLayout = new QGridLayout();
    moneyLayout->addWidget(new QLabel("Итого:"), 0, 0);
    moneyLayout->addWidget(totalLable, 0, 1);
    moneyLayout->addWidget(new QLabel("Оплатить:"), 1, 0);
    moneyLayout->addWidget(paydLable, 1, 1);
    moneyLayout->addWidget(new QLabel("Внесено:"), 3, 0);
    moneyLayout->addWidget(inCashEdit, 3, 1);
    moneyLayout->addWidget(new QLabel("Сдача:"), 4, 0);
    moneyLayout->addWidget(outCashLable, 4, 1);

    mainLayout->addLayout(moneyLayout);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PayDialog::slot_accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PayDialog::slot_reject);

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    setWindowTitle("Оплата чека");

    currentBookingId = bookingId;
    update();
    fiscal = new atoldev9fiscalobject();
    fiscal->setOperator(ECoreObject::instance()->userPrintTitle(), ECoreObject::instance()->userVatNum());
}

PayDialog::~PayDialog() {
    qDebug() << "destroy";
    delete  fiscal;
}

void PayDialog::update(){
    ESqlQuery query;

    query.prepare("SELECT apartments.shortname, to_char(bookings.start_date,'DD.MM.YYYY'),to_char(bookings.stop_date,'DD.MM.YYYY'), "
                  "cost::numeric::float8, discont,total::numeric::float8,  "
                  "extract(days from (bookings.stop_date - bookings.start_date)) as count, "
                  "payd::numeric::float8, personsprofileid  FROM bookings "
                  "LEFT JOIN apartments ON apartments.id=bookings.apartment_id "
                  "WHERE bookings.id=?");
    query.addBindValue(currentBookingId);
    query.exec();
    if( query.next())
    {
        //       str.goodName=query.value(0).toString()+" C " + query.value(1).toString() + " ПО " + query.value(2).toString();

        total = inCashEdit->value();

        if (sale->isChecked())
            payd = query.value(5).toDouble() - query.value(7).toDouble();
        else
            payd = query.value(7).toDouble();

        personProfileId =  query.value(8).toInt();
    }

    totalLable->setText(QString::number(total));
    paydLable->setText(QString::number(payd));
    inCashEdit->setValue(static_cast<int>(payd));

}

void PayDialog::slot_accept() {
    if (payd>0) {
        str.goodName="Услуги проживания";
        str.goodPrice =static_cast<double>(inCashEdit->value());

        str.goodCount =1;

        if (!inCard->isChecked())
        {
            if(sale->isChecked())
                if(fiscal ->openFiscalDocument(operationType::sale).contains("Ошибка"))
                {
                    QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                    this->slot_reject();
                }
            if(saleReturn->isChecked())
                if(fiscal ->openFiscalDocument(operationType::returnSale).contains("Ошибка"))
                {
                    QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                    this->slot_reject();
                }

            if(fiscal->printFiscalString(str).contains("Ошибка"))
            {
                QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                this->slot_reject();
            }

            if(cash->isChecked())
                if(fiscal->payFiscalDocument(inCashEdit->value(), payType::payTypeCash).contains("Ошибка"))
                {
                    QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                    this->slot_reject();
                }
            if(card->isChecked())
                if(fiscal->payFiscalDocument(inCashEdit->value(), payType::payTypeElectronic).contains("Ошибка"))
                {
                    QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                    this->slot_reject();
                }

            if(fiscal->closeFiscalDocument().contains("Ошибка"))
            {
                QMessageBox::critical(this,"Ошибка выполенния", fiscal->lastError());
                this->slot_reject();
            }
        }

        ESqlQuery query;
        query.prepare("UPDATE bookings set payd=(payd::money+?::money) WHERE bookings.id=?");
        if(sale->isChecked())
            query.addBindValue(inCashEdit->value());
        else if(saleReturn->isChecked())
            query.addBindValue(-inCashEdit->value());

        query.addBindValue(currentBookingId);

        if(!query.exec()&&query.next()){
            QMessageBox::information(this,"Ошибка выполенния", "Ошибка внесения платежа в бронь");
            this->slot_reject();
        }

        if(!savePayDocument()){
            QMessageBox::information(this,"Ошибка выполенния", "Ошибка соханения чека в БД");
            this->slot_reject();
        }

        sale->setChecked(true);
    }
    accept();
}

void PayDialog::slot_reject() {
    reject();
}

bool PayDialog::savePayDocument()
{
    ESqlQuery  query;
    query.prepare("INSERT INTO pay_documents(document_number,personsprofileid, bookings_id, accessUser_id, count, cost, operation_type, cash_id, shift,comment,payType) "
                  "VALUES (?,?,?,?,?,?,?,?,?,?,?);");
    query.addBindValue(fiscal->lastDocumet().toInt());
    query.addBindValue(personProfileId);
    query.addBindValue(currentBookingId);
    query.addBindValue(ECoreObject::instance()->userId());
    query.addBindValue(str.goodCount);
    query.addBindValue(sale->isChecked()?str.goodPrice:-str.goodPrice);
    query.addBindValue(sale->isChecked()?operationType::sale:operationType::returnSale);
    query.addBindValue(1);
    query.addBindValue(fiscal->getShiftNumber().toInt());
    query.addBindValue(fiscal->lastError());
    query.addBindValue(cash->isChecked()?0:(card->isChecked()?1:2));
    if(query.exec())
        return true;
    else
        return false;
}
