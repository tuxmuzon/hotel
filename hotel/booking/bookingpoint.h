#ifndef BOOKINGPOINT_H
#define BOOKINGPOINT_H
#include <QWidget>
#include <QTableWidget>
#include <QTableView>
#include <QLabel>
#include <QDate>
#include <QStandardItemModel>
#include <QMap>

class BookingPoint : public  QWidget
{
    Q_OBJECT
public:
    BookingPoint() ;
    explicit BookingPoint(QWidget *parent = nullptr);
    void bookingActualize(QDate mainDate);
    void getAppartements(QDate mainDate);

private:
    QLabel *currentMonthLable;
    QTableView *table;
    QDate currentDate =  QDate::currentDate();
    QStandardItemModel  *model;
    QMap <int,int> appartmenstMap;
    QMap <QDate,int> dateMap;
    QMap <QStandardItem*,int> bookingMap;
    void bookindEdit(QModelIndex index);
};

#endif // BOOKINGPOINT_H
