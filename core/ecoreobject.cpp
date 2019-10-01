#include "ecoreobject.h"
#include "esqlquery.h"
//#include "edatetime.h"
#include "defines/prefix.h"
//#include "esettings.h"
#include <initializer_list>

ECoreObject* ECoreObject::m_Instance = Q_NULLPTR;

ECoreObject::ECoreObject()
{
#ifdef QT_NO_DEBUG
    p_userId = -1;
    p_allmethods = false;
#else
    p_userId = 0;
    p_userTitle = QObject::tr("Отладка");
//    p_allmethods = true;
#endif
    p_frPass = "30";
    p_clubId = -1;
    moduleAccessId = 0;

    //    reloadOptions();
}

bool ECoreObject::tryAuthorize(QString login, QString password)
{

    QByteArray passwordByteArray = password.toUtf8();
    if (passwordByteArray.size() != 0)
        passwordByteArray = QCryptographicHash::hash(passwordByteArray, QCryptographicHash::Sha256).toHex();


    ESqlQuery query;
    query.prepare("SELECT id, shortname, printname, vat_number FROM access_users "
                  "WHERE login = $1 "
                  "  AND CASE WHEN $2 = '' THEN password IS NULL OR password = $2 ELSE password = $2 END;");
    query.addBindValue(login);
    query.addBindValue(static_cast<QString>(passwordByteArray));
    if (query.exec() && query.next())
    {
        setUserId(query.value(0).toInt());
        setUserName(query.value(1).toString());
        setUserPrintTitle(query.value(2).toString());
        setUserVatNum(query.value(3).toString());
        return true;
    }

    return false;
}

//bool ECoreObject::tryAccessProgram()
//{
//#ifndef QT_NO_DEBUG
//    return true;
//#endif
//    p_allmethods = checkAllAccess();
//    if(p_allmethods)
//        return true;

//    // --- check access details ---
//    qint64 idstart = -1;
//    ESqlQuery query;

//    QString appname = QCoreApplication::applicationName();
//    if(appname == "sip-wizard")
//        appname = "manager";
//    if(appname == "launcher")
//    {
//         query.prepare("SELECT cmdarg FROM access_mgroups WHERE cmdarg!=''");
//         query.exec();
//         while (query.next())
//         {
//            if(ECoreObject::tryAccessProgram(query.value(0).toString()))
//                appname = query.value(0).toString();
//         }
//    }

//    query.prepare("SELECT id FROM access_mgroups WHERE cmdarg = ?");
//    query.addBindValue(appname);
//    query.exec();
//    if(query.next())
//    {
//        moduleAccessId = query.value(0).toInt();
//        idstart = moduleAccessId * 10000;
//    }

//    query.prepare(
//        "SELECT access_mlinks.access_mitem_id"
//        " FROM access_mlinks"
//        " WHERE (access_mlinks.access_mitem_id BETWEEN ? AND ?)"
//        "   AND access_mlinks.access_group_id IN("
//        "       SELECT access_group_id"
//        "       FROM access_links"
//        "       WHERE access_user_id = ?)");
//    query.addBindValue(idstart);
//    query.addBindValue(idstart + 9999);
//    query.addBindValue(userId());
//    query.exec();
//    while(query.next())
//        p_accessList << query.value(0).toInt() - idstart;
//    return p_accessList.contains(0);
//}


//bool ECoreObject::tryAccessProgram(QString program)
//{
//#ifndef QT_NO_DEBUG
//    return true;
//#endif

//    // --- check all access ---
//    if(checkAllAccess())
//        return true;

//    // --- check access details ---
//    qint64 idstart = -1;

//    ESqlQuery query;
//    query.prepare("SELECT id FROM access_mgroups WHERE cmdarg = ?");
//    query.addBindValue(program);
//    query.exec();

//    if(query.next())
//        idstart = query.value(0).toInt() * 10000;

//    query.prepare(
//        "SELECT access_mlinks.access_mitem_id"
//        " FROM access_mlinks"
//        " WHERE (access_mlinks.access_mitem_id = ?)"
//        "   AND access_mlinks.access_group_id IN("
//        "       SELECT access_group_id"
//        "       FROM access_links"
//        "       WHERE access_user_id = ?)");
//    query.addBindValue(idstart);
//    query.addBindValue(userId());
//    query.exec();
//    return query.next();
//}

