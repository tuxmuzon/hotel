#include "mainwindow.h"
#include "hotel/booking/bookingdialog.h"
#include "QWidget"
#include "hotel/booking/bookingpoint.h"
#include "mail_out/widgets/maillistwidget.h"
#include "fiscal/abstractdevfiscalobject.h"
#include "fiscal/atoldev9fiscalobject.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connectDB();

#ifdef QT_DEBUG
    ECoreObject::instance()->tryAuthorize("admin", "");
#else
    AuthDialog *authDialog = new AuthDialog;
    if (authDialog->exec() == QDialog::Rejected)
        exit(0);

#endif

    setWindowTitle(tr("Программа Q Отель"));
    setWindowIcon(QIcon(":/img/main.png"));

    BookingPoint *bookingWidget = new BookingPoint(this);

    personsTable = new QTableView(this);
    personsTable->selectRow(0);
    personsActualize();

    QLineEdit *filterEdit = new QLineEdit;

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Поиск: "));
    filterLayout->addWidget(filterEdit);

    QVBoxLayout *personsLayout = new QVBoxLayout(this);
    personsLayout->addLayout(filterLayout);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *editButton = new QPushButton("Редактировать");
    buttonLayout->addWidget(editButton);
    QPushButton *resetButton = new QPushButton("Сбросить");
    buttonLayout->addWidget(resetButton);
    QPushButton *addButton = new QPushButton("Добавить");
    buttonLayout->addWidget(addButton);
    QPushButton *dellButton = new QPushButton("Удалить");
    buttonLayout->addWidget(dellButton);
    personsLayout->addLayout(buttonLayout);
    personsLayout->addWidget(personsTable);

    personsPoint->setLayout(personsLayout);

    connect(filterEdit,&QLineEdit::textEdited, this, &MainWindow::personsActualize);

    connect(personsTable, &QTableWidget::doubleClicked, [=](const QModelIndex current){
        profileEdit dialog(personsTable->model()->data(personsTable->model()->index(current.row(),0)).toInt());
        dialog.exec();
    } );

    connect(editButton, &QPushButton::clicked, [=](){
        profileEdit dialog(personsTable->model()->data(personsTable->model()->index(personsTable->currentIndex().row(),0)).toInt());
        dialog.exec();
        personsModel->select();
    } );
    connect(resetButton, &QPushButton::clicked, [=](){
        filterEdit->setText("");
        personsModel->select();
    } );

    connect(addButton, &QPushButton::clicked, [=](){
        profileEdit dialog(-1);
        dialog.exec();
        personsModel->select();
    } );

    connect(dellButton, &QPushButton::clicked, [=](){
        QMessageBox msgBox;
        msgBox.setText("Действительно удалить Клиента?");
        msgBox.setInformativeText("Действительно удалить Клиента?, Если по клиенту пристввуют движения удаление невозможно");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int res = msgBox.exec();
        if (res == QMessageBox::Ok) //нажата кнопка Ok
            personsModel->removeRow(personsTable->currentIndex().row());
        else //отмена
            qWarning() << "Cancel";//  отмена
        personsModel->select();
    } );

    tabWidget = new QTabWidget(this);

    shiftWidget = new ShiftReport;
    doctWidget = new PayDocReport;

    setCentralWidget(tabWidget);
    tabWidget->showMaximized();
    tabWidget->addTab(saleWidgetGet(),QIcon(":/img/sale_point.png"), "Точка продажи");
    tabWidget->addTab(bookingWidget,QIcon(":/img/chees.png"),"Шахматка");
    tabWidget->addTab(personsPoint, QIcon(":/img/clients.png"), "Клиенты");
    tabWidget->addTab(bookings, QIcon(":/img/booking.png"), "Брони");
    tabWidget->addTab(shiftWidget,  QIcon(":/img/shift.png"),"Смены");
    tabWidget->addTab(doctWidget,  QIcon(":/img/pay_doc.png"),"Чеки");
    tabWidget->addTab(new MaillistWidget, QIcon(":/img/mail.png"), "Рассылки");
    tabWidget->setCurrentIndex(1);


    connect(tabWidget, &QTabWidget::currentChanged, [&](int index){
        switch(index)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            personsModel->select();
            break;
        case 3:
            bookings->updateData();
            break;
        case 4:
            shiftWidget->actualze();
            break;
        case 5:
            doctWidget->actualze();
            break;

        }
    });
    updateWindow();


}

MainWindow::~MainWindow()
{
}

void MainWindow::connectDB()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL"));
    db->setHostName("localhost");
    db->setDatabaseName("mis");
    db->setUserName("mis");
    db->setPassword("mis");

    if(!db->open())
    {
        qCritical() << "DB not open";
        exit(1);
    }
}

