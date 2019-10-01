#ifndef PAYDOCREPORT_H
#define PAYDOCREPORT_H
#include <QObject>
#include <QWidget>
#include "QTableView"
#include "../core/esqlquerymodel.h"

class PayDocReport : public QWidget
{
    Q_OBJECT
public:
    explicit PayDocReport(QWidget *parent = nullptr);
    void setBooking(int bookingId);
    void actualze();

private:
    //   ESqlQueryModel model;

    QTableView*  table;
    QString filter;
    void print(const QModelIndex &index);

signals:

public slots:
};

#endif // PAYDOCREPORT_H