//bool ECoreObject::checkAllAccess()
//{
//#ifndef QT_NO_DEBUG
//    return true;
//#endif
//    ESqlQuery query;
//    query.prepare(
//        "SELECT MAX(allmethods::integer)"
//        " FROM access_groups"
//        " WHERE id IN("
//        "   SELECT access_group_id"
//        "   FROM access_links"
//        "   WHERE access_user_id = ?)");
//    query.addBindValue(userId());
//    query.exec();
//    if(query.next())
//        return query.value(0).toBool();
//    return false;
//}

//bool ECoreObject::tryAccesMode(int code)
//{
//    return (p_allmethods||p_accessList.contains(code));
//}

//bool ECoreObject::checkAcces(std::initializer_list<int> fullCodes, const bool defaultAccess)
//{
//    for(int fullCode : fullCodes)
//        if(checkAcces(fullCode, defaultAccess))
//            return true;
//    return false;
//}

//bool ECoreObject::checkAcces(const int fullCode, const bool defaultAccess)
//{
//    if(p_allmethods)
//        return true;

//    int moduleId = fullCode / 10000;
//    if(moduleId != moduleAccessId)
//        return defaultAccess;
//    int code = fullCode - moduleId * 10000;
//    return tryAccesMode(code);
//}

//// --- options ---
//bool ECoreObject::reloadOptions()
//{
//    ESqlQuery query;
//    query.prepare("SELECT parameter,value FROM settings_items WHERE parameter IN "
//                  "('club_title','club_address','club_phone','block_performers','block_prolongation'"
//                  ",'mopen_limit','req_check_paysum','allow_denied_entrance','check_pay_days','check_pay_days_allow','check_pay_sum','check_pay_sum_allow'"
//                  ",'minimum_freeze','allow_freeze_rest','date_edit_deny'"
//                  ",'stopover_interval_value','stopover_interval_enabled','separate_key','nodebit_block_outclub','date_edit_deny'"
//                  ",'allow_phone_repeat','allow_phone_repeat_contacts','stopover_interval_allowed','stopover_forcibly','stopover_max_checks'"
//                  ",'lock_inclub_by_fio','warn_contract_ends_in','show_payment_sum_on_search','max_bonus_percent','allow_nondigit_cardnum'"
//                  ",'show_visits_on_search','show_maxvisitcount','allow_merge_anothers_profiles','no_inclub_autodebit','allow_fio_repeat_contacts','login_require_inclub'"
//                  ",'delete_small_freeze','fr_new','fr_prolong','fr_change','fr_break','fr_deposit','allow_unopen_inclub','bonus_type','card_end_length','card_begin_length'"
//                  ",'guest_pay_before_outclub','single_free_guest','delete_unpchecks_on_zreport','money_round_precision','deposit_fiscal','contract_fiscal','checkpay_fiscal'"
//                  ",'map_coordinates','map_zoom','print_km6','use_all_performers', 'enable_return_payinst'"
//                  ",'fr_new_card','fr_prolong_card','fr_change_card','fr_break_card','fr_deposit_card'"
//                  ",'req_recomend','req_event_recomend','kpi_type','kpi_constant','kpi_varying','offer_merge_profiles'"
//                  ",'req_prejournal_performer','req_prejournal_price','deposit_pos','contract_pos','checkpay_pos','req_performer_inclub','deny_change_fiscaldocs','is_multiclub'"
//                  ",'actualize_store','create_store_doc_on','control_store','mdiscount_round_precision','spa_purpose_fields','db_in_header','all_inclub_debit_button','no_debit_others_tarifrank'"
//                  ",'nostrike_locked_blocks','auto_preparation','auto_preparation_type','show_balance','payinst_precision','autodebit_group_visitors'"
//                  ",'allow_email_repeat','allow_socialnetwork_repeat', 'arrearage_control_combobox', 'allow_rent_check', 'allow_rent_default_combo', 'nodebit_free_block_outclub','disable_action_when_unpaid' );");
//    if(!query.exec())
//        return false;
//    while(query.next())
//        p_optionsHash[query.value(0).toString()] = query.value(1);

//    return true;
//}

//QHash<QString,QVariant> ECoreObject::optionMassLoad(const QStringList &keys, QSqlDatabase db)
//{
//    QString parameterString = keys.join("','");
//    parameterString.prepend("'");
//    parameterString.append("'");
//    QString queryString =
//        "SELECT parameter,value "
//        " FROM settings_items "
//        " WHERE parameter IN (%1)";
//    queryString = QString(queryString).arg(parameterString);

