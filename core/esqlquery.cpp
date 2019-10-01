#include "esqlquery.h"
#include <QSqlError>
#include <QDebug>
#include <QtCore>
#include <QSqlField>
#include <QSqlDriver>

//#include "ecoreeventfilter.h"

bool ESqlQuery::exec(const QString &query)
{
    if(checkConnection && !db.isOpen())
    {
//        QCoreApplication::postEvent(QCoreApplication::instance(),new QEvent(ECoreEventFilter::disconnectedEventType()));
        return false;
    }

    int ret = QSqlQuery::exec(query);

    if(ret)
        qDebug()<<lastQuery();
    else
        qWarning()<<lastQuery()<<lastError();

    return ret;
}

bool ESqlQuery::exec()
{
    if(checkConnection && !db.isOpen())
    {
//        QCoreApplication::postEvent(QCoreApplication::instance(),new QEvent(ECoreEventFilter::disconnectedEventType()));
        return false;
    }
    int ret = QSqlQuery::exec();

    if(!ret)
        qWarning()<<QSqlQuery::lastQuery()<<QSqlQuery::lastError();

//    TODO убрать после отладки
//    if(!ret)
//        qWarning()<<QSqlQuery::lastQuery()<<QSqlQuery::boundValues()<<QSqlQuery::lastError();

#ifndef QT_NO_DEBUG
    qDebug()<<QSqlQuery::lastQuery()<<QSqlQuery::boundValues();
#endif

    return ret;
}

bool ESqlQuery::execBatch()
{
    if(checkConnection && !db.isOpen())
    {
//        QCoreApplication::postEvent(QCoreApplication::instance(),new QEvent(ECoreEventFilter::disconnectedEventType()));
        return false;
    }
    int ret = QSqlQuery::execBatch();
    if(!ret)
        qWarning()<<QSqlQuery::lastQuery()<<QSqlQuery::lastError();

#ifndef QT_NO_DEBUG
    qDebug()<<QSqlQuery::lastQuery()<<QSqlQuery::boundValues();
#endif
    return ret;
}

QString ESqlQuery::executedQueryWithValues()
{
    QString sql = executedQuery();
    int nbBindValues = boundValues().size();

    for(int i = 0, j = 0; j < nbBindValues;)
    {
        int s = sql.indexOf(QLatin1Char('\''), i);
        i = sql.indexOf(QLatin1Char('?'), i);
        if (i < 1)
            break;

        if(s < i && s > 0)
        {
            i = sql.indexOf(QLatin1Char('\''), s + 1) + 1;
            if(i < 2)
                break;
        }
        else
        {
            const QVariant &var = boundValue(j);
            QSqlField field(QLatin1String(""), var.type());
            if (var.isNull())
                field.clear();
            else
                field.setValue(var);
            QString formatV = driver()->formatValue(field);
            sql.replace(i, 1, formatV);
            i += formatV.length();
            ++j;
        }
    }
    return sql;
}

QList<int> ESqlQuery::getResultsAsIntegerList(const int &column)
{
    QList<int> integers;
    while (next())
        integers << value(column).toInt();
    return integers;
}

QStringList ESqlQuery::getResultsAsStringList(const int &column)
{
    QStringList values;
    while (next())
        values << value(column).toString();
    return values;
}

void ESqlQuery::bindValues(const QVariantHash &values, QSql::ParamType type)
{
    QHashIterator<QString, QVariant> iterator(values);
    while(iterator.hasNext())
    {
        iterator.next();
        bindValue(":" + iterator.key(), iterator.value(), type);
    }
}
