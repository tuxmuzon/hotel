#include "bookingdialog.h"
#include <QGridLayout>
#include "QLabel"
#include "core/esqlquery.h"
#include "QSqlRecord"
#include "QSqlError"
#include "QDebug"
#include "QPushButton"
#include "hotel/personselectdialog.h"
#include "hotel/profileedit.h"
#include "hotel/pay/paydocshow.h"
#include "hotel/pay/paydialog.h"

BookingDialog::BookingDialog(int bookingId, QDate startDate, int appartId, QWidget *parent)  : QDialog (parent)
{
    appartamentId= appartId;
    currentId = bookingId;
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *guestLayout = new QHBoxLayout();
    guestInfo = new QLabel;
    //    guestInfo->setEditable(false);
    guestLayout->addWidget( new QLabel("Гость:"));
    guestLayout->addWidget(guestInfo);

    QPushButton *guestChangeButton = new QPushButton("Выбрать");
    connect(guestChangeButton, &QPushButton::clicked,[&](){
        PersonSelectDialog *dialog = new PersonSelectDialog(-1,this);
        if(dialog->exec() == QDialog::Accepted){
            profileId = dialog->personId;
            guestUpdate();
        }
        delete dialog;
    });

    QPushButton *guestEditButton = new QPushButton("Изменить/Добавить");
    connect(guestEditButton, &QPushButton::clicked,[&](){
        profileEdit dialog(profileId);
        if(dialog.exec()== QDialog::Accepted){
            profileId = dialog.currentId;
            guestUpdate();
        }
    });

    QPushButton *guestClearButton = new QPushButton("Очистить");
    connect(guestClearButton, &QPushButton::clicked,[&](){
        profileId = 0;
        guestInfo->clear();
    });

    guestLayout->addWidget(guestChangeButton);
    guestLayout->addWidget(guestEditButton);
    guestLayout->addWidget(guestClearButton);

    mainLayout->addLayout(guestLayout);

    //ЛИНИЯ ДЛЯ РАЗДЕЛЕНИЯ ЗОН
    QWidget *horizontalLineWidget = new QWidget(this);
    horizontalLineWidget->setFixedHeight(2);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: #c0c0c0;"));
    mainLayout->addWidget(horizontalLineWidget);

    QGridLayout *infoLayout = new QGridLayout();
    accessUserCombo = new ESqlComboBox;
    accessUserCombo->setQuery("SELECT id, shortname FROM access_users");
    startDateEdit = new QDateEdit;
    startDateEdit->setCalendarPopup(true);

    stopDateEdit = new QDateEdit;
    stopDateEdit->setCalendarPopup(true);

    appartamentCombo = new ESqlComboBox;

    commentEdit = new QTextEdit;
    advertisingSourceCombo = new ESqlComboBox;
    advertisingSourceCombo->setQuery("SELECT id, shortname FROM advertising_source ORDER BY id");

    statusCombo = new ESqlComboBox();
    statusCombo->setQuery("SELECT id, shortname FROM booking_status");

    bookingType = new ESqlComboBox();
    bookingType->setQuery("VALUES(0,'Одиночная') UNION VALUES(1,'Груповая');");

    infoLayout->addWidget(new QLabel("Выставлено:"), 0, 0);
    infoLayout->addWidget(accessUserCombo, 0, 1);
    infoLayout->addWidget(new QLabel("Дата ОТ:"), 1, 0);
    infoLayout->addWidget(startDateEdit, 1, 1);
    infoLayout->addWidget(new QLabel("Дата ДО:"), 2, 0);
    infoLayout->addWidget(stopDateEdit, 2, 1);
    infoLayout->addWidget(new QLabel("Помещение:"), 3, 0);
    infoLayout->addWidget(appartamentCombo, 3, 1);
    infoLayout->addWidget(new QLabel("Статус:"), 4, 0);
    infoLayout->addWidget(statusCombo, 4, 1);
    infoLayout->addWidget(new QLabel("Комментарий:"), 5, 0);
    infoLayout->addWidget(commentEdit, 5, 1);
    infoLayout->addWidget(new QLabel("Рекламный источник:"), 6, 0);
    infoLayout->addWidget(advertisingSourceCombo, 6, 1);
    infoLayout->addWidget(new QLabel("Тип Брони:"), 7, 0);
    infoLayout->addWidget(bookingType, 7, 1);

    QGridLayout *costLayout = new QGridLayout();
    costLable = new QLineEdit;
    costLable->setReadOnly(true);

    discontBox = new QSpinBox;
    //размер скидки, если больше то только с подтяжкой и профиля
    discontBox->setRange(0,10);
    connect(discontBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&BookingDialog::afterUpdateAppartement);

    discontMoneyBox = new QSpinBox;
    discontMoneyBox->setRange(0,10000);
    connect(discontMoneyBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&BookingDialog::afterUpdateAppartement);

    discontMoneyBox->setDisabled(true);

    totalLable = new QLineEdit;
    totalLable->setReadOnly(true);
    paydLable = new QLineEdit;
    paydLable->setReadOnly(true);
    QPushButton *saveButton = new QPushButton(QIcon(":/img/save.png"),"Сохранить");
    QPushButton *payButton = new QPushButton(QIcon(":/img/pay.png"),"Оплатить");
    QPushButton *payDocsButton = new QPushButton(QIcon(":/img/pay.png"),"Чеки");
    QPushButton *printProfileButton = new QPushButton(QIcon(":/img/print.png"),"Печать Анкеты");
    QPushButton *printInvoiceButton = new QPushButton(QIcon(":/img/print.png"),"Печать Счет");

    connect(statusCombo, &ESqlComboBox::currentIdChanged, this,[=](const int id){
        if(id==9) {
            if(paydLable->text()!="0,00 ₽")
            {
                statusCombo->setId(status);
            }
        }
        if(id==8)
            payButton->setEnabled(false);
        else
            payButton->setEnabled(true);
    } );

    connect(payDocsButton, &QPushButton::clicked, this,[=](){
        payDocShow *dialog = new payDocShow(this, currentId);
        dialog->exec();
        delete dialog;
    } );

    connect(printProfileButton,&QPushButton::clicked,this,&BookingDialog::printProfile);
    connect(printInvoiceButton,&QPushButton::clicked,this,&BookingDialog::printInvoice);
    connect(saveButton,&QPushButton::clicked,this,&BookingDialog::save);

    costLayout->addWidget(new QLabel("Цена:"), 0, 0);
    costLayout->addWidget(costLable, 0, 1);
    costLayout->addWidget(new QLabel("Скидка %:"), 1, 0);
    costLayout->addWidget(discontBox, 1, 1);
    costLayout->addWidget(new QLabel("Скидка В валюте:"), 2, 0);
    costLayout->addWidget(discontMoneyBox, 2, 1);

    costLayout->addWidget(new QLabel("К оплате:"), 3, 0);
    costLayout->addWidget(totalLable, 3, 1);
    costLayout->addWidget(new QLabel("Оплачено:"), 4, 0);
    costLayout->addWidget(paydLable, 4, 1);
    costLayout->addWidget(saveButton, 6, 0, 1,2);
    costLayout->addWidget(payDocsButton, 7, 0, 1,2);
    costLayout->addWidget(payButton, 8, 0, 1,2);
    costLayout->addWidget(printProfileButton, 9, 0, 1,2);
    costLayout->addWidget(printInvoiceButton, 10, 0, 1,2);

    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addLayout(infoLayout);
    downLayout->addWidget(horizontalLineWidget);
    downLayout->addLayout(costLayout);
    mainLayout->addLayout(downLayout);

    connect(payButton,&QPushButton::clicked, this, &BookingDialog::pay);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox,&QDialogButtonBox::accepted,this,&BookingDialog::accept);
    connect(buttonBox,&QDialogButtonBox::rejected, this,&BookingDialog::reject);

    mainLayout->addWidget(buttonBox);


    setLayout(mainLayout);
    setWindowTitle("Бронирование");
    setWindowIcon(QIcon(":/img/booking.png"));

    startDateEdit->setDate(startDate);
    stopDateEdit->setDate(startDate.addDays(1));

    if(currentId>0) {
        update();
        afterUpdateDate();
    }
    else {
        afterUpdateDate();
        afterUpdateAppartement();
        paydLable->setText("0");
    }

    connect(appartamentCombo, &ESqlComboBox::currentIdChanged , this, &BookingDialog::afterUpdateAppartement);
    connect(startDateEdit, &QDateEdit::dateChanged, this, [=](){ afterUpdateAppartement();});
    connect(stopDateEdit, &QDateEdit::dateChanged, this,  [=](){ afterUpdateAppartement();});

    if (QDate::currentDate()>stopDateEdit->date().addDays(1)){
        startDateEdit->setDisabled(true);
        stopDateEdit->setDisabled(true);
    }
}