QWidget* MainWindow::saleWidgetGet()
{
    QGroupBox *groupBox = new QGroupBox(this);
    saleRadio = new QRadioButton(tr("Продажа"));
    depositRadio = new QRadioButton(tr("Депозит"));
    returnRadio = new QRadioButton(tr("Возврат"));
    saleRadio->setChecked(true);
    QHBoxLayout *vbox = new QHBoxLayout(this);
    vbox->addWidget(saleRadio);
    //    vbox->addWidget(depositRadio);
    vbox->addWidget(returnRadio);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    goodsModel =  new QSqlQueryModel(this);

    goodsModel->setQuery("SELECT * FROM nomenclature UNION "
                         "SELECT 10000 + row_number() over (order by (select NULL)) as idd, 10000 + row_number() over (order by (select NULL)) as idd,  'Услуги проживания', apartments.shortname||'(Пол суток)', cost/2, 1, 2, false,1,'Из текущих цен', current_timestamp FROM apartments "
                         "LEFT JOIN  prices ON  apartments.id=prices.apartment_id AND NOT for_children "
                         "WHERE current_date BETWEEN start_date AND stop_date "
                         "UNION SELECT 1000+10000 + row_number() over (order by (select NULL)) as idd, 1000+10000 + row_number() over (order by (select NULL)) as idd, 'Услуги проживания', apartments.shortname||'(Пол суток)', cost/2, 1, 2, false,1,'Из текущих цен', current_timestamp FROM apartments "
                         "LEFT JOIN  prices ON  apartments.parent_id=prices.apartment_id AND  for_children "
                         "WHERE current_date BETWEEN start_date AND stop_date ");
    //    goodsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    goodsModel->setHeaderData(1, Qt::Horizontal, tr("Код"));
    goodsModel->setHeaderData(2, Qt::Horizontal, tr("Для печати"));
    goodsModel->setHeaderData(3, Qt::Horizontal, tr("Наименование"));
    goodsModel->setHeaderData(4, Qt::Horizontal, tr("Цена"));
    goodsModel->setHeaderData(5, Qt::Horizontal, tr("Ед. \n изм."));
    goodsModel->setHeaderData(6, Qt::Horizontal, tr("Тип"));
    goodsModel->setHeaderData(7, Qt::Horizontal, tr("Алко"));
    goodsModel->setHeaderData(8, Qt::Horizontal, tr("Создатель"));
    goodsModel->setHeaderData(9, Qt::Horizontal, tr("Комментарий"));
    goodsModel->setHeaderData(10, Qt::Horizontal, tr("Создано"));
    //    goodsModel->select();

    services = new QTableView(this);
    services->setModel(goodsModel);
    services->setSelectionBehavior(QAbstractItemView::SelectRows);
    services->hideColumn(0);
    services->resizeColumnsToContents();
    //read only
    services->setEditTriggers(QTableWidget::NoEditTriggers);

    saleDocument = new QTableWidget(this);
    saleDocument->setColumnCount(5);

    saleDocument->setSelectionBehavior(QAbstractItemView::SelectRows);
    saleDocument->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(saleDocument, &QTableWidget::doubleClicked, this,&MainWindow::editCountRowsInSaleDocument);

    QPushButton *sale = new QPushButton("Перенести в чек");
    connect(sale,&QPushButton::clicked,this,&MainWindow::addRowsInSaleDocument);
    sale->setMinimumHeight(50);

    QPushButton *countPosition = new QPushButton("Кол-во");
    countPosition->setMinimumHeight(50);
    connect(countPosition,&QPushButton::clicked,this,&MainWindow::editCountRowsInSaleDocument);

    QPushButton *delPosition = new QPushButton("Убрать");
    delPosition->setMinimumHeight(50);
    connect(delPosition,&QPushButton::clicked,this,&MainWindow::dellRowsInSaleDocument);

    QPushButton *guestChangeButton = new QPushButton("Выбрать Клиента");
    connect(guestChangeButton, &QPushButton::clicked,[&](){
        PersonSelectDialog *dialog = new PersonSelectDialog(-1,this);
        if(dialog->exec() == QDialog::Accepted){
            profileId = dialog->personId;
        }
        delete dialog;
    });

    QPushButton *setSale = new QPushButton("РЕГИСТРАЦИЯ ЧЕКА");
    setSale->setMinimumHeight(50);
    connect(setSale,&QPushButton::clicked,this,&MainWindow::printFiscalDocument);

    total = new QLabel(this);
    shiftStatus = new QLabel(this);
    shiftNumber = new QLabel(this);
    QFont labelFont = shiftStatus->font();
    labelFont.setPointSize(25);
    labelFont.setBold(true);
    total->setFont(labelFont);
    shiftNumber->setFont(labelFont);
    shiftStatus->setFont(labelFont);

    QFont font = shiftStatus->font();
    font.setPointSize(25);
    font.setBold(true);
    shiftNumber->setFont(font);
    shiftStatus->setFont(font);

    QLabel *sumCount = new QLabel(this);

    QVBoxLayout *mainLayout= new QVBoxLayout(this);

    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(services);
    mainLayout->addWidget(sale);

    QHBoxLayout *saleLayout= new QHBoxLayout(this);
    saleLayout->addWidget(saleDocument);

    QVBoxLayout *saleOperation= new QVBoxLayout(this);
    saleOperation->addWidget(countPosition);
    saleOperation->addWidget(delPosition);
    saleOperation->addWidget(guestChangeButton);
    saleLayout->addLayout(saleOperation);

    mainLayout->addLayout(saleLayout);
    mainLayout->addWidget(setSale);

    QPushButton *serviceOperation = new QPushButton("Сервис");
    serviceOperation->setMinimumHeight(50);
    connect(serviceOperation,&QPushButton::clicked, this, &MainWindow::serviceOpertionView);


    QHBoxLayout *shiftLayout= new QHBoxLayout();
    shiftLayout->addWidget(serviceOperation);
    shiftLayout->addWidget(new QLabel("К оплате"));
    shiftLayout->addWidget(total);
    total->setText("0.00");
    shiftLayout->addWidget(new QLabel("Статус касс"));
    shiftLayout->addWidget(shiftStatus);
    shiftLayout->addWidget(new QLabel("Номер смены"));
    shiftLayout->addWidget(shiftNumber);
    shiftLayout->addWidget(new QLabel("денег в кассе"));
    shiftLayout->addWidget(sumCount);
    mainLayout->addLayout(shiftLayout);
    QWidget *salePoint =  new QWidget(this);
    salePoint->setLayout(mainLayout);
    return salePoint;
}

