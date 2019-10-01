#ifndef ECOREOBJECT_H
#define ECOREOBJECT_H

#include <QtCore>
#include <QtSql>
#include <QCryptographicHash>
//#include "defines/bonusmode.h"
//#include "esqldatabase.h"

class ECoreObject
{
public:
    static ECoreObject* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new ECoreObject;

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = Q_NULLPTR;
        mutex.unlock();
    }

private:
    ECoreObject();

    ECoreObject(const ECoreObject &); // hide copy constructor
    ECoreObject& operator=(const ECoreObject &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static ECoreObject* m_Instance;

    // --- main ---
public:
    // user
    void setUserId(const int userId) { p_userId = userId; }
    int userId() const { return p_userId; }
    void setFrPassword(const QString &frPass) { p_frPass = frPass; }
    QString frPassword() const { return p_frPass; }
    void setUserTitle(const QString &userTitle) { p_userTitle = userTitle; }
    QString userTitle() const { return p_userTitle; }
    void setUserName(const QString &userName) { p_userName = userName; }
    QString userName() const { return p_userName; }
    void setUserPrintTitle(const QString &userPrintTitle) { p_userPrintTitle = userPrintTitle; }
    QString userPrintTitle() const { return p_userPrintTitle; }
    void setUserVatNum(const QString &userVatNum) { p_userVatNum = userVatNum; }
    QString userVatNum() const { return p_userVatNum; }
    void setUserPassword(const QString &userPassword) { p_userPassword = userPassword; }
    QString userPassword() const { return p_userPassword; }
//    void setCardData(const QString &cardData) { p_cardData = cardData; }
//    QString cardData() const  { return p_cardData; }
    // db
    void setDbTitle(const QString &title) { p_dbTitle = title; }
    QString dbTitle() const { return p_dbTitle; }
    void setDbUuid(const QString &uuid) { p_dbUuid = uuid; }
    QString dbUuid() const { return p_dbUuid; }
    bool tryAuthorize(QString login, QString password);
//    bool tryAccessProgram();
//    bool tryAccessProgram(QString program);
//    bool checkAllAccess();
//    bool tryAccesMode(int code);
//    bool checkAcces(std::initializer_list<int> fullCodes, const bool defaultAccess = false);
//    bool checkAcces(const int fullCode, const bool defaultAccess = false);
//    bool allAccess(){ return p_allmethods; }
//    void setAllAccess(){ p_allmethods=true; }
//    void setClubId(int id){ p_clubId = id; }
//    int clubId(){ return p_clubId; }

    // options
//    bool reloadOptions();

//    QHash<QString,QVariant> optionMassLoad(const QStringList &keys, QSqlDatabase db = ESqlDatabaseManager::database());
//    QVariant optionLoad(const QString &key, const QVariant &defaultValue = QVariant(), QSqlDatabase db = ESqlDatabaseManager::database());
//    QVariant optionValue (const QString &key, const QVariant &defaultValue = QVariant()) const;
//    bool setGlobalOption(const QString &key, const QVariant &value);
//    int actualizeMembershipState(int id);
//    int actualizeProfileType(int id);
//    void setOption(const QString &key, const QVariant &value){ p_optionsHash[key] = value; }

//    BonusMode bonusMode();

//    qreal eRound(const qreal v, QString option = "money_round_precision") const;

//    // reports
//    void startReports(const QStringList addArguments = QStringList());
//    static QString programFullPath(const QString &program);
//    static QString programFullPath(const QString &path, const QString &program);
//    void startProgram(const QString &program, const QStringList addArguments = QStringList());

//    int detectFiscal(const int &legalId, const int &payType);
//    static bool fieldIsAvailable(const QString &key);
private:
    QString p_frPass;
    QString p_userTitle;
    QString p_userName;
    QString p_userVatNum;
    QString p_userPassword;
    QString p_userPrintTitle;
//    QString p_cardData;
    QString p_dbTitle;
    QString p_dbUuid;
    int p_userId;
    int p_clubId;
    int moduleAccessId;

    bool p_allmethods;
//    QList<qint64> p_accessList;
//    QHash<QString,QVariant> p_optionsHash;
};

#endif // ECOREOBJECT_H
