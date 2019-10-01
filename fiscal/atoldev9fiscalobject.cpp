#include "atoldev9fiscalobject.h"

#include "QHash"
#include "QLibrary"
#include "QDebug"
#include "QFileInfo"
#include "QString"

//#include "libfptr10.h"

atoldev9fiscalobject::atoldev9fiscalobject(QObject *parent) : AbstractDevFiscalObject(parent)
{

    //Инициализация драйвера
#ifndef Q_OS_MAC
    libfptr_create(&fptr);
    //

    // Запрос версии рафвера
    const  char *version = libfptr_get_version_string();

    qDebug() << "PRINT DRIVEr:" << version;

    libfptr_open(fptr);

    // настройки надо тестить
    wchar_t settings[1024] = {0};
    swprintf(&settings[0], 1024, L"{\"%ls\": %d, \"%ls\": %d, \"%ls\": \"%ls\", \"%ls\": %d}",
            LIBFPTR_SETTING_MODEL, LIBFPTR_MODEL_ATOL_AUTO,
            LIBFPTR_SETTING_PORT, LIBFPTR_PORT_USB,
            LIBFPTR_SETTING_USB_DEVICE_PATH,L"auto");
    //            LIBFPTR_SETTING_COM_FILE, L"COM5",
    //            LIBFPTR_SETTING_BAUDRATE, LIBFPTR_PORT_BR_115200);
    libfptr_set_settings(fptr, settings);


    //Открываем фискалку
    bool isOpened = (libfptr_is_opened(fptr) != 0);

    qDebug() << "open: " << isOpened;
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_DATA_TYPE, LIBFPTR_DT_SHORT_STATUS);
    libfptr_query_data(fptr);

    bool isCashDrawerOpened     = (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_CASHDRAWER_OPENED) != 0);
    bool isPaperPresent         = (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_RECEIPT_PAPER_PRESENT) != 0);
    bool isPaperNearEnd         = (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_PAPER_NEAR_END) != 0);
    bool isCoverOpened          = (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_COVER_OPENED) != 0);
    int shiftState             = (libfptr_get_param_int(fptr, LIBFPTR_PARAM_SHIFT_STATE) != 0);

    qDebug() << "isCashDrawerOpened" << isCashDrawerOpened;
    qDebug() << "isPaperPresent" << isPaperPresent;
    qDebug() << "isPaperNearEnd" << isPaperNearEnd;
    qDebug() << "isCoverOpened" << isCoverOpened;
    qDebug() << "shiftState" << shiftState;


    //    libfptr_set_param_str(fptr, 1021, L"Кассир Администратор");
    //    libfptr_set_param_str(fptr, 1203, L"123456789047");
    //    if (libfptr_operator_login(fptr)!=0)
    //        qDebug() << lastError();

    //    libfptr_set_param_str(fptr, LIBFPTR_PARAM_TEXT, L"Строка");

    //    if (libfptr_print_text(fptr) < 0)
    //    {
    //        int errorCode = libfptr_error_code(fptr);

    //        std::vector<wchar_t> errorDescription(32);
    //        int size = libfptr_error_description(fptr, &errorDescription[0], errorDescription.size());
    //        if (size > errorDescription.size())
    //        {
    //            errorDescription.resize(size);
    //            libfptr_error_description(fptr, &errorDescription[0], errorDescription.size());
    //        }

    //        qDebug() << L"Ошибка - "
    //                 << errorCode
    //                 << L" [" << &errorDescription[0] << L"]"
    //                    ;
    //    }






    //    // указание позиции
    //    libfptr_set_param_str(fptr, LIBFPTR_PARAM_COMMODITY_NAME, L"Товар");
    //    libfptr_set_param_double(fptr, LIBFPTR_PARAM_PRICE, 100);
    //    libfptr_set_param_double(fptr, LIBFPTR_PARAM_QUANTITY, 5.15);
    //    libfptr_set_param_int(fptr, LIBFPTR_PARAM_TAX_TYPE, LIBFPTR_TAX_NO);
    //    libfptr_registration(fptr);




    //    qDebug() << "СДАЧА ПО ЧЕКУ" <<  libfptr_get_param_double(fptr,LIBFPTR_PARAM_CHANGE);



    //    //  Закрытие полностью оплаченного чека

    //    libfptr_close_receipt(fptr);




    //    Проверка закрытия документа (на примере закрытия фискального чека)

    //    // Вспомогательная функция для чтения текста ошибки
    //    std::wstring getErrorDescription(libfptr_handle fptr)
    //    {
    //        std::vector<wchar_t> str(32);
    //        int size = libfptr_error_description(fptr, &str[0], str.size());
    //        if (size > str.size())
    //        {
    //            str.resize(size);
    //            libfptr_error_description(fptr, &str[0], str.size());
    //        }
    //        return std::wstring(&str[0]);
    //    }

    //    libfptr_close_receipt(fptr);

    //    while (libfptr_check_document_closed(fptr) < 0) {
    //        // Не удалось проверить состояние документа. Вывести пользователю текст ошибки, попросить устранить неполадку и повторить запрос
    //        //qDebug() << getErrorDescription(fptr) << std::endl;
    //        continue;
    //    }

    //    if (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_DOCUMENT_CLOSED) == 0) {
    //        // Документ не закрылся. Требуется его отменить (если это чек) и сформировать заново
    //        libfptr_cancel_receipt(fptr);
    //        return;
    //    }

    //    if (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_DOCUMENT_PRINTED) == 0) {
    //        // Можно сразу вызвать метод допечатывания документа, он завершится с ошибкой, если это невозможно
    //        while (libfptr_continue_print(fptr) < 0) {
    //            // Если не удалось допечатать документ - показать пользователю ошибку и попробовать еще раз.
    //            //std::wcout << L"Не удалось напечатать документ (Ошибка \""<< getErrorDescription(fptr) << L"\"). Устраните неполадку и повторите.";
    //            continue;
    //        }
    //    }
