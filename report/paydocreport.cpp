#include "paydocreport.h"
#include "QVBoxLayout"
#include "core/esqlquery.h"
#include "QDebug"
#include "QDialog"
#include "QWebEngineView"
#include "QFile"
#include "QPushButton"
#include "QPrinter"
#include "QPrintDialog"
#include "QFileDialog"


PayDocReport::PayDocReport(QWidget *parent)
{
    table = new QTableView(parent);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout * mainLayout = new  QVBoxLayout();

    mainLayout->addWidget(table);
    setLayout(mainLayout);
    actualze();
    connect(table,&QTableView::doubleClicked,this,&PayDocReport::print);
}

void PayDocReport::setBooking(int bookingId)
{
    filter=" AND bookings_id="+QString::number(bookingId);
    actualze();
}

void PayDocReport::actualze()
{
    // по сумме добавить минус возврата
    QSqlQueryModel *model = new  QSqlQueryModel();
    model->setQuery("SELECT pay_documents.id, access_users.shortname, pay_documents.created, "
                    "CASE WHEN nomenclature_id IS NULL THEN 'Проживание' ELSE nomenclature.shortname END, pay_documents.cost, "
                    "CASE WHEN payType=0 THEN 'Наличный' ELSE CASE WHEN payType=1 THEN 'Картой' ELSE 'На карту' END END, "
                    "CASE WHEN operation_type=1 THEN 'Продажа' ELSE 'Возврат' END FROM pay_documents "
                    "LEFT JOIN access_users ON access_users.id=accessUser_id "
                    "LEFT JOIN nomenclature ON nomenclature.id=nomenclature_id "
                    "WHERE true" + filter + " ORDER BY pay_documents.created DESC");

    model->setHeaderData(0, Qt::Horizontal, tr("Номер"));
    model->setHeaderData(1, Qt::Horizontal, tr("Кассир"));
    model->setHeaderData(2, Qt::Horizontal, tr("Дата"));
    model->setHeaderData(3, Qt::Horizontal, tr("Назначение"));
    model->setHeaderData(4, Qt::Horizontal, tr("Сумма"));
    model->setHeaderData(5, Qt::Horizontal, tr("Вид оплаты"));
    model->setHeaderData(6, Qt::Horizontal, tr("Вид операции"));

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setModel(model);
    table->resizeColumnsToContents();
    table->setSortingEnabled(true);
}

void PayDocReport::print(const QModelIndex &index)
{
    int currentDocId=0;
    qDebug() <<  "printInvoiceDOC";
    QDialog *dialog = new QDialog(this);

    QVBoxLayout *layout = new QVBoxLayout;
    QWebEngineView *view = new QWebEngineView(this);
    QByteArray htmlText;

    QString data;
    QString fileName(":/templates/invoice.html");

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
       qDebug() << file.errorString();
        return;
    }

    data = file.readAll();
    file.close();

    ESqlQuery query;
    query.prepare("SELECT bookings.id as doc_number, concat_ws(' ',firstname, secondname, lastname) as fio,"
                  " to_char(bookings.start_date, 'DD.MM.YYYY') as start_date, to_char(bookings.stop_date, 'DD.MM.YYYY') as stop_date, "
                  "  bookings.total, bookings.discont, bookings.cost, bookings.payd, "
                  "  apartments.shortname as appart_name, access_users.shortname as user_name "
                  "FROM persons_profiles "
                  "LEFT JOIN bookings ON bookings.personsProfileID = persons_profiles.id "
                  "LEFT JOIN apartments ON bookings.apartment_id = apartments.id "
                  "LEFT JOIN access_users ON access_users.id=bookings.accessUser_id "

                  "WHERE bookings.id = ?;"
                  );
    query.addBindValue(currentDocId);

    if (query.exec() && query.next())
    {
        for( int i = 0; i < query.record().count(); i ++)
            data.replace("[" + query.record().fieldName(i).toUtf8() + "]", query.value(i).toString());

        view->setHtml(data);

        layout->addWidget(view);

        QHBoxLayout *buttonLayout = new QHBoxLayout;

        QPushButton *printTemplate = new QPushButton("Печать на принтер");
        QPushButton *pdfTemplate = new QPushButton("Сохранить в pdf");

        connect(printTemplate, &QPushButton::clicked, view, [&view](){
            QPrinter *printer = new QPrinter(QPrinter::ScreenResolution);

            QPrintDialog* printDialog = new QPrintDialog(printer);
            if (printDialog->exec() == QDialog::Accepted) {
                view->page()->print(printer, [=](bool ok){
                    if (ok)
                        qDebug() << "Printed";
                    else
                        qDebug() << "Not printed";
                });
            }
            delete printDialog;
        });


        connect(pdfTemplate, &QPushButton::clicked, [&](){
            QString fileName = QFileDialog::getSaveFileName(this, tr("Выберите существующий файл или введите название нового"),
                                                            QString(), tr("Файлы PDF (*.pdf)"));
            if(fileName.isEmpty())
                return;
            view->page()->printToPdf(fileName);
        });

        buttonLayout->addWidget(pdfTemplate);
        buttonLayout->addWidget(printTemplate);
        layout->addLayout(buttonLayout);
        dialog->setLayout(layout);
        dialog->setMinimumSize(1000, 700);
        dialog->exec();

        delete dialog;
    }
}