void MainWindow::hotelWidgetGen()
{
}

void MainWindow::addRowsInSaleDocument()
{
    if (services->currentIndex()!=QModelIndex()) {
        QItemSelectionModel *selections = services->selectionModel();

        QModelIndex item;
        int rowCount=0;
        rowCount= saleDocument->rowCount();
        qDebug() << "rowCount" << rowCount;
        //You can't put a cell to another you have to use a new constructor
        //saleDocument->setRowCount(rowCount++);

        saleDocument->insertRow(rowCount);
        int insertCount=0;
        foreach (item, selections->selectedIndexes())
        {
            // наименование //цена
            //        if(item.column()==0||item.column()==2||item.column()==3)
            //        {
            if (item.column()==1&&insertCount>0)
                saleDocument->insertRow(rowCount);
            QTableWidgetItem *WidgetItem = new QTableWidgetItem(services->model()->data(item).toString());
            if(item.column()==1||item.column()==2)
                saleDocument->setItem( rowCount, item.column()-1, WidgetItem);
            if(item.column()==4)
                saleDocument->setItem( rowCount, item.column()-2, WidgetItem);


            if(item.column()==4) {
                saleDocument->setItem( rowCount, 3,new QTableWidgetItem("1"));

                ESqlQuery getTotal;
                getTotal.prepare("SELECT ?*cost FROM nomenclature WHERE user_code=?");
                getTotal.addBindValue( saleDocument->item(rowCount,3)->text().toInt());
                getTotal.addBindValue( saleDocument->item(rowCount,0)->text().toInt());
                getTotal.exec();
                getTotal.next();
                qDebug() << "price: "  << getTotal.value(0).toString();
                saleDocument->setItem( rowCount, 4,new QTableWidgetItem(getTotal.value(0).toString()));
                //                                double price = saleDocument->item(rowCount,3)->text().toDouble();

                //                saleDocument->setItem( rowCount, 5, new QTableWidgetItem(QString::number(price*1)));
                insertCount++;
                rowCount++;
            }
            //            if(item.column()==2) {
            //              saleDocument->setItem( rowCount, 3,new QTableWidgetItem("1"));
            //        }

            //        }
        }
        saleDocument->resizeColumnsToContents();
    }
    totalSum();
}

