#include "profileedit.h"
#include <QGridLayout>
#include "QLabel"
#include "core/esqlquery.h"
#include "QSqlRecord"
#include "QSqlError"
#include "QDebug"
#include "QGroupBox"

static const QStringList sexItems =
        QStringList() << "Мужской" << "Женский";

// получуается это нужно только для иностраных граждан в связке с визитом
static const QStringList visitDocs =
        QStringList() << "Виза" << "Вид на жительство" << "Разрешение на временное проживание";

static const QStringList purposeOfVisit  =
        QStringList() << "Служебная" << "Туризм" << "Деловая" << "Учеба"  << "Работа" << "Частная"
                      << "Гуманитарная" << "Другая";

profileEdit::profileEdit(int prodileId , QWidget *parent)  : QDialog (parent)
{
    currentId = prodileId;
    QVBoxLayout *mainLayout = new QVBoxLayout();

    lastName = new QLineEdit;
    secondName = new QLineEdit;
    firstName = new QLineEdit;
    QHBoxLayout *fio= new QHBoxLayout;
    QGroupBox *fioBox = new QGroupBox(tr("Анкетные данные"));
    fio->addWidget( new QLabel("Фамилия:"));
    fio->addWidget(firstName);
    fio->addWidget( new QLabel("Имя:"));
    fio->addWidget(secondName);
    fio->addWidget( new QLabel("Отчество:"));
    fio->addWidget(lastName);
    fioBox->setLayout(fio);
    mainLayout->addWidget(fioBox);

    QHBoxLayout *beathSex= new QHBoxLayout;
    QGroupBox *beathSexBox = new QGroupBox(tr("Личные данные"));
    beathSex->addWidget(new QLabel("Дата рождения:"));
    beathDay = new QDateEdit;
    beathDay->setDisplayFormat("dd.MM.yyyy");
    //показывать календарь
    beathDay->setCalendarPopup(true);
    beathSex->addWidget(beathDay);
    sex =new QComboBox();
    sex->addItems(sexItems);
    beathSex->addWidget( new QLabel("Пол:"));
    beathSex->addWidget(sex);
    beathSexBox->setLayout(beathSex);
    mainLayout->addWidget(beathSexBox);

    beathCountry = new QLineEdit();
    residentCountry = new QLineEdit();
    address = new QLineEdit();

    QGroupBox *placeBox = new QGroupBox("Данные о месте проживания");
    QVBoxLayout *placeMain = new QVBoxLayout;

    QHBoxLayout *place1 = new QHBoxLayout;
    place1->addWidget( new QLabel("Место рождения гос-во:"));
    place1->addWidget(beathCountry);
    placeMain->addLayout(place1);

    QHBoxLayout *place2 = new QHBoxLayout;
    place2->addWidget( new QLabel("Место жительства Страна:"));
    place2->addWidget(residentCountry);
    placeMain->addLayout(place2);

    QHBoxLayout *place3 = new QHBoxLayout;
    place3->addWidget( new QLabel("Место жительства Адрес:"));
    place3->addWidget(address);
    placeMain->addLayout(place3);

    placeBox->setLayout(placeMain);
    mainLayout->addWidget(placeBox);

    QGroupBox *docBox = new QGroupBox("Паспортные данные");
    QVBoxLayout *docMain = new QVBoxLayout;

    docType = new QLineEdit();
    QHBoxLayout *doc1 = new QHBoxLayout;
    doc1->addWidget( new QLabel("Документ удств. личность:"));
    doc1->addWidget(docType);
    docMain->addLayout(doc1);


    QHBoxLayout *doc2 = new QHBoxLayout;
    beathPlace = new QLineEdit();
    doc2->addWidget(new QLabel("Место рождения как в паспорте:"));
    doc2->addWidget(beathPlace);
    docMain->addLayout(doc2);

    docCreateDate = new QDateEdit();
    docCreateDate->setDisplayFormat("dd.MM.yyyy");
    QHBoxLayout *doc3 = new QHBoxLayout;
    beathPlace = new QLineEdit();
    doc3->addWidget(new QLabel("Дата выдачи:"));
    doc3->addWidget(docCreateDate);
    docMain->addLayout(doc3);

    docSerial = new QLineEdit();
    docNumber = new QLineEdit();
    QHBoxLayout *doc4 = new QHBoxLayout;
    beathPlace = new QLineEdit();
    doc4->addWidget(new QLabel("Серия:"));
    doc4->addWidget(docSerial);
    doc4->addWidget( new QLabel("№:"));
    doc4->addWidget(docNumber);
    docMain->addLayout(doc4);

    // docValideDate = new QDateEdit();

    docCreateOrg = new QLineEdit();
    QHBoxLayout *doc5 = new QHBoxLayout;
    beathPlace = new QLineEdit();
    doc5->addWidget(new QLabel("Кем выдан:"));
    doc5->addWidget(docCreateOrg);
    docMain->addLayout(doc5);

    docCreateOrgCode = new QLineEdit();
    QHBoxLayout *doc6 = new QHBoxLayout;
    beathPlace = new QLineEdit();
    doc6->addWidget(new QLabel("Код подразделения:"));
    doc6->addWidget(docCreateOrgCode);
    docMain->addLayout(doc6);

    docBox->setLayout(docMain);
    mainLayout->addWidget(docBox);

    eMail =  new QLineEdit;
    mobilePhone = new QLineEdit;
    QGroupBox *contactBox = new QGroupBox("Контактные данные");
    QHBoxLayout *contact = new QHBoxLayout;

    contact->addWidget(new QLabel("e-Mail:"));
    contact->addWidget(eMail);
    contact->addWidget(new QLabel("Телефон:"));
    contact->addWidget(mobilePhone);
    contactBox->setLayout(contact);

    mainLayout->addWidget(contactBox);

    isVip =  new QCheckBox;
    vipPrice = new QSpinBox;
    connect(isVip, &QCheckBox::toggled,vipPrice,&QSpinBox::setEnabled);

    vipPrice->setDisabled(true);
    vipPrice->setMaximum(30000);
    vipPrice->setMinimum(200);
    //    mainLayout->addWidget(new QLabel("Срок действия:"), 6,2);
    //    mainLayout->addWidget(docValideDate,6,3);

    QGroupBox *grantBox = new QGroupBox("Дополнительные привелегии");
    QHBoxLayout *grant = new QHBoxLayout();
    grant->addWidget(isVip);
    grant->addWidget(new QLabel("Фиксированная Цена за место:"));
    grant->addWidget(vipPrice);
    grant->addStretch(1);

    grantBox->setLayout(grant);

    mainLayout->addWidget(grantBox);

    setLayout(mainLayout);
    setWindowTitle("Анкета клиента");
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox,&QDialogButtonBox::accepted,this,&profileEdit::slot_accept);
    connect(buttonBox,&QDialogButtonBox::rejected,this,&profileEdit::reject);

    mainLayout->addWidget(buttonBox);

    update();
}

