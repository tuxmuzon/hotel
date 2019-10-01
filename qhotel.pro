#-------------------------------------------------
#
# Project created by QtCreator 2018-04-13T19:26:05
#
#-------------------------------------------------

QT  += core gui sql network widgets webengine webenginewidgets serialport
TARGET = qhotelsalepoint
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    auth/dbsettingdialog.cpp \
    cash/cashwidget.cpp \
    hotel/pay/paydocshow.cpp \
        main.cpp \
        mainwindow.cpp \
    fiscal/abstractdevfiscalobject.cpp \
    fiscal/atoldev9fiscalobject.cpp \
    cash/cashservicedialog.cpp \
    hotel/profileedit.cpp \
    core/esqlquery.cpp \
    hotel/personselectdialog.cpp \
    bookingtabwidget.cpp \
    models/bookingviewmodel.cpp \
    hotel/booking/bookingdialog.cpp \
    hotel/booking/bookingpoint.cpp \
    hotel/booking/bookingitem.cpp \
    core/ecolorsqlquerymodel.cpp \
    core/esqlquerymodel.cpp \
    core/esqlcombobox.cpp \
    core/ecoreobject.cpp \
    auth/authdialog.cpp \
    hotel/pay/paydialog.cpp \
    mail_out/dialogs/maillistdialog.cpp \
    mail_out/widgets/maillistwidget.cpp \
    mail_out/widgets/smswidget.cpp \
    mail_out/senders/gsmsender.cpp \
    mail_out/dialogs/personsmailout.cpp \
    models/pesonssqltablemodel.cpp \
    report/paydocreport.cpp \
    report/shiftreport.cpp


HEADERS += \
    auth/dbsettingdialog.h \
    cash/cashwidget.h \
    hotel/pay/paydocshow.h \
        mainwindow.h \
    fiscal/abstractdevfiscalobject.h \
    fiscal/atoldev9fiscalobject.h \
    cash/cashservicedialog.h \
    hotel/profileedit.h \
    core/esqlquery.h \
    hotel/personselectdialog.h \
    3d-party/libfptr10.h \
    bookingtabwidget.h \
    models/bookingviewmodel.h \
    hotel/booking/bookingdialog.h \
    hotel/booking/bookingpoint.h \
    hotel/booking/bookingitem.h \
    core/ecolorsqlquerymodel.h \
    core/esqlquerymodel.h \
    core/esqlcombobox.h \
    core/ecoreobject.h \
    auth/authdialog.h \
    core/defines/prefix.h \
    hotel/pay/paydialog.h \
    mail_out/dialogs/maillistdialog.h \
    mail_out/widgets/maillistwidget.h \
    mail_out/widgets/smswidget.h \
    mail_out/senders/gsmsender.h \
    mail_out/dialogs/personsmailout.h \
    models/pesonssqltablemodel.h \
    report/paydocreport.h \
    report/shiftreport.h

unix:!macx: LIBS += -lfptr10

INCLUDEPATH += $$PWD/3d-party
DEPENDPATH += $$PWD/3d-party

DISTFILES += \
    scheme/1.sql \
    scheme/basever1.sql \
    scheme/scheme.sql \
    scheme/main.db \
    templates/bookinProfile.html \
    templates/invoice.html \
    templates/reports/shift_report.html

!android: DESTDIR = ../bin

RESOURCES += \
    templates.qrc \
    img.qrc