void BookingDialog::setProfileId(int profileId)
{
    this->profileId = profileId;
    guestUpdate();
}

void BookingDialog::update()
{
    ESqlQuery query;
    query.prepare("SELECT personsProfileID, apartment_id, accessUser_id, advertising_source_id, start_date, stop_date, "
                  "cost::numeric::float8, discont, total::numeric::float8, payd::numeric::float8, "
                  "comment, booking_status, discont_money::numeric::float8, type FROM bookings WHERE id = ? ");

    query.addBindValue(currentId);
    query.exec();
    while(query.next())
    {
        QSqlRecord rec = query.record();
        profileId=query.value(rec.indexOf("personsProfileID")).toInt();
        guestUpdate();

        accessUserCombo->setId(query.value(rec.indexOf("accessUser_id")).toInt());
        advertisingSourceCombo->setCurrentIndex(query.value(rec.indexOf("advertising_source_id")).toInt());
        startDateEdit->setDate(query.value(rec.indexOf("start_date")).toDate());
        stopDateEdit->setDate(query.value(rec.indexOf("stop_date")).toDate());
        appartamentId = query.value(rec.indexOf("apartment_id")).toInt();
        appartamentCombo->setId(appartamentId);
        costLable->setText(query.value(rec.indexOf("cost")).toString());
        discontBox->setValue(query.value(rec.indexOf("discont")).toInt());
        totalLable->setText(query.value(rec.indexOf("total")).toString());
        paydLable->setText(query.value(rec.indexOf("payd")).toString());
        commentEdit->setText(query.value(rec.indexOf("comment")).toString());
        status = query.value(rec.indexOf("booking_status")).toInt();
        discontMoneyBox->setValue(query.value(rec.indexOf("discont_money")).toInt());
        statusCombo->setId(status);
        bookingType->setId(query.value(rec.indexOf("type")).toInt());
    }
}

