#ifndef BOOKINGVIEWMODEL_H
#define BOOKINGVIEWMODEL_H
#include <QSqlQueryModel>
#include "core/esqlquery.h"

class BookingViewModel : public QSqlQueryModel
{
public:
    explicit BookingViewModel(QObject *parent = nullptr);
    void updateData();
    void setFilterByApartment(int apartment);
    void setFilterByClient(int client);
    void resetFilter();
    int getAppartmentId(const QModelIndex &ind);
    int getBookingId(const QModelIndex &ind);
    int getPersonsId(const QModelIndex &ind);
private:
    QStringList headers;
    int filterApartmentIds;
    int filterClientIds;
    void updateQuery();

public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

Q_SIGNALS:


};

#endif // BOOKINGVIEWMODEL_H
