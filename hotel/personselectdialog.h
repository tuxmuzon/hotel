#ifndef PERSONSELECTDIALOG_H
#define PERSONSELECTDIALOG_H


#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include "QDialogButtonBox"
#include "QMessageBox"
#include "QTableView"
#include "QSqlTableModel"


class PersonSelectDialog : public  QDialog
{
    Q_OBJECT
public:
    explicit PersonSelectDialog(int prodileId = -1 ,QWidget *parent = nullptr);
    int personId;

public slots:
    void update(QString filterString=QString(""));
    void slot_accept();

private:
    int currentId;

    QTableView *personsTable;
    QSqlTableModel *personsModel;
    QDialogButtonBox *buttonBox;
};

#endif // PERSONSELECTDIALOG_H