void BookingDialog::afterUpdateDate()
{
    // пока вот такйо гемарой
    QString subQuery;
    subQuery ="SELECT array_agg(apartments.path) FROM bookings "
              "LEFT JOIN apartments ON apartments.id=bookings.apartment_id "
              "WHERE '" + startDateEdit->date().toString(Qt::DateFormat::ISODate) +
            "'::date BETWEEN start_date AND stop_date - interval '1 day'" ;
    if(currentId>0)
        subQuery.append("AND bookings.id<>" + QString::number(currentId));

    QString allQuery;
    allQuery = "SELECT  apartments.id, shortname FROM apartments WHERE id<>1 OR id="+ QString::number(appartamentId) +
            " AND id NOT IN "
            "(SELECT apartments.id FROM apartments WHERE path @> "
            "("+ subQuery + ")"
                            "UNION "
                            "SELECT apartments.id FROM apartments WHERE path <@ "
                            "("+ subQuery + ")"
                                            ") ";

    appartamentCombo->setQuery(allQuery);
    appartamentCombo->setId(appartamentId);
}

void BookingDialog::afterUpdateAppartement()
{
    costLable->setText("");

    int bookingDays = static_cast<int>(stopDateEdit->date().toJulianDay()-startDateEdit->date().toJulianDay()) ;
    appartamentId=appartamentCombo->id();

    ESqlQuery query;
    query.prepare("SELECT cost, cost*?/100*?-? FROM prices WHERE (current_Date BETWEEN start_date AND stop_date) "
                  "AND for_children=false AND apartment_id = ?");
    query.addBindValue(bookingDays);
    query.addBindValue(discontBox->value()==0?100:(100-discontBox->value()));
    query.addBindValue(discontMoneyBox->value());
    query.addBindValue(appartamentId);

    query.exec() && query.next();
    if(query.size()==0)
    {
        ESqlQuery parentFind;
        parentFind.prepare("SELECT parent_id FROM apartments WHERE id = ?;");
        parentFind.addBindValue(appartamentId);
        if(parentFind.exec() && parentFind.next()){

            query.prepare("SELECT cost,cost*?/100*?-? FROM prices WHERE (current_Date BETWEEN start_date AND stop_date) "
                          "AND for_children=true AND apartment_id = ?");
            query.addBindValue(bookingDays);
            query.addBindValue(discontBox->value()==0?100:(100-discontBox->value()));
            query.addBindValue(discontMoneyBox->value());
            query.addBindValue(parentFind.value(0).toInt());
            query.exec() && query.next();
        }
    }
    if(isVip)
    {
        costLable->setText(QString::number(vipCost));
        totalLable->setText((QString::number(vipCost*bookingDays)));
    }
    else {
        costLable->setText(query.value(0).toString());
        totalLable->setText(query.value(1).toString());
    }
}

