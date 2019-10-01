#ifndef BOOKINGTABWIDGET_H
#define BOOKINGTABWIDGET_H

#include <QTabWidget>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include "models/bookingviewmodel.h"
#include <QComboBox>
#include <QDebug>
#include <QPushButton>
#include "hotel/personselectdialog.h"
#include "hotel/booking/bookingdialog.h"

class BookingTabWidget : public QTabWidget
{
public:
    explicit BookingTabWidget(QWidget *parent = nullptr);
private:
    QTableView *bookingTable;
    BookingViewModel *bookingModel;
    QComboBox *apartmentsComboBox;
private Q_SLOTS:
    void slot_apartmentChanged(int index);
    void slot_clientChanged(int client_id);
    void slot_wizard();
public slots:
    void updateData();
};

#endif // BOOKINGTABWIDGET_H