#endif

}


QString atoldev9fiscalobject::fiscalStatus()
{
#ifndef Q_OS_MAC
    return QString("");
    //    return checkError(fptr);
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::openFiscalDocument(operationType fiscalDoc, double sum)
{
#ifndef Q_OS_MAC
    if(libfptr_operator_login(fptr)!=0){
        qDebug() << lastError();
        
    }

    if (fiscalDoc== sale)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_RECEIPT_TYPE, LIBFPTR_RT_SELL);
    if (fiscalDoc== returnSale)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_RECEIPT_TYPE, LIBFPTR_RT_SELL_RETURN);
    if (fiscalDoc== inCash) {
        libfptr_set_param_double(fptr, LIBFPTR_PARAM_SUM, sum);
        libfptr_cash_income(fptr);
    }
    if (fiscalDoc== outCash) {
        libfptr_set_param_double(fptr, LIBFPTR_PARAM_SUM, sum);
        libfptr_cash_outcome(fptr);
    }

    if(libfptr_open_receipt(fptr)!=0)
        return lastError();
    else
        return QString("Открыт чек");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::openShift()
{
#ifndef Q_OS_MAC
    // открываем смену
    if(libfptr_open_shift(fptr)!=0)
        return lastError();
    libfptr_check_document_closed(fptr);
    return QString("Открыта смена");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::closeShift()
{
#ifndef Q_OS_MAC
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_REPORT_TYPE, LIBFPTR_RT_CLOSE_SHIFT);
    if(libfptr_report(fptr)!=0)
        return lastError();
    else
        return QString("Print Z-report");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::setAtributeSubjectCalculation()
{
    return QString("");
}

QString atoldev9fiscalobject::printFiscalString(FISCALSTRING str)
{
#ifndef Q_OS_MAC
    wchar_t* array = strTowchar(str.goodName);

    libfptr_set_param_str(fptr, LIBFPTR_PARAM_COMMODITY_NAME, array );
    libfptr_set_param_double(fptr, LIBFPTR_PARAM_PRICE, str.goodPrice);
    libfptr_set_param_double(fptr, LIBFPTR_PARAM_QUANTITY, str.goodCount);
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_TAX_TYPE, LIBFPTR_TAX_NO);

    if(libfptr_registration(fptr)!=0)
        return lastError();
    else return
            QString ("Строка напечатана");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::printNoFiscalString(QString text)
{
#ifndef Q_OS_MAC
    wchar_t* array = strTowchar(text);

    libfptr_set_param_str(fptr, LIBFPTR_PARAM_TEXT, array);


    return QString("");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::cancelFiscalDocument()
{
#ifndef Q_OS_MAC
    if(libfptr_cancel_receipt(fptr)!=0)
        return lastError();
    else
        return QString("cancel fisсal document");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}


QString atoldev9fiscalobject::closeFiscalDocument()
{
#ifndef Q_OS_MAC
    if(libfptr_close_receipt(fptr)!=0)
        return lastError();
    else
        return QString("Fisacl doc is closed");

    //    Проверка закрытия документа (на примере закрытия фискального чека)

    //    // Вспомогательная функция для чтения текста ошибки

    //    libfptr_close_receipt(fptr);

    //    while (libfptr_check_document_closed(fptr) < 0) {
    //      // Не удалось проверить состояние документа. Вывести пользователю текст ошибки, попросить устранить неполадку и повторить запрос
    //      std::wcout << getErrorDescription(fptr) << std::endl;
    //      continue;
    //    }

    //    if (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_DOCUMENT_CLOSED) == 0) {
    //      // Документ не закрылся. Требуется его отменить (если это чек) и сформировать заново
    //      libfptr_cancel_receipt(fptr);
    //      return;
    //    }

    //    if (libfptr_get_param_bool(fptr, LIBFPTR_PARAM_DOCUMENT_PRINTED) == 0) {
    //      // Можно сразу вызвать метод допечатывания документа, он завершится с ошибкой, если это невозможно
    //      while (libfptr_continue_print(fptr) < 0) {
    //        // Если не удалось допечатать документ - показать пользователю ошибку и попробовать еще раз.
    //        std::wcout << L"Не удалось напечатать документ (Ошибка \""<< getErrorDescription(fptr) << L"\"). Устраните неполадку и повторите.";
    //        continue;
    //      }
    //    }
#else
    return QString("Not support OS");
#endif
}

QString atoldev9fiscalobject::showProperties()
{
#ifndef Q_OS_MAC
    //    if(ifptr->ShowProperties() < 0)
    //        return checkError(ifptr);
    //    else QString("0");
    return QString("");
#endif
}

QString atoldev9fiscalobject::payFiscalDocument(double sum, payType type)
{
#ifndef Q_OS_MAC
    //    //регистрация оплаты

    if (type==payTypeCash)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_PAYMENT_TYPE, LIBFPTR_PT_CASH);
    if (type==payTypeElectronic)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_PAYMENT_TYPE, LIBFPTR_PT_ELECTRONICALLY);
    if (type==payTypCredit)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_PAYMENT_TYPE,  LIBFPTR_PT_CREDIT);
    if (type==payTypeExpense)
        libfptr_set_param_int(fptr, LIBFPTR_PARAM_PAYMENT_TYPE,  LIBFPTR_PT_PREPAID);

    libfptr_set_param_double(fptr, LIBFPTR_PARAM_PAYMENT_SUM, sum);
    if(libfptr_payment(fptr)!=0)
        return lastError();

    //    LIBFPTR_PARAM_REMAINDER	Неоплаченный остаток чека	double
    //    LIBFPTR_PARAM_CHANGE	Сдача по чеку	double


    return QString("Внесена оплата на сумму")+QString::number(sum)+QString("ТИП:") + QString::number(type);
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}
QString atoldev9fiscalobject::printXReport()
{
#ifndef Q_OS_MAC
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_REPORT_TYPE, LIBFPTR_RT_X);
    if(libfptr_report(fptr)!=0)
        return lastError();

    return QString("X report Print");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}
