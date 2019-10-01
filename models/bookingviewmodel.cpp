#include "bookingviewmodel.h"

BookingViewModel::BookingViewModel(QObject *parent) : QSqlQueryModel (parent)
{
    headers << "bookingId" << "appartmentsId" << "personsId" << "Номер" << "Дата заезда" << "Дата выезда" << "Статус" << "Цена" << "Всего" << "Оплачено" << "Комментарий" << "Клиент";
    resetFilter();
}

void BookingViewModel::updateData()
{
    updateQuery();
}

void BookingViewModel::setFilterByApartment(int apartment)
{
    filterApartmentIds = apartment;
    updateQuery();
}

void BookingViewModel::setFilterByClient(int client)
{
    filterClientIds = client;
    updateQuery();
}

void BookingViewModel::updateQuery()
{
    ESqlQuery query;
    query.prepare(" SELECT bookings.id, apartments.id, persons_profiles.id, apartments.shortname, bookings.start_date, stop_date, "
                  "booking_status.shortname, cost, total, payd, bookings.comment,"
                  " concat_ws(' ', firstname, secondname, lastname) "
                  " FROM bookings"
                  " LEFT JOIN persons_profiles ON bookings.personsprofileid = persons_profiles.id"
                  " LEFT JOIN booking_status ON booking_status.id = bookings.booking_status"
                  " LEFT JOIN apartments ON apartment_id = apartments.id"
                  " WHERE CASE WHEN $1 = -1 THEN TRUE ELSE apartments.id = $1 END"
                  "    AND CASE WHEN $2 = -1 THEN TRUE ELSE persons_profiles.id = $2 END;");
    query.addBindValue(filterApartmentIds);
    query.addBindValue(filterClientIds);
    query.exec();
    setQuery(query);
}

QVariant BookingViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal)
    {
        if (section < headers.size())
            return headers.at(section);
    }

    return QSqlQueryModel::headerData(section, orientation, role);
}

void BookingViewModel::resetFilter()
{
    filterApartmentIds = -1;
    filterClientIds = -1;
    updateData();
}

int BookingViewModel::getAppartmentId(const QModelIndex &ind)
{
    QModelIndex selectedIndex = index(ind.row(), 1);
    return data(selectedIndex).toInt();
}

int BookingViewModel::getBookingId(const QModelIndex &ind)
{
    QModelIndex selectedIndex = index(ind.row(), 0);
    return data(selectedIndex).toInt();
}

int BookingViewModel::getPersonsId(const QModelIndex &ind)
{
    QModelIndex selectedIndex = index(ind.row(), 2);
    return data(selectedIndex).toInt();
}
