#include "bookingpoint.h"
#include <QVBoxLayout>
#include <QDebug>
#include "core/esqlquery.h"
#include <QPushButton>
#include <QBrush>
#include "bookingitem.h"
#include "bookingdialog.h"

BookingPoint::BookingPoint(QWidget *parent)
{
    table = new QTableView(parent);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QVBoxLayout *bookingLayout= new QVBoxLayout;
    QHBoxLayout *monthLayout = new QHBoxLayout;
    QPushButton *leftDateButton = new QPushButton("<<");
    currentMonthLable = new QLabel();

    currentMonthLable->setStyleSheet("QLabel { "
                                     " qproperty-alignment: AlignCenter; font-weight: bold; font-size: 14pt;"
                                     " }" );
    QPushButton *rightDateButton = new QPushButton(">>");

    monthLayout->addWidget(leftDateButton);
    monthLayout->addWidget(currentMonthLable);
    monthLayout->addWidget(rightDateButton);
    bookingLayout->addLayout(monthLayout);
    bookingLayout->addWidget(table);
    this->setLayout(bookingLayout);

    connect(leftDateButton, &QPushButton::clicked, [&]() { currentDate = currentDate.addDays(-10);  bookingActualize(currentDate);   });
    connect(rightDateButton, &QPushButton::clicked, [&]() { currentDate = currentDate.addDays(10);  bookingActualize(currentDate);   });

    connect(table,&QTableView::doubleClicked,this,&BookingPoint::bookindEdit);

    bookingActualize(currentDate);
}

void BookingPoint::bookingActualize(QDate mainDate)
{
    QString monthName = mainDate.toString("dd MMMM yyyy");
    QDate leftDate = mainDate.addDays(-7);
    QDate rightDate = mainDate.addDays(23);
    monthName = monthName.toUpper();
    currentMonthLable->setText(monthName);
    getAppartements(mainDate);
    table->clearSpans();

    ESqlQuery query;

    query.prepare("SELECT bookings.id, apartment_id, (SELECT MAX(start) FROM (VALUES (start_date), (:leftdate)) AS Starts(start)) AS start_date, "
                  " EXTRACT(DAY FROM (SELECT MIN(stop) FROM (VALUES (stop_date), (:rightdate )) AS Stops(stop)) - (SELECT MAX(start) FROM (VALUES (start_date), (:leftdate)) AS Starts(start))),"
                  " persons_profiles.firstname, "
                  " booking_status FROM bookings "
                  " LEFT JOIN persons_profiles ON persons_profiles.id=bookings.personsprofileid "
                  " WHERE (bookings.start_date BETWEEN :leftdate AND :rightdate) OR "
                  " (bookings.stop_date BETWEEN :leftdate AND :rightdate)");

    query.bindValue(":leftdate",leftDate);
    query.bindValue(":rightdate",rightDate);
    query.exec();
    QBrush *b =  new QBrush(Qt::gray);


    // Надо сделать кастомный QStandardItem, для дабл клика по нему

    while(query.next()){
        QStandardItem *myItem = new QStandardItem(query.value(4).toString());
        bookingMap[myItem] = query.value(0).toInt();
        int bookingStatus  = query.value(5).toInt();
        if (bookingStatus==0)
            b->setColor(Qt::gray);
        if (bookingStatus==1)
            b->setColor(Qt::yellow);
        if (bookingStatus==2)
            b->setColor(Qt::blue);
        if (bookingStatus==3)
            b->setColor(Qt::green);
        if (bookingStatus==4)
            b->setColor(Qt::red);
        if (bookingStatus==5)
            b->setColor(Qt::cyan);
        if (bookingStatus==6)
            b->setColor(Qt::darkGreen);
        if (bookingStatus==7)
            b->setColor(Qt::darkYellow);
        if (bookingStatus==8)
            b->setColor(Qt::darkGray);
        if (bookingStatus==9)
            b->setColor(Qt::lightGray);

        myItem->setBackground(*b);
        int y = dateMap[query.value(2).toDate()];
        int x = appartmenstMap.key(query.value(1).toInt());
        table->setSpan(x,y,1,query.value(3).toInt());
        model->setItem(x,y,myItem);
    }

}

void BookingPoint::getAppartements(QDate mainDate)
{

    // пока такой костыль, сохраним максимальный ID остальные скороем,
    ESqlQuery query;
    query.prepare("SELECT max(id) FROM apartments");

    query.exec(); query.next();
    int setRowCount = query.value(0).toInt();
    model = new QStandardItemModel(setRowCount,26);
    model->clear();
    query.prepare("SELECT id, shortname, parent_id FROM apartments WHERE id<>1");
    query.exec();

    int a=0;
    appartmenstMap.clear();
    while (query.next()){
        QStandardItem *item = new QStandardItem(query.value(1).toString());
        QBrush Brush(Qt::darkRed);

        if(  query.value(2).toInt()==1        )
            item->setForeground(Brush);

        appartmenstMap[a] = query.value(0).toInt();
        model->setVerticalHeaderItem(a,item);
        a++;
    }

    int column  = 0;
    dateMap.clear();
    for (int i=-7; i<23; i++)
    {
        QDate currentDate = mainDate.addDays(i);
        dateMap[currentDate] = column;
        QString dayName = currentDate.toString("dd/ddd");
        if (dayName.contains("сб") || dayName.contains("вс"))
        { QStandardItem *item = new QStandardItem(QString(dayName));
            QBrush Brush(Qt::red);
            item->setForeground(Brush);
            model->setHorizontalHeaderItem(column, item);
        }
        else
            model->setHorizontalHeaderItem(column,new QStandardItem(QString(dayName)));

        column++;
    }
    table->setModel(model);


    for (int i = 0 ; i< model->columnCount(); i++)
        table->setColumnWidth(i,55);
}

void BookingPoint::bookindEdit(QModelIndex index)
{
    //    QStandardItem *item = new QStandardItem();
    QStandardItem *item = model->itemFromIndex(index);

    if (item==Q_NULLPTR)
    {qDebug() << "empty item";}
    qDebug() << "ID" << bookingMap[item] << "APP1" << appartmenstMap[ index.row()];


    BookingDialog *dialog = Q_NULLPTR;
    if (bookingMap[item]>0){
    dialog = new BookingDialog(bookingMap[item]);
    }
    else {
        dialog = new BookingDialog(0,dateMap.key(index.column()),appartmenstMap[index.row()],this);
    }
    dialog->exec();
    delete  dialog;
    bookingActualize(currentDate);
}
