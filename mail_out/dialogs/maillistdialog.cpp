#include "maillistdialog.h"
#include "QSqlError"

maillistDialog::maillistDialog(QSqlDatabase db, QWidget *parent) : QDialog(parent)
{
    query = new QSqlQuery(db);
    textEdit = new QPlainTextEdit;
    typeCombo =  new QComboBox();
    typeCombo->addItems(QStringList() << "sms" << "e-mail" << "sms & e-mail");
    isSite = new QCheckBox("Публиковать рассылку на сайте");
    startDate =new QDateEdit(QDate::currentDate());
    stopDate =new QDateEdit(QDate::currentDate().addDays(3));
    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget( new QLabel("Дата начала рассылки"));
    dateLayout->addWidget( startDate);
    dateLayout->addWidget( new QLabel("Дата окончания рассылки"));
    dateLayout->addWidget( stopDate);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QVBoxLayout *mainLayot= new QVBoxLayout(parent);
    mainLayot->addWidget(textEdit);
    mainLayot->addWidget(isSite);
    QHBoxLayout *typeLayout = new QHBoxLayout(parent);
    typeLayout->addWidget(new QLabel("Тип рассылки"));
    typeLayout->addWidget(typeCombo);
    mainLayot->addLayout(typeLayout);
    mainLayot->addLayout(dateLayout);
    mainLayot->addWidget(buttons);
    this->setLayout(mainLayot);
    connect(buttons, &QDialogButtonBox::accepted, this, [=](){ this->accept();  qDebug("ok or save");     });
    connect(buttons, &QDialogButtonBox::rejected, this, [=](){this->close();  qDebug("cancel"); });

}

void maillistDialog::update()
{
    query->prepare("SELECT text, type, is_site, start_send_date, stop_send_date FROM news WHERE id=?" );
    query->addBindValue(currentId>0?currentId:0);
    if(!query->exec())
        qWarning() << "Запрос с ошибкой \n" << query->lastQuery() << "Текст Запроса \n" << query->lastError().text();

    query->next();

    textEdit->insertPlainText(query->value(0).toString());
    typeCombo->setCurrentIndex(query->value(1).toInt());
    isSite->setChecked(query->value(2).toBool());
    startDate->setDate(query->value(3).toDate());
    stopDate->setDate(query->value(4).toDate());
}

void maillistDialog::accept()
{
    if(currentId==0)
        query->prepare("INSERT INTO news(text, type, is_site, start_send_date, stop_send_date) VALUES (?,?,?,?,?);" );
    else
        query->prepare("UPDATE news SET text=?, type=?, is_site=?, start_send_date=?, stop_send_date=? WHERE id=?;" );

    query->addBindValue(textEdit->toPlainText());
    query->addBindValue(typeCombo->currentIndex());
    query->addBindValue(isSite->isChecked());
    query->addBindValue(startDate->date());
    query->addBindValue(stopDate->date());


    if(currentId!=0)
        query->addBindValue(currentId);

    if(!query->exec())
        qWarning() << "Запрос с ошибкой \n" << query->lastQuery() << "Текст Запроса \n" << query->lastError().text();
    this->close();
}
