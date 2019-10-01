#include "cashservicedialog.h"
#include "QWidget"
#include "QGridLayout"
#include "QMessageBox"
#include "QDebug"
#include "QInputDialog"
#include "../core/ecoreobject.h"

CashServiceDialog::CashServiceDialog(QWidget *parent) : QDialog (parent)
{
    fiscal = new atoldev9fiscalobject();
    fiscal->setOperator(ECoreObject::instance()->userPrintTitle(), ECoreObject::instance()->userVatNum());

    openShift = new QPushButton("Открытие смены");
    openShift->setMinimumHeight(50);
    zReport = new QPushButton("Снять Z отчет(Закрыть)");
    zReport->setMinimumHeight(50);
    xReport = new QPushButton("Снять X отчет(Без закрытия)");
    xReport ->setMinimumHeight(50);
    inCash = new QPushButton("Внесение наличных");
    inCash ->setMinimumHeight(50);
    outCash = new QPushButton("Выдача наличных");
    outCash ->setMinimumHeight(50);
    cancel = new QPushButton("Отмена чека");
    cancel ->setMinimumHeight(50);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(openShift,0,0);
    gridLayout->addWidget(zReport,0,1);
    gridLayout->addWidget(xReport,0,2);
    gridLayout->addWidget(inCash,1,0);
    gridLayout->addWidget(outCash,1,1);
    gridLayout->addWidget(cancel,1,2);
    setLayout(gridLayout);
    setWindowTitle("Сервисные операции");
    setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

    connect(openShift,&QPushButton::clicked,this,&CashServiceDialog::runOpenShift);
    connect(zReport,&QPushButton::clicked,this,&CashServiceDialog::runZReport);
    connect(xReport,&QPushButton::clicked,this,&CashServiceDialog::runXReport);
    connect(inCash,&QPushButton::clicked,this,&CashServiceDialog::runInCash);
    connect(outCash,&QPushButton::clicked,this,&CashServiceDialog::runOutCash);
    connect(cancel,&QPushButton::clicked,this,&CashServiceDialog::cancelDoc);
}

CashServiceDialog::~CashServiceDialog()
{
    delete fiscal;
}

void CashServiceDialog::runOpenShift()
{
    //TODO, не запилено, но надо по ФЗ 54.
    fiscal->openShift();
    close();
}

void CashServiceDialog::runZReport()
{
    exeption(fiscal->closeShift());
    close();
}

void CashServiceDialog::runXReport()
{
    exeption(fiscal->printXReport());
    close();
}

void CashServiceDialog::runInCash()
{
    bool ok;
    double sum = QInputDialog::getDouble(this, tr("Внесение"),
                                         tr("Введите сумму (максимум 40000):"),0,0,
                                         40000,2, &ok);
    if (ok && sum>0)
        exeption(fiscal->openFiscalDocument(operationType::inCash, sum));
    close();
}

void CashServiceDialog::runOutCash()
{
    bool ok;
    double sum = QInputDialog::getDouble(this, tr("Выдача"),
                                         tr("Введите сумму (максимум 40000):"),0,0,
                                         40000,2, &ok);
    if (ok && sum>0)
        exeption(fiscal->openFiscalDocument(operationType::outCash, sum));
    close();
}

void CashServiceDialog::cancelDoc()
{
    exeption(fiscal->cancelFiscalDocument());
    close();
}

void CashServiceDialog::exeption(QString returnString)
{
    qDebug() << "return STRING: " << returnString;
    if(returnString.contains("Ошибка ФР:"))
        QMessageBox::critical(Q_NULLPTR,"Ошибка выполенния", returnString);
    else
        QMessageBox::information(Q_NULLPTR,"Успешно", "Операция Успешна");

}
