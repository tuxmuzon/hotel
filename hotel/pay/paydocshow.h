#ifndef PAYDOCSHOW_H
#define PAYDOCSHOW_H
#include "QDialog"
#include "report/paydocreport.h"

class payDocShow : public  QDialog
{
    Q_OBJECT

public:
    payDocShow( QWidget *parent, int bookinkId);
};

#endif // PAYDOCSHOW_H