void profileEdit::update()
{
    ESqlQuery query;
    qDebug() << "currentId: " << currentId;
    query.prepare("SELECT firstname, secondname, lastname, mobile_phone, email, beathday, sex, beath_country, beath_place, doc_type, "
                  "doc_serial, doc_number, doc_create_date, doc_create_org, doc_create_org_code, resident_country, address, is_vip, vip_price::numeric::float8 "
                  "FROM  persons_profiles WHERE id = ? ");

    query.addBindValue(currentId);
    query.exec();
    while(query.next())
    {
        QSqlRecord rec = query.record();
        firstName->setText(query.value(rec.indexOf("firstname")).toString());
        secondName->setText(query.value(rec.indexOf("secondname")).toString());
        lastName->setText(query.value(rec.indexOf("lastname")).toString());
        mobilePhone->setText(query.value(rec.indexOf("mobile_phone")).toString());
        eMail->setText(query.value(rec.indexOf("email")).toString());
        beathDay->setDate(query.value(rec.indexOf("beathday")).toDate());
        sex->setCurrentIndex(query.value(rec.indexOf("sex")).toInt());
        beathCountry->setText(query.value(rec.indexOf("beath_country")).toString());
        beathPlace->setText(query.value(rec.indexOf("beath_place")).toString());
        docType->setText(query.value(rec.indexOf("doc_type")).toString());
        docSerial->setText(query.value(rec.indexOf("doc_serial")).toString());
        docNumber->setText(query.value(rec.indexOf("doc_number")).toString());
        docCreateDate->setDate(query.value(rec.indexOf("doc_create_date")).toDate());
        docCreateOrg->setText(query.value(rec.indexOf("doc_create_org")).toString());
        docCreateOrgCode->setText(query.value(rec.indexOf("doc_create_org_code")).toString());
        residentCountry->setText(query.value(rec.indexOf("resident_country")).toString());
        address->setText(query.value(rec.indexOf("address")).toString());
        isVip->setChecked(query.value(rec.indexOf("is_vip")).toBool());
        vipPrice->setValue(query.value(rec.indexOf("vip_price")).toInt());
    }
}

