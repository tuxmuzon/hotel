#include "personselectdialog.h"
#include "QVBoxLayout"
#include "QLabel"
#include "QDebug"
#include "QModelIndex"
#include "QAbstractItemModel"

PersonSelectDialog::PersonSelectDialog(int prodileId, QWidget *parent)
{
    // TODO: подвечивать тот что выбран

    personsTable = new QTableView();
    //    personsActualize();
    QVBoxLayout *personsLayout= new QVBoxLayout(this);
    QLineEdit *filterEdit = new QLineEdit;
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Поиск: "));
    filterLayout->addWidget(filterEdit);
    personsLayout->addLayout(filterLayout);
    personsLayout->addWidget(personsTable);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    personsLayout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slot_accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(personsTable, &QTableView::doubleClicked, this, &PersonSelectDialog::slot_accept);
    setLayout(personsLayout);
    connect(filterEdit,&QLineEdit::textEdited, this, &PersonSelectDialog::update);
    personsTable->size().width();
    this->resize(personsTable->size().width(), this->size().height());
    update();
    setWindowTitle("Выбор клиента");

}

void PersonSelectDialog::update(QString filterString)
{
    personsModel =  new QSqlTableModel;
    personsModel->setTable("persons_profiles");
    personsModel->setFilter("true ORDER BY firstname ASC, secondname ASC, lastname ASC");
    if(filterString!="")
    {
        personsModel->setFilter("firstname ILIKE '%"+ filterString + "%' OR "
                                "secondname ILIKE '%"+ filterString + "%' OR "
                                "lastname ILIKE '%"+ filterString + "%' OR "
                                "mobile_phone ILIKE '%"+ filterString + "%' OR "
                                "email ILIKE '%"+ filterString + "%'");
    }
    qDebug() << "filter" << personsModel->filter();
    personsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    personsModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    personsModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    personsModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    personsModel->setHeaderData(4, Qt::Horizontal, tr("Мобильный"));
    personsModel->setHeaderData(5, Qt::Horizontal, tr("e-mail"));
    personsModel->setHeaderData(6, Qt::Horizontal, tr("Дата.р."));
    personsModel->select();
    personsTable->setModel(personsModel);
    personsTable->hideColumn(0);
    for(int i = 7; i<personsModel->columnCount(); i++)
        personsTable->hideColumn(i);

    personsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    personsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PersonSelectDialog::slot_accept()
{
    QModelIndex inx = personsModel->index(personsTable->currentIndex().row(),0);
    personId = personsTable->model()->data(inx).toInt();
    accept();
    close();
}
