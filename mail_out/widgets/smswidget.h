#ifndef SMSWIDGET_H
#define SMSWIDGET_H

#include <QObject>
#include <QWidget>
#include "QSqlQuery"
#include "QSqlDatabase"
#include "QTableWidget"
#include "QSqlTableModel"
#include "QDebug"
#include "QDir"
#include "QVBoxLayout"

class smsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit smsWidget(QWidget *parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase::database());

private:
    QString tableName;
    QString filterString;
    QSqlTableModel *model;

   void editRecivers();
signals:

public slots:
    void setTable(QString TableName) {tableName=TableName;}
    void setFilter(QString FilterString) {filterString=FilterString; }
    QSqlTableModel *getModel() {return model;}

};

#endif // SMSWIDGET_H
