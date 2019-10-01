#include "paydocshow.h"
#include "QVBoxLayout"
#include "QDebug"

payDocShow::payDocShow(QWidget *parent, int bookinkId)
{
    qDebug() << "booking ID" << bookinkId;
            PayDocReport * payWidget =  new  PayDocReport(parent);
            payWidget->setBooking(bookinkId);
            QVBoxLayout *mainLayout = new QVBoxLayout();
            mainLayout->addWidget(payWidget);
            setLayout(mainLayout);
}
