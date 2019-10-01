#include "pesonssqltablemodel.h"

PesonsSqlTableModel::PesonsSqlTableModel(QObject *parent,  QSqlDatabase db) : QSqlTableModel (parent)
{

    this->setTable("persons_profiles");
    this->setEditStrategy(QSqlTableModel::OnFieldChange);
    this->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    this->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    this->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    this->setHeaderData(4, Qt::Horizontal, tr("Мобильный"));
    this->setHeaderData(5, Qt::Horizontal, tr("e-mail"));
    this->setHeaderData(6, Qt::Horizontal, tr("Дата.р."));

}

void PesonsSqlTableModel::setfilter(QString filter)
{

    this->setFilter("firstname ILIKE '%"+ filter + "%' OR "
                    "secondname ILIKE '%"+ filter + "%' OR "
                    "lastname ILIKE '%"+ filter + "%' OR "
                    "mobile_phone ILIKE '%"+ filter + "%' OR "
                    "email ILIKE '%"+ filter + "%'");
    this->setSort(1,Qt::SortOrder::DescendingOrder);
    this->setSort(2,Qt::SortOrder::DescendingOrder);
    this->setSort(3,Qt::SortOrder::DescendingOrder);
    this->select();
}


