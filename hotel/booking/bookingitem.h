#ifndef BOOKINGITEM_H
#define BOOKINGITEM_H
#include "QStandardItem"

class BookingItem : public  QStandardItem
{
public:
    BookingItem();
    void setBookingId(int id){bookingId=id;}
    int getBookigId(){return  bookingId;}
private:
   int bookingId = 1;
};

#endif // BOOKINGITEM_H