void BookingDialog::accept()
{
    save();
    close();
}

void BookingDialog::printProfile()
{
    QDialog *dialog = new QDialog(this);

    QVBoxLayout *layout = new QVBoxLayout;
    QWebEngineView *view = new QWebEngineView(this);
    QByteArray htmlText;

    QString data;
    QString fileName(":/templates/bookinProfile.html");

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;

    data = file.readAll();
    file.close();

    ESqlQuery query;
    query.prepare("SELECT firstname, secondname, lastname, mobile_phone, email, to_char(beathday,'DD.MM.YYYY') as beathday, CASE WHEN sex='0' THEN 'М' ELSE 'Ж' END AS sex, beath_country, beath_place, doc_type, "
                  "  doc_serial, doc_number, to_char(doc_create_date,'DD.MM.YYYY') as doc_create_date, doc_create_org, doc_create_org_code, resident_country, address,"
                  "  to_char(bookings.start_date, 'DD.MM.YYYY') as start_date, CASE WHEN booking_status='6' THEN to_char(bookings.stop_date, 'DD.MM.YYYY') ELSE '_____' END as stop_date, bookings.cost, bookings.discont, bookings.total, bookings.payd, "
                  "  apartments.shortname as appartamento, access_users.shortname as admin_name "
                  "FROM persons_profiles "
                  "LEFT JOIN bookings ON bookings.personsProfileID = persons_profiles.id "
                  "LEFT JOIN apartments ON bookings.apartment_id = apartments.id "
                  "LEFT JOIN access_users ON access_users.id=bookings.accessUser_id "
                  "WHERE bookings.id = ?;"
                  );
    query.addBindValue(currentId);

    if (query.exec() && query.next())
    {
        for( int i = 0; i < query.record().count(); i ++)
            data.replace("[" + query.record().fieldName(i).toUtf8() + "]", query.value(i).toString());

        view->setHtml(data);

        layout->addWidget(view);

        QHBoxLayout *buttonLayout = new QHBoxLayout;

        QPushButton *printTemplate = new QPushButton("Печать на принтер");
        QPushButton *htmlTemplate = new QPushButton("Сохранить в HTML");
        QPushButton *pdfTemplate = new QPushButton("Сохранить в pdf");

        connect(htmlTemplate, &QPushButton::clicked, [&](){
            QString htmlFileName = QFileDialog::getSaveFileName(this, tr("Выберите существующий файл или введите название нового"),
                                                            "profile.html", tr("Файлы HTML (*.html)"));
            if(htmlFileName.isEmpty())
                return;

            QFile fileHtml(htmlFileName);
            if(!fileHtml.open(QIODevice::WriteOnly| QIODevice::Text))
                return;

            QTextStream out(&fileHtml);
            out << data;
            fileHtml.close();

        });

        connect(printTemplate, &QPushButton::clicked, [&view](){
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
        buttonLayout->addWidget(htmlTemplate);
        buttonLayout->addWidget(printTemplate);
        layout->addLayout(buttonLayout);
        dialog->setLayout(layout);
        dialog->setMinimumSize(1000, 700);
        dialog->exec();

        delete dialog;
    }
}

void BookingDialog::printInvoice()
{  
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
                  "  bookings.total, bookings.cost, bookings.payd, bookings.cost*?-bookings.total as discont,"
                  "  apartments.shortname as appart_name, access_users.shortname as user_name "
                  "FROM persons_profiles "
                  "LEFT JOIN bookings ON bookings.personsProfileID = persons_profiles.id "
                  "LEFT JOIN apartments ON bookings.apartment_id = apartments.id "
                  "LEFT JOIN access_users ON access_users.id=bookings.accessUser_id "

                  "WHERE bookings.id = ?;"
                  );
    query.addBindValue(static_cast<int>(stopDateEdit->date().toJulianDay()-startDateEdit->date().toJulianDay()));
    query.addBindValue(currentId);

    if (query.exec() && query.next())
    {
        for( int i = 0; i < query.record().count(); i ++)
            data.replace("[" + query.record().fieldName(i).toUtf8() + "]", query.value(i).toString());
        QSqlRecord rec = query.record();
        //      int bookingDays = static_cast<int>(stopDateEdit->date().toJulianDay()-startDateEdit->date().toJulianDay()) ;


        view->setHtml(data);

        layout->addWidget(view);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *htmlTemplate = new QPushButton("Сохранить в HTML");
        QPushButton *printTemplate = new QPushButton("Печать на принтер");
        QPushButton *pdfTemplate = new QPushButton("Сохранить в pdf");


        connect(htmlTemplate, &QPushButton::clicked, [&](){
            QString fileName = QFileDialog::getSaveFileName(this, tr("Выберите существующий файл или введите название нового"),
                                                            "invoice.html", tr("Файлы HTML (*.html)"));
            if(fileName.isEmpty())
                return;

            QFile fileHtml(fileName);
            if(!fileHtml.open(QIODevice::WriteOnly| QIODevice::Text))
                return;

            QTextStream out(&fileHtml);
            out << data;
            fileHtml.close();

        });


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
        buttonLayout->addWidget(htmlTemplate);
        buttonLayout->addWidget(printTemplate);
        layout->addLayout(buttonLayout);
        dialog->setLayout(layout);
        dialog->setMinimumSize(1000, 700);
        dialog->exec();

        delete dialog;
    }
}

void BookingDialog::pay()
{
    PayDialog *dialog = new PayDialog(currentId,this);
    dialog->exec();

    delete dialog;
    this->update();
}

void BookingDialog::guestUpdate()
{
    ESqlQuery query;
    query.prepare("SELECT concat_ws(' ',firstname,secondname,lastname,mobile_phone), is_vip, vip_price::numeric::float8 FROM persons_profiles WHERE id=?");
    query.addBindValue(profileId);
    if(query.exec() && query.next()){
        guestInfo->setText(query.value(0).toString());
        isVip=query.value(1).toBool();
        vipCost=query.value(2).toDouble();
    }
    afterUpdateAppartement();
}

void BookingDialog::save()
{
    ESqlQuery query;
    if (currentId < 1)
    {
        query.prepare("INSERT INTO bookings(personsProfileID, apartment_id, accessUser_id, advertising_source_id, start_date, stop_date, cost, discont, "
                      "total, comment,booking_status,payd,discont_money,type) "
                      "VALUES (:personsProfileID,:apartment_id,:accessUser_id,:advertising_source_id,:start_date,:stop_date,:cost,:discont,:total,"
                      ":comment,:booking_status,0,:discont_money,:booking_type) RETURNING id" );
    } else
    {
        query.prepare("UPDATE bookings SET "
                      "personsProfileID = :personsProfileID, "
                      "apartment_id = :apartment_id, "
                      "accessUser_id = :accessUser_id, "
                      "advertising_source_id = :advertising_source_id, "
                      "start_date = :start_date, "
                      "stop_date = :stop_date, "
                      "cost = :cost, "
                      "discont = :discont, "
                      "total = :total, "
                      "comment = :comment, "
                      "booking_status = :booking_status, "
                      "discont_money = :discont_money, "
                      "type = :booking_type "
                      "WHERE id =:ID ");
        query.bindValue(":ID", currentId);
    }
    QRegExp space("\\s");
    query.bindValue(":personsProfileID", profileId);
    query.bindValue(":apartment_id",appartamentId);
    query.bindValue(":accessUser_id",accessUserCombo->id());
    query.bindValue(":advertising_source_id", advertisingSourceCombo->id());
    query.bindValue(":start_date",startDateEdit->date());
    query.bindValue(":stop_date",stopDateEdit->date());
    query.bindValue(":cost",costLable->text().replace(" ₽","").remove(space));
    query.bindValue(":discont",discontBox->value());
    query.bindValue(":total",totalLable->text().replace(" ₽","").remove(space));
    query.bindValue(":comment",commentEdit->toPlainText());
    query.bindValue(":booking_status",statusCombo->id());
    query.bindValue(":discont_money",discontMoneyBox->value());
    query.bindValue(":booking_type",bookingType->id());
    if(!query.exec()){
        QMessageBox::warning(this,"Ошибка сохранения", query.lastError().text());
    }
    else
    {
        QMessageBox::information(this,"Успешно сохранено", query.lastError().text());
        if (currentId < 1)
        {
            query.next();
            currentId = query.value(0).toInt();
        }
    }
}
