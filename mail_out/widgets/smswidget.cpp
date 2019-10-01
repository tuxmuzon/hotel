#include "smswidget.h"



smsWidget::smsWidget(QWidget *parent, QSqlDatabase db) : QWidget(parent)
{
    model = new QSqlTableModel(this, db);
    model->setTable(QString("public.outbox"));
    model->setFilter("processed=true");
    qWarning() << "current DIR" <<QDir::currentPath();
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->select();

    model->setHeaderData(1, Qt::Horizontal, tr("Кому"));
    model->setHeaderData(2, Qt::Horizontal, tr("Отправлено"));
    model->setHeaderData(3, Qt::Horizontal, tr("Создано"));
    model->setHeaderData(4, Qt::Horizontal, tr("Текст Сообщения"));
    model->setHeaderData(5, Qt::Horizontal, tr("Телефон?"));
    model->setHeaderData(6, Qt::Horizontal, tr("Статус Отправки"));

    QTableView *smsWidget = new QTableView(this);
    smsWidget->setModel(model);
    smsWidget->hideColumn(0);
    smsWidget->resizeColumnsToContents();
    QVBoxLayout *mainLayot= new QVBoxLayout;
    mainLayot->addWidget(smsWidget);
    this->setLayout(mainLayot);
}

void smsWidget::editRecivers()
{

}
