#include "cashwidget.h"
//#include "../dialogs/personsmailout.h"
#include "../core/esqlquery.h"


CashWidget::CashWidget(QWidget *parent, QSqlDatabase db) : QWidget(parent)
{
    this->db=db;
    model = new QSqlRelationalTableModel(this, db);
    model->setTable(QString("public.news"));
//    model->setFilter("processed=true");
    qWarning() << "current DIR" <<QDir::currentPath();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(1, Qt::Horizontal, tr("Текст"));
    model->setHeaderData(2, Qt::Horizontal, tr("Тип"));
    model->setHeaderData(3, Qt::Horizontal, tr("Создано"));
    model->setHeaderData(4, Qt::Horizontal, tr("На сайте"));
    model->setHeaderData(5, Qt::Horizontal, tr("Начало"));
    model->setHeaderData(6, Qt::Horizontal, tr("Окончание"));

    tabView = new QTableView(this);
    tabView->setModel(model);
    tabView->hideColumn(0);
    tabView->resizeColumnsToContents();
    tabView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QVBoxLayout *mainLayot= new QVBoxLayout;
    mainLayot->addWidget(tabView);

    QPushButton *add = new QPushButton(QIcon(":/img/ok.png"),"Добавить");
    QPushButton *dell = new QPushButton("Удалить");
    QPushButton *edit = new QPushButton(QIcon(":/img/edit.png"),"Редактировать");
    QPushButton *recivers = new QPushButton(QIcon(":/img/clients.png"),"Получатели");
    QPushButton *publish  = new QPushButton(QIcon(":/img/ok.png"),"Опубликовать");

    QHBoxLayout *buttons=new QHBoxLayout;
    buttons->addWidget(add);
    buttons->addWidget(dell);
    buttons->addWidget(edit);
    buttons->addWidget(recivers);
    buttons->addWidget(publish);

    mainLayot->addLayout(buttons);
    this->setLayout(mainLayot);
    tabView->selectRow(0);

//    connect(dell,&QPushButton::pressed,this,&MaillistWidget::dell);
//    connect(edit,&QPushButton::pressed,this,&MaillistWidget::edit);
//    connect(tabView,&QTableView::doubleClicked,this,&MaillistWidget::edit);
//    connect(add,&QPushButton::pressed,this,&MaillistWidget::add);
//    connect(recivers,&QPushButton::pressed,this,&MaillistWidget::recivers);

    this->setMinimumWidth(480);
}

void CashWidget::getDialog()
{

}

void CashWidget::dell()
{
    QMessageBox msgBox;
    msgBox.setText("Действительно удалить рассылку?");
    msgBox.setInformativeText("Действительно удалить рассылку?, после удаления рассылку не восстановить");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int res = msgBox.exec();
    if (res == QMessageBox::Ok) //нажата кнопка Ok
         model->removeRow(tabView->currentIndex().row());
    else //отмена
        qWarning() << "Cancel";//  отмена
}

void CashWidget::edit()
{
qWarning() << "edit " << model->data(model->index(tabView->currentIndex().row(),0)).toInt() ;
//QSqlDatabase::database() ;
//    maillistDialog dialod(this->db);
//  //      model.record(i).value("id").toInt();


//    dialod.setId(model->data(model->index(tabView->currentIndex().row(),0)).toInt() );
//    dialod.update();
//    dialod.show();
//    dialod.exec();
    model->select();
}

void CashWidget::add()
{
//        maillistDialog dialod(this->db);
//        dialod.exec();
        model->select();
        tabView->resizeColumnsToContents();
}

void CashWidget::recivers()
{
//    PersonsMailOut dialog(this->db);
//    dialog.setId(model->data(model->index(tabView->currentIndex().row(),0)).toInt());
//    dialog.exec();
}

void CashWidget::setId(int id)
{

    currentId=id;
}


