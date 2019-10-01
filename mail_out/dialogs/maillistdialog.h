#ifndef MAILLISTDIALOG_H
#define MAILLISTDIALOG_H

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


class maillistDialog : public QDialog
{
    Q_OBJECT
private:
    QSqlQuery *query;
    int currentId=0;
    QPlainTextEdit *textEdit;
    QComboBox *typeCombo;
    QCheckBox *isSite;
    QDateEdit *startDate;
    QDateEdit *stopDate;
public:

    explicit maillistDialog( QSqlDatabase db ,QWidget *parent = Q_NULLPTR);
    void setId(int id) {currentId = id;}
    void update();
    void accept();

signals:

public slots:
};

#endif // MAILLISTDIALOG_H