void MainWindow::personsActualize(const QString &filterString)
{
    personsModel =  new PesonsSqlTableModel;

    if(filterString!="")
        personsModel-> setfilter(filterString);

    personsModel->select();
    personsTable->setModel(personsModel);
    personsTable->setSortingEnabled(true);
    personsTable->hideColumn(0);

    personsTable->sortByColumn(1, Qt::AscendingOrder);
    for(int i = 7; i<personsModel->columnCount(); i++)
        personsTable->hideColumn(i);

    personsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    personsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void MainWindow::printFiscalDocument()
{
    if (saleDocument->rowCount()>0) {
        bool ok;
        QStringList payList;
        payList << "Наличные";

        //        if(false)
        //            payList << "Картой";

        if(true)
            payList << "На карту";
        QString recivePayType = QInputDialog::getItem(this, QString::fromUtf8("Тип оплаты"),
                                                      QString::fromUtf8("       Тип оплаты     "),
                                                      payList, 0, false, &ok);
        if (ok){
            atoldev9fiscalobject *fiscal = new atoldev9fiscalobject();
            qDebug() << ECoreObject::instance()->userPrintTitle() <<  ECoreObject::instance()->userVatNum();
            if(!(recivePayType=="На карту"))
                fiscal->setOperator(ECoreObject::instance()->userPrintTitle(), ECoreObject::instance()->userVatNum());

            QSqlDatabase::database().transaction();
            if(saleRadio->isChecked())
                if(!(recivePayType=="На карту")&&fiscal ->openFiscalDocument(operationType::sale).contains("Ошибка ФР:"))
                {
                    QMessageBox::critical(Q_NULLPTR,"Ошибка открытия документа прихода", fiscal->lastError());
                }
            if(!(recivePayType=="На карту")&&returnRadio->isChecked())
                if(fiscal ->openFiscalDocument(operationType::returnSale).contains("Ошибка ФР:"))
                {
                    QMessageBox::critical(Q_NULLPTR,"Ошибка открытия документа прихода", fiscal->lastError());
                }
            QString docNum = fiscal->lastDocumet();
            QString shiftNum = fiscal->getShiftNumber();
            for(int i=0; i<saleDocument->rowCount(); i++){
                FISCALSTRING str;
                str.goodName = saleDocument->item(i,1)->text();
                str.goodPrice = saleDocument->item(i,2)->text().replace(" ₽","").replace(",",".").toDouble();
                str.goodCount = saleDocument->item(i,3)->text().toDouble();
                str.goodCode = saleDocument->item(i,0)->text();

                if(!(recivePayType=="На карту")){
                    if(fiscal->printFiscalString(str).contains("Ошибка ФР:"))
                    {
                        QMessageBox::critical(Q_NULLPTR,"Ошибка печати фискальной строки", fiscal->lastError());
                    }
                }
                if(!saveFiscalString(str,docNum,shiftNum,recivePayType=="Наличные"?payType::payTypeCash:payType::payTypeElectronic,saleRadio->isChecked()?operationType::sale:operationType::returnSale, profileId, fiscal->lastError()))
                    QMessageBox::critical(Q_NULLPTR,"Ошибка сохранения документа", fiscal->lastError());
            }

            if(recivePayType=="Наличные")
                if(fiscal->payFiscalDocument(total->text().toDouble(), payType::payTypeCash).contains("Ошибка ФР:"))
                {
                    QMessageBox::critical(Q_NULLPTR,"Ошибка передачи оплаты", fiscal->lastError());
                }
            if(recivePayType=="Карта")
                if(fiscal->payFiscalDocument(total->text().toDouble(), payType::payTypeElectronic).contains("Ошибка ФР:"))
                {
                    QMessageBox::critical(Q_NULLPTR,"Ошибка передачи оплаты", fiscal->lastError());
                }
            if(!(recivePayType=="На карту")){
                if(fiscal->closeFiscalDocument().contains("Ошибка ФР:"))
                {
                    QMessageBox::critical(Q_NULLPTR,"Ошибка закрытия доумента", fiscal->lastError());
                    // ОТКАТИТЬ ТРАНЗАЦИЮ
                    //                QSqlDatabase::database().rollback();
                }}
            delete fiscal;
        }
        QSqlDatabase::database().commit();

        while (saleDocument->rowCount() > 0)
            saleDocument->removeRow(0);

        total->clear();
        saleRadio->setChecked(true);
    }
}

void MainWindow::editCountRowsInSaleDocument()
{
    bool ok;
    double d = QInputDialog::getDouble(this,
                                       QString::fromUtf8("Введите кол-во"),
                                       QString::fromUtf8("Кол-во:"),
                                       0.00, 0, 100000, 2, &ok );
    if (ok){
        saleDocument->setItem( saleDocument->currentRow(), 3, new QTableWidgetItem(QString::number(d)));
        ESqlQuery getTotal;
        getTotal.prepare("SELECT ?*cost FROM nomenclature WHERE user_code=?");
        getTotal.addBindValue( saleDocument->item(saleDocument->currentRow(),3)->text().toInt());
        getTotal.addBindValue( saleDocument->item(saleDocument->currentRow(),0)->text().toInt());
        getTotal.exec();
        getTotal.next();
        qDebug() << "price: "  << getTotal.value(0).toString();
        saleDocument->setItem(saleDocument->currentRow(), 4,new QTableWidgetItem(getTotal.value(0).toString()));
        saleDocument->resizeColumnsToContents();

        totalSum();
    }
}

void MainWindow::dellRowsInSaleDocument()
{
    qDebug() << "remove row";
    saleDocument->removeRow(saleDocument->currentRow());
    totalSum();
}

void MainWindow::updateWindow()
{
    saleDocument->clear();
    saleDocument->setHorizontalHeaderItem( 0, new QTableWidgetItem("Код"));
    //    saleDocument->hideColumn(1);
    saleDocument->setHorizontalHeaderItem( 1, new QTableWidgetItem("Наименование"));
    saleDocument->setHorizontalHeaderItem( 2, new QTableWidgetItem("Цена"));
    saleDocument->setHorizontalHeaderItem( 3, new QTableWidgetItem("Кол-Во"));
    saleDocument->setHorizontalHeaderItem( 4, new QTableWidgetItem("Сумма"));
    totalSum();
    //    AbstractDevFiscalObject *fiscal =  new atoldev9fiscalobject(this);
    //    shiftNumber->setText(fiscal->getShiftNumber());
    //    shiftStatus->setText(fiscal->fiscalStatus());
    //    qDebug() << "STATUS" << fiscal->fiscalStatus();

    //    delete fiscal;
}

void MainWindow::serviceOpertionView()
{
    CashServiceDialog *dialog = new CashServiceDialog(this);
    dialog->exec();
    delete dialog;
}

bool MainWindow::saveFiscalString(FISCALSTRING str, QString docNum, QString shiftNumber, payType pay_Type, operationType operation_Type, int personId, QString comment)
{
    ESqlQuery getNomenclId;

    int nomenclature_id = 0;
    getNomenclId.prepare("SELECT id FROM nomenclature WHERE user_code=?");
    getNomenclId.addBindValue(str.goodCode.toInt());
    if(getNomenclId.exec()&&getNomenclId.next())
        nomenclature_id = getNomenclId.value(0).toInt();


    ESqlQuery insertFiscalDocument;
    //    personsProfileID integer REFERENCES persons_profiles (id),
    //    bookings_id integer REFERENCES bookings (id) DEFAULT 0,
    //    accessUser_id integer REFERENCES access_users (id),
    //    nomenclature_id integer REFERENCES nomenclature (id) DEFAULT 0,
    //    cost money,
    //    count real DEFAULT 0,
    //    payType  integer, -- 0-Cash, 1-Electro
    //    operation_type integer, --1  sale, 2- return, 3-openshift, 4-z-report, 5-x-report
    //    cash_id integer,
    //  document_number
    //    shift integer,
    //    comment text,

    insertFiscalDocument.prepare("INSERT INTO pay_documents (personsprofileid,accessuser_id,nomenclature_id,"
                                 "count,cost,payType,operation_type,cash_id,document_number,shift,comment ) VALUES(?,?,?,?,?,?,?,?,?,?,?);");
    insertFiscalDocument.addBindValue(personId);
    insertFiscalDocument.addBindValue(ECoreObject::instance()->userId());
    insertFiscalDocument.addBindValue(nomenclature_id);
    insertFiscalDocument.addBindValue(str.goodCount);
    insertFiscalDocument.addBindValue(str.goodPrice);
    insertFiscalDocument.addBindValue(pay_Type);
    insertFiscalDocument.addBindValue(operation_Type);
    insertFiscalDocument.addBindValue(1);
    insertFiscalDocument.addBindValue(docNum.toInt());
    insertFiscalDocument.addBindValue(shiftNumber.toInt());
    insertFiscalDocument.addBindValue(comment);
    if(insertFiscalDocument.exec())
        return true;
    else
        return false;
}

void MainWindow::totalSum()
{
    double sum=0;
    for (int i =0; i< saleDocument->rowCount();i++) {
        sum+=saleDocument->item(i,4)->text().replace("₽","").replace(",",".").replace(" ","").toDouble();
    }
    qDebug() << "SUM" << sum;
    total->setText(QString::number(sum));
}