void profileEdit::slot_accept()
{
    ESqlQuery query;
    if (currentId < 1)
    {
        query.prepare("INSERT INTO persons_profiles (firstname, secondname, lastname, mobile_phone, email, beathday, sex, beath_country, beath_place, doc_type, "
                      "doc_serial, doc_number, doc_create_date, doc_create_org, doc_create_org_code, resident_country, address, is_vip, vip_price ) "
                      "VALUES (:FirstName,:secondname,:lastname,:mobile_phone,:email,:beathday,:sex,:beath_country,:beath_place,:doc_type,"
                      ":doc_serial,:doc_number,:doc_create_date,:doc_create_org,:doc_create_org_code,:resident_country,:address,:is_vip,:vip_price) RETURNING id" );
    } else
    {
        query.prepare("UPDATE persons_profiles SET "
                      "firstname = :FirstName, "
                      "secondname = :secondname, "
                      "lastname = :lastname, "
                      "mobile_phone = :mobile_phone, "
                      "email = :email, "
                      "beathday = :beathday, "
                      "sex = :sex, "
                      "beath_country = :beath_country, "
                      "beath_place = :beath_place, "
                      "doc_type = :doc_type, "
                      "doc_serial = :doc_serial, "
                      "doc_number = :doc_number, "
                      "doc_create_date = :doc_create_date, "
                      "doc_create_org = :doc_create_org, "
                      "doc_create_org_code = :doc_create_org_code, "
                      "resident_country = :resident_country, "
                      "address = :address, "
                      "is_vip = :is_vip, "
                      "vip_price = :vip_price "
                      "WHERE id =:ID ");
        query.bindValue(":ID", currentId);
    }

    query.bindValue(":FirstName", firstName->text());
    query.bindValue(":secondname",secondName->text());
    query.bindValue(":lastname",lastName->text());
    query.bindValue(":mobile_phone",mobilePhone->text());
    query.bindValue(":email",eMail->text());
    query.bindValue(":beathday",beathDay->date());
    query.bindValue(":sex",sex->currentIndex());
    query.bindValue(":beath_country",beathCountry->text());
    query.bindValue(":beath_place",beathPlace->text());
    query.bindValue(":doc_type",docType->text());
    query.bindValue(":doc_serial",docSerial->text());
    query.bindValue(":doc_number",docNumber->text());
    query.bindValue(":doc_create_date",docCreateDate->date());
    query.bindValue(":doc_create_org",docCreateOrg->text());
    query.bindValue(":doc_create_org_code",docCreateOrgCode->text());
    query.bindValue(":resident_country",residentCountry->text());
    query.bindValue(":address",address->text());
    query.bindValue(":is_vip",isVip->isChecked());
    query.bindValue(":vip_price",vipPrice->value());

    if(!query.exec()){
        QMessageBox::warning(this,"Ошибка сохранения", query.lastError().text());
    }
    else
    {
        if(currentId < 1 && query.next())
        {
            currentId =  query.value(0).toInt();
        }
        accept();
    }
}