//    QHash<QString,QVariant> settings;
//    ESqlQuery query(db);
//    query.exec(queryString);
//    while (query.next())
//        settings[query.value(0).toString()] = query.value(1);

//    QHash<QString,QVariant>::iterator i;
//    for(i = settings.begin(); i != settings.end(); ++i)
//        if(p_optionsHash.contains(i.key()))
//                p_optionsHash[i.key()] = i.value();
//    return settings;
//}

//QVariant ECoreObject::optionLoad(const QString &key, const QVariant &defaultValue, QSqlDatabase db)
//{
//    if(p_optionsHash.contains(key))
//        return p_optionsHash.value(key);
//    ESqlQuery query(db);
//    query.prepare("SELECT value"
//                  " FROM settings_items "
//                  " WHERE parameter = ?");
//    query.addBindValue(key);
//    if(!query.exec() || !query.next())
//        return defaultValue;
//    QVariant value = query.value(0);
//    p_optionsHash[key] = value;
//    return value;
//}

//QVariant ECoreObject::optionValue(const QString &key, const QVariant &defaultValue) const
//{
//    return p_optionsHash.value(key,defaultValue);
//}

//bool ECoreObject::setGlobalOption(const QString &key, const QVariant &value)
//{
//    p_optionsHash[key] = value;

//    ESqlQuery query;
//    query.prepare(
//        "UPDATE settings_items"
//        " SET value = ?"
//        " WHERE parameter = ?");
//    query.addBindValue(value);
//    query.addBindValue(key);
//    return query.exec();
//}

//int ECoreObject::actualizeMembershipState(int id)
//{
//    int result = -1;

//    ESqlQuery query;
//    query.prepare("SELECT start,stop,contracts_state_id FROM contracts_memberships WHERE id=?;");
//    query.addBindValue(id);
//    query.exec();
//    if(query.next())
//    {
//        qDebug()<<"actualize"<<"start="<<query.value(0).toDate()<<"stop="<<query.value(1).toDate()<<"satate="<<query.value(2).toInt();
//        int oldState = query.value(2).toInt();
//        bool oldStateIsNull = query.value(2).isNull();
//        result = oldState;
//        if(result==3)
//        {
//            return 3; // заблокировано - не трогаем
//        }

//        QDate startDate = query.value(0).toDate();
//        QDate stopDate = query.value(1).toDate();
//        int freezedDays = 0; // количество использованных дней заморозки
//        bool isFreezed = false;

//        // считаем количество использованных дней заморозки
//        query.prepare("SELECT SUM(dayscount) FROM contracts_freeze WHERE contracts_membership_id=?;");
//        query.addBindValue(id);
//        query.exec();
//        if(query.next())
//        {
//            freezedDays = query.value(0).toInt();
//        }

//        // проверяем на активную заморозку
//        query.prepare("SELECT true FROM contracts_freeze WHERE contracts_membership_id=? AND current_date BETWEEN start AND stop;"); // проверяем контракт на заморозку
//        query.addBindValue(id);
//        query.exec();
//        if(query.next())
//        {
//            isFreezed = true;
//        }

//        // определяем состояние согласно приоритетам
//        if(stopDate.addDays(freezedDays)<EDateTime::currentDate())
//        {
//            result = 2; // закрыто
//        }
//        else if(startDate>EDateTime::currentDate())
//        {
//            result = 0; // неоткрыто
//        }
//        else if(isFreezed)
//        {
//            result = 5; // заморожено
//        }
//        else if(startDate<=EDateTime::currentDate())
//        {
//            result = 1; // открыто
//        }
//        else
//        {
//            return result; // что-то непонятное с состоянием
//        }

//        if( (result!=oldState) || oldStateIsNull ) // отличается новое состояние от старого - обновляем
//        {
//            query.prepare("UPDATE contracts_memberships SET contracts_state_id=? WHERE id=?;");
//            query.addBindValue(result);
//            query.addBindValue(id);
//            query.exec();
//        }
//    }

//    return result;
//}

//int ECoreObject::actualizeProfileType(int id)
//{
//    int result = -1;
//    ESqlQuery query;
//    query.prepare("SELECT persons_type_id FROM persons_profiles WHERE id=?;");
//    query.addBindValue(id);
//    query.exec();
//    if(query.next())
//        result  = query.value(0).toInt();

