#ifndef SHIFTREPORT_H
#define SHIFTREPORT_H

#include <QObject>
#include <QWidget>
#include "QTableView"
#include "../core/esqlquerymodel.h"

class ShiftReport : public QWidget
{
    Q_OBJECT
public:
    explicit ShiftReport(QWidget *parent = nullptr);
    void actualze();

private:
    //   ESqlQueryModel model;

    QTableView*  table;

signals:

public slots:
};

#endif // SHIFTREPORT_H
