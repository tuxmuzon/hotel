#include "bookingtabwidget.h"
#include "QWizard"

BookingTabWidget::BookingTabWidget(QWidget *parent) : QTabWidget (parent)
{
    bookingTable = new QTableView;
    bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookingModel = new BookingViewModel;

    bookingTable->setModel(bookingModel);

    apartmentsComboBox = new QComboBox;
    connect(apartmentsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        slot_apartmentChanged(index);
    });

    QSqlQueryModel *apartmentsModel = new QSqlQueryModel;
    apartmentsModel->setQuery("SELECT '---' as shortname, -1 as id "
                              "UNION "
                              "SELECT shortname, id FROM apartments "
                              "ORDER BY id;");
    apartmentsComboBox->setModel(apartmentsModel);

    QPushButton *guestChangeButton = new QPushButton("Выбрать гостя");
    connect(guestChangeButton, &QPushButton::clicked,[&](){
        PersonSelectDialog *dialog = new PersonSelectDialog(-1,this);
        if(dialog->exec() == QDialog::Accepted){
            this->slot_clientChanged(dialog->personId);
        }
        delete dialog;
    });

    QPushButton *resetButton = new QPushButton("Сбросить");
    connect(resetButton, &QPushButton::clicked, this, &BookingTabWidget::updateData);

    QPushButton *wizardButton = new QPushButton("Груповая Бронь");
    connect(wizardButton, &QPushButton::clicked, this, &BookingTabWidget::slot_wizard);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(apartmentsComboBox);
    hlayout->addWidget(guestChangeButton);
    hlayout->addWidget(resetButton);
    hlayout->addWidget(wizardButton);

    QHBoxLayout *actionButtonsLayout = new QHBoxLayout;

    QPushButton *newBooking = new QPushButton(tr("Добавить"));
    QPushButton *editBooking = new QPushButton(QIcon(":/img/edit.png"),tr("Редактировать"));

    connect(newBooking, &QPushButton::clicked, this, [&](){
        BookingDialog dialog;
        dialog.exec();
    });

    connect(editBooking, &QPushButton::clicked, this, [&](){
        QModelIndex currentIndex = bookingTable->currentIndex();
        int bookingId = bookingModel->getBookingId(currentIndex);

        BookingDialog * dialog = new BookingDialog(bookingId);
        if (dialog->exec() == QDialog::Accepted)
        {
            bookingModel->resetFilter();

        }
        delete  dialog;
    }
    );

    connect(bookingTable, &QTableView::doubleClicked, this, [&](){
        QModelIndex currentIndex = bookingTable->currentIndex();
        int bookingId = bookingModel->getBookingId(currentIndex);

        BookingDialog * dialog = new BookingDialog(bookingId);

        if (dialog->exec() == QDialog::Accepted)
        {
            bookingModel->resetFilter();

        }
        delete  dialog;

    });



    actionButtonsLayout->addWidget(newBooking);
    actionButtonsLayout->addWidget(editBooking);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addLayout(actionButtonsLayout);
    vlayout->addWidget(bookingTable);

    this->setLayout(vlayout);

    updateData();
}

void BookingTabWidget::slot_apartmentChanged(int index)
{
    QSqlQueryModel *model = dynamic_cast<QSqlQueryModel*>(apartmentsComboBox->model());

    int apartment_id = model->data(model->index(index, 1)).toInt();

    bookingModel->setFilterByApartment(apartment_id);
}

void BookingTabWidget::slot_clientChanged(int client_id)
{
    bookingModel->setFilterByClient(client_id);
}

void BookingTabWidget::slot_wizard()
{
    QWizard groupBookingWizard;
    QWizardPage page1;

//    groupBookingWizard.setPage()

}

void BookingTabWidget::updateData()
{
    apartmentsComboBox->setCurrentIndex(0);
    bookingModel->resetFilter();
    bookingTable->hideColumn(0);
    bookingTable->hideColumn(1);
    bookingTable->hideColumn(2);
    bookingTable->hideColumn(8);
    bookingTable->resizeColumnsToContents();
}
