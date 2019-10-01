#include "shiftreport.h"

#include "QVBoxLayout"

ShiftReport::ShiftReport(QWidget *parent)
{
    table = new QTableView(parent);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSortingEnabled(true);

    QVBoxLayout * mainLayout = new  QVBoxLayout();

    mainLayout->addWidget(table);
    setLayout(mainLayout);
    actualze();
}

void ShiftReport::actualze()
{
    // по сумме добавить минус возврата
    QSqlQueryModel *model = new  QSqlQueryModel();
    model->setQuery("SELECT shift,  MIN(shortname), MIN(pay_documents.created) as open, MAX(pay_documents.created) as closed, SUM(cost) FROM pay_documents "
                    "LEFT JOIN access_users ON access_users.id=accessUser_id "
                    //      "WHERE operation_type=1"
                    "GROUP BY shift ORDER BY shift");

    model->setHeaderData(0,  Qt::Horizontal, tr("Номер"));
    model->setHeaderData(1,  Qt::Horizontal, tr("Кассир"));
    model->setHeaderData(2,  Qt::Horizontal, tr("Открыта"));
    model->setHeaderData(3,  Qt::Horizontal, tr("Закрыта"));
    model->setHeaderData(4,  Qt::Horizontal, tr("Выручка"));


    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setModel(model);
    table->resizeColumnsToContents();
}
