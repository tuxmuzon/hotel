#ifndef PERSONSMAILOUT_H
#define PERSONSMAILOUT_H
#include <QObject>
#include <QWidget>
#include "QSqlQuery"
#include "QSqlDatabase"
#include "QTableWidget"
#include "QSqlTableModel"
#include "QDebug"
#include "QDir"
#include "QVBoxLayout"
#include "QDialog"
#include "QDialogButtonBox"
#include "QPlainTextEdit"
#include "QCheckBox"
#include "QDateEdit"
#include "QLabel"
#include "QComboBox"
#include "QList"
#include "../models/pesonssqltablemodel.h"


class PersonsMailOut : public QDialog
{

    Q_OBJECT
private:

    PesonsSqlTableModel *personsModel;
    QTableView *persons;


    PesonsSqlTableModel *selectedPersonsModel;
    QTableView *selectedPersons;
    QList<int> personsList;

    void add();
    void dell();
    void delAll();


    QSqlQuery *query;
    int currentId=0;
    QPlainTextEdit *textEdit;
    QComboBox *typeCombo;
    QCheckBox *isSite;
    QDateEdit *startDate;
    QDateEdit *stopDate;
public:

    explicit PersonsMailOut( QSqlDatabase db ,QWidget *parent = Q_NULLPTR);
    void setId(int id);
    void update();
    void accept();
};

#endif // PERSONSMAILOUT_H
