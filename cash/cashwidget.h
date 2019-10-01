#ifndef maillistWIDGET_H
#define maillistWIDGET_H

#include <QObject>
#include <QWidget>
#include "QSqlQuery"
#include "QSqlDatabase"
#include "QTableWidget"
#include "QSqlTableModel"
#include "QDebug"
#include "QDir"
#include "QVBoxLayout"
#include "QButtonGroup"
#include "QPushButton"
#include "QMessageBox"
//#include "../dialogs/maillistdialog.h"
#include "QSqlRelationalTableModel"

class CashWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CashWidget(QWidget *parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase::database());

private:
    QString tableName;
    QString filterString;
    QSqlRelationalTableModel *model;
    QButtonGroup *manageButton;
    QTableView *tabView;
    QSqlDatabase db;
    int currentId;
    QList<int> perosons;


    void getDialog();
    void dell();
    void edit();
    void add();
    void recivers();


signals:

public slots:
    void setId(int id );

    void setTable(QString TableName) {tableName=TableName;}
    void setFilter(QString FilterString) {filterString=FilterString; }
    QSqlTableModel *getModel() {return model;}

};

#endif // maillistWIDGET_H
