#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableWidget>
#include <QLabel>
#include <QStandardItemModel>
#include <cash/cashservicedialog.h>
#include "QVBoxLayout"
#include "QPushButton"
#include "QTableView"
#include "fiscal/atoldev9fiscalobject.h"
#include "hotel/profileedit.h"
#include "QDebug"
#include "QMessageBox"
#include "QFile"
#include "QSqlQuery"
#include "QSqlError"
#include "QGroupBox"
#include "QRadioButton"
#include "QItemSelectionModel"
#include "QLabel"
#include "QInputDialog"
#include "QFileInfo"
#include "QDateTime"
#include "QTableWidget"
#include "bookingtabwidget.h"
#include "auth/authdialog.h"
#include <QApplication>
#include <QHash>
#include "models/pesonssqltablemodel.h"
#include "report/shiftreport.h"
#include "report/paydocreport.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    QSqlDatabase *db ;
    QSqlQueryModel *goodsModel;
    PesonsSqlTableModel *personsModel;
    QTableView *services;
    QTableWidget *saleDocument;
    ShiftReport *shiftWidget;
    PayDocReport *doctWidget;

    QWidget *personsPoint = new QWidget(this);
    QTabWidget *tabWidget;
    BookingTabWidget *bookings = new BookingTabWidget();

    //  s  QTableWidget *bookingTable;
    QTableView *personsTable;
    ~MainWindow() override;

private:
    int profileId =0 ;
    void connectDB();
    QWidget* saleWidgetGet();
    void hotelWidgetGen();
    void addRowsInSaleDocument();
    void slot_current_changed(int index);
    void printFiscalDocument();
    void editCountRowsInSaleDocument();
    void dellRowsInSaleDocument();
    void updateWindow();
    void serviceOpertionView();
    bool saveFiscalString(FISCALSTRING str, QString docNum, QString shiftNumber, payType pay_Type = payTypeCash, operationType operation_Type = sale, int personId = 0, QString comment="");
    void totalSum();
    QLabel *total;
    QLabel *shiftNumber;
    QLabel *shiftStatus;

    QRadioButton *saleRadio;
    QRadioButton *depositRadio;
    QRadioButton *returnRadio;

public slots:
    void personsActualize(const QString &filterString=QString(""));
};

#endif // MAINWINDOW_H