//    query.prepare("UPDATE persons_profiles SET persons_type_id=3 WHERE id=? AND id IN (SELECT persons_profile_id FROM contracts_memberships WHERE contracts_state_id IN (0,1,5) AND persons_profile_id IS NOT NULL) RETURNING persons_type_id;");
//    query.addBindValue(id);
//    query.exec();
//    if(query.next())
//        result  = query.value(0).toInt();

//    query.prepare("UPDATE persons_profiles SET persons_type_id=4 WHERE id=? AND id IN (SELECT persons_profile_id FROM contracts_memberships WHERE contracts_state_id IN (2,3) AND persons_profile_id IS NOT NULL)"
//                  " AND id NOT IN (SELECT persons_profile_id FROM contracts_memberships WHERE contracts_state_id IN (0,1,5) AND persons_profile_id IS NOT NULL) RETURNING persons_type_id;");
//    query.addBindValue(id);
//    query.exec();
//    if(query.next())
//        result  = query.value(0).toInt();

//    return result;
//}

//BonusMode ECoreObject::bonusMode()
//{
//    return static_cast<BonusMode>(p_optionsHash["bonus_type"].toInt());
//}

//qreal ECoreObject::eRound(const qreal v, QString option) const
//{
//    int precision = optionValue(option).toInt();
//    if(precision==0)
//        return qRound(v);
//    else
//    {
//        static const double doBase = 10.0;
//        qreal doComplete5, doComplete5i;

//        doComplete5 = v * qPow(doBase, (qreal) (precision + 1));

//        if (v < 0.0)
//            doComplete5 -= 5.0;
//        else
//            doComplete5 += 5.0;

//        doComplete5 /= doBase;
//        modf(doComplete5, &doComplete5i);

//        return doComplete5i / qPow(doBase, (qreal) precision);
//    }
//}

//void ECoreObject::startReports(const QStringList addArguments)
//{
//    QStringList arguments;
//    if(addArguments.isEmpty())
//        arguments << "-g="+QString(QCoreApplication::applicationName()=="group"?"fitness":QCoreApplication::applicationName());
//    else
//        arguments << addArguments;

//    startProgram("reports",arguments);
//}

//QString ECoreObject::programFullPath(const QString &program)
//{
//    return QCoreApplication::applicationDirPath()+ QDir::separator() + PROGRAMPREFIX + program;
//}

//QString ECoreObject::programFullPath(const QString &path, const QString &program)
//{
//    return path + QDir::separator() + PROGRAMPREFIX + program;
//}

//void ECoreObject::startProgram(const QString &program, const QStringList addArguments)
//{
//    QStringList arguments;
//    if(ECoreObject::instance()->cardData().isEmpty())
//        arguments << "-u="+ECoreObject::instance()->userName() << "-p="+ECoreObject::instance()->userPassword();
//    else
//        arguments << "-c="+ECoreObject::instance()->cardData();

//    arguments << "-d="+ECoreObject::instance()->dbUuid();

//    arguments << addArguments;

//    qDebug()<<"Start <<"+program+" with arguments "<<arguments;

//    QProcess *reportProcess = new QProcess;
//    reportProcess->start(programFullPath(program), arguments);
//}

//int ECoreObject::detectFiscal(const int &legalId, const int &payType)
//{
//    ESettings settings;
//    int result = settings.value("fiscal_id",0).toInt();
//    if(payType==1)
//    {
//        settings.beginGroup("fiscals");
//        QString number = settings.value(QString::number(legalId)).toString();
//        if(!number.isEmpty())
//        {
//            settings.beginGroup(number);
//            if(settings.value("fiscal_id",-1).toInt()!=-1)
//                result = settings.value("fiscal_id").toInt();
//        }
//    }
//    else if(payType==3)
//    {
//        settings.beginGroup("pos");
//        settings.beginGroup(QString::number(legalId));
//        if(settings.value("fiscal_id",-1).toInt()!=-1)
//            result = settings.value("fiscal_id").toInt();
//    }
//    return result;
//}

//bool ECoreObject::fieldIsAvailable(const QString &key)
//{
//    ESqlQuery query;
//    query.prepare(
//        "SELECT value"
//        " FROM settings_items "
//        " WHERE parameter = ?");
//    query.addBindValue(key);
//    if(query.exec() && query.next())
//        return true;
//    return false;
//}
