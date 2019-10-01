#include "authdialog.h"
#include <QShortcut>

AuthDialog::AuthDialog(QWidget *parent) : QDialog (parent)
{
    loginEdit = new QLineEdit;
    loginEdit->setPlaceholderText("Логин");

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *okButton = new QPushButton("Ок");
    QPushButton *cancelButton = new QPushButton("Отмена");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QShortcut *ok = new QShortcut(this); // Инициализируем объект
    ok->setKey(Qt::Key_Enter);
    connect(ok,&QShortcut::activated,this,&AuthDialog::slot_ok);

    connect(okButton,&QPushButton::clicked,this,&AuthDialog::slot_ok);
    connect(cancelButton,&QPushButton::clicked,this,&AuthDialog::slot_cancel);

    QVBoxLayout *layout = new QVBoxLayout;

    QPushButton *settingButton =  new QPushButton("Настройки БД");

    layout->addWidget(settingButton);
    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addLayout(buttonLayout);
    this->setLayout(layout);
    setWindowTitle("Авторизация");

    setMinimumWidth(250);
    loginEdit->setFocus();
}

void AuthDialog::slot_ok()
{
    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    if (ECoreObject::instance()->tryAuthorize(login, password))
    {
        accept();
        return;
    }

    QMessageBox::warning(this, "Не удалось авторизироваться", "Не правильные логин или пароль");
}

void AuthDialog::slot_cancel()
{
    reject();
}
