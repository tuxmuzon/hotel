#include "personsmailout.h"
#include "QPushButton"
#include "../../core/esqlquery.h"


PersonsMailOut::PersonsMailOut(QSqlDatabase db, QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    personsModel = new PesonsSqlTableModel(this, db);
    personsModel->select();
    persons = new QTableView();
    persons->setModel(personsModel);
    persons->hideColumn(0);
    for(int i = 7; i<personsModel->columnCount(); i++)
        persons->hideColumn(i);

    persons->setSelectionBehavior(QAbstractItemView::SelectRows);
    persons->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(persons);
    QPushButton *allSelectButton = new QPushButton("Выделить все");
    connect(allSelectButton,&QPushButton::clicked,[=](){persons->selectAll();});

    QPushButton *disableSelectButton = new QPushButton("Снять выделение");
    connect(disableSelectButton,&QPushButton::clicked,[=](){persons->clearSelection();});

    QPushButton *addButton = new QPushButton("Добавить к рассылке");
    connect(addButton,&QPushButton::clicked, this, &PersonsMailOut::add);

    QHBoxLayout *selectLayout = new QHBoxLayout();
    selectLayout->addWidget(allSelectButton);
    selectLayout->addWidget(disableSelectButton);
    selectLayout->addWidget(addButton);

    mainLayout->addLayout(selectLayout);

    selectedPersonsModel = new PesonsSqlTableModel(this, db);
    selectedPersonsModel->select();
    selectedPersons = new QTableView();
    selectedPersons->setModel(selectedPersonsModel);
    selectedPersons->hideColumn(0);
    for(int i = 7; i< selectedPersonsModel->columnCount(); i++)
        selectedPersons->hideColumn(i);

    selectedPersons->setSelectionBehavior(QAbstractItemView::SelectRows);
    selectedPersons->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(selectedPersons);

    QPushButton *dellButton = new QPushButton("Удалить из рассылки");
    connect(dellButton,&QPushButton::clicked, this, &PersonsMailOut::dell);

    QPushButton *dellAllButton = new QPushButton("Удалить все из рассылки");
    //connect(dellAllButton,&QPushButton::clicked, this, &PersonsMailOut::dell);


    QHBoxLayout *personsLayout = new QHBoxLayout();
    personsLayout->addWidget(dellButton);
    personsLayout->addWidget(dellAllButton);

    mainLayout->addLayout(personsLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, [=](){ this->accept();  qDebug("ok or save");     });
    connect(buttons, &QDialogButtonBox::rejected, this, [=](){this->close();  qDebug("cancel"); });
    setLayout(mainLayout);
    setWindowTitle("Получатели");
}

void PersonsMailOut::setId(int id) {

    personsList.clear();
    ESqlQuery query;
    query.prepare("SELECT persons_profiles_id FROM mail_out WHERE news_id=?");
    query.addBindValue(id);
    query.exec();

    while (query.next()) {
        personsList.append(query.value(0).toInt());
    }
    qDebug() << "persons List: " << personsList;
    currentId = id;
    update();
}

void PersonsMailOut::add()
{
    QModelIndexList selection = persons->selectionModel()->selectedRows();
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        personsList.append(personsModel->data(personsModel->index(index.row(),0)).toInt());
    }
    update();
}

void PersonsMailOut::dell()
{
    QModelIndexList selection = selectedPersons->selectionModel()->selectedRows();
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        personsList.removeOne(selectedPersonsModel->data(selectedPersonsModel->index(index.row(),0)).toInt());
    }
    update();
}

void PersonsMailOut::delAll()
{
    personsList.clear();
    update();
}

void PersonsMailOut::update()
{
    QString string;
    for(int i=0; i<personsList.size(); i++)
    {
        string += QString::number(personsList[i]);
        if(i<personsList.size()-1)
            string += "," ;
    }

    if (string.isEmpty())
        string = "NULL";
    selectedPersonsModel->setFilter("id IN (" + string +")");

    if (string == "NULL")
        personsModel->setFilter("");
    else
        personsModel->setFilter("id NOT IN (" + string +")");


}

void PersonsMailOut::accept()
{
    // все удалить из рассылки и напихать заново
    ESqlQuery query;
    query.prepare("DELETE FROM mail_out WHERE news_id=?");
    query.addBindValue(currentId);
    query.exec();

    for (int i =0 ; i< personsList.size(); i++)
    {
        query.prepare("INSERT INTO mail_out(news_id,persons_profiles_id) VALUES(?,?)");
        query.addBindValue(currentId);
        query.addBindValue(personsList.at(i));
        query.exec();
    }
       this->close();
}
