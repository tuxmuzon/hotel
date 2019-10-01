#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include "core/ecoreobject.h"
#include <QMessageBox>
class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent = nullptr);
private:
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;

public Q_SLOTS:
    void slot_ok();
    void slot_cancel();
};

#endif // AUTHDIALOG_H