QString atoldev9fiscalobject::getShiftNumber()
{   // протестировать, + режим многих фискальников. Логичнее создавать в каждом обьекте драйвера через настройки
#ifndef Q_OS_MAC
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_DATA_TYPE, LIBFPTR_DT_SHIFT_STATE);
    libfptr_query_data(fptr);

    // на будущее
    int state   = static_cast<int>(libfptr_get_param_int(fptr, LIBFPTR_PARAM_SHIFT_STATE));
    Q_UNUSED(state);
    int year, month, day, hour, minute, second;
    libfptr_get_param_datetime(fptr, LIBFPTR_PARAM_DATE_TIME, &year, &month, &day, &hour, &minute, &second);

    int number  = static_cast<int>(libfptr_get_param_int(fptr, LIBFPTR_PARAM_SHIFT_NUMBER));

    if (number>1)
        return QString::number(number);
    else return QString("Ошибка КАССЫ!");
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::setOperator(QString p_shortName, QString p_vatNumber)
{
#ifndef Q_OS_MAC
    wchar_t* shortName = strTowchar(p_shortName);
    wchar_t* vatNumber = strTowchar(p_vatNumber);
    libfptr_set_param_str(fptr, 1021, shortName);
    libfptr_set_param_str(fptr, 1203, vatNumber);
    if(libfptr_operator_login(fptr)!=0)
        return lastError();
    return QString("Установлен оператор: ")+ p_shortName + QString(" ИНН: ") + p_vatNumber;
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::lastError()
{
#ifndef Q_OS_MAC
    std::vector<wchar_t> str(32);
    int size = libfptr_error_description(fptr, &str[0], static_cast<int>(str.size()) );
    if (size > static_cast<int>(str.size()))
    {
        str.resize(size);
        libfptr_error_description(fptr, &str[0], str.size());
    }

    return QString ("Ошибка ФР: ") + QString::fromWCharArray(&str[0]);
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

QString atoldev9fiscalobject::lastDocumet()
{
#ifndef Q_OS_MAC
    libfptr_set_param_int(fptr, LIBFPTR_PARAM_FN_DATA_TYPE, LIBFPTR_FNDT_LAST_DOCUMENT);
    libfptr_fn_query_data(fptr);

    int documentNumber      = static_cast<int>(libfptr_get_param_int(fptr, LIBFPTR_PARAM_DOCUMENT_NUMBER));

    std::vector<wchar_t> str(1024);
    int size = libfptr_get_param_str(fptr, LIBFPTR_PARAM_FISCAL_SIGN, &str[0], str.size());
    if (size > str.size())
    {
        str.resize(size);
        libfptr_get_param_str(fptr, LIBFPTR_PARAM_FISCAL_SIGN, &str[0], str.size());
    }
    std::wstring fiscalSign = std::wstring(&str[0]);

    int year, month, day, hour, minute, second;
    libfptr_get_param_datetime(fptr, LIBFPTR_PARAM_DATE_TIME, &year, &month, &day, &hour, &minute, &second);

    if(documentNumber>0)
        return  QString::number(documentNumber);
    else
        return lastError();
#else
    return QString("MAC OS NOT SUPPORT");
#endif
}

wchar_t *atoldev9fiscalobject::strTowchar(QString str)
{
    wchar_t* array = new wchar_t[static_cast<unsigned long>(str.length() + 1)];
    array[str.toWCharArray(array)] = '\0';
    return  array;
}
