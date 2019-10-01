#ifndef PESONSSQLTABLEMODEL_H
#define PESONSSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QString>


class PesonsSqlTableModel : public QSqlTableModel
{
public:
    PesonsSqlTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase::database());

public:
    void setfilter(QString filter);
};

#endif // PESONSSQLTABLEMODEL_H
