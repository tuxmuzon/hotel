#include "esqlquerymodel.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QRegExp>
#include "esqlquery.h"

ESqlQueryModel::ESqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    p_fixedSortColumn = -1;
    p_fixedSortOrder = Qt::AscendingOrder;
    p_fixedPriorSortColumn = -1;
    p_fixedPriorSortOrder = Qt::AscendingOrder;
}

Qt::ItemFlags ESqlQueryModel::flags(const QModelIndex &index) const
{
    if(p_flagsHash.contains(index))
        return p_flagsHash[index];
    return (QSqlQueryModel::flags(index)|Qt::ItemIsUserCheckable);
}

QVariant ESqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(QSqlQueryModel::data(index,Qt::EditRole).type()==QVariant::Bool)
    {
        if(role == Qt::CheckStateRole)
            return QSqlQueryModel::data(index,Qt::EditRole).toBool()?Qt::Checked:Qt::Unchecked;
        else if(role==Qt::UserRole)
            return QSqlQueryModel::data(index,Qt::EditRole);
        else
            return QVariant();
    }
    else if(QSqlQueryModel::data(index,Qt::EditRole).type()==QVariant::String && role==Qt::ToolTipRole)
        return "<span>"+QSqlQueryModel::data(index,Qt::EditRole).toString().replace("\n","<br>")+"</span>";
    else
        return QSqlQueryModel::data(index, role);
}

void ESqlQueryModel::setQuery(const QSqlQuery &query)
{
    QSqlQueryModel::setQuery(query);
    p_queryStr = query.executedQuery().remove(QRegExp("ORDER\\ BY.*SC;$",Qt::CaseInsensitive));
    boundValues = query.boundValues();

    if(QSqlQueryModel::lastError().isValid())
        qWarning()<<QSqlQueryModel::query().lastQuery()<<QSqlQueryModel::lastError();

#ifndef QT_NO_DEBUG
    qDebug()<<QSqlQueryModel::query().lastQuery();
#endif
}

void ESqlQueryModel::setQuery(const QString &query, const QSqlDatabase &db)
{
    p_queryStr = query;

    QString queryString = p_queryStr.remove(QRegExp(";$"));
    if(!queryString.isEmpty())
    {
        queryString.append(p_filterStr);
        queryString.append(p_groupStr);
        queryString.append(p_sortStr);
    }

    QSqlQueryModel::setQuery(queryString,db);

    if(QSqlQueryModel::lastError().isValid())
        qWarning()<<QSqlQueryModel::query().lastQuery()<<QSqlQueryModel::lastError();

#ifndef QT_NO_DEBUG
    qDebug()<<QSqlQueryModel::query().lastQuery();
#endif
}

void ESqlQueryModel::setSort(int column, Qt::SortOrder order)
{
    QString orderIdxString;
    QString priorSortString;
    if(p_orderidxHash.contains(column))
        orderIdxString = QString(" %1 %2, ").arg(p_orderidxHash.value(column)+1).arg(order == Qt::AscendingOrder?"ASC":"DESC");

    if(p_fixedPriorSortColumn!=-1)
    {
        priorSortString = QString(" %1 %2, ").arg(p_fixedPriorSortColumn+1).arg(p_fixedPriorSortOrder == Qt::AscendingOrder?"ASC":"DESC");
    }

    if(p_fixedSortColumn==-1)
    {
        p_sortStr = QString(" ORDER BY "+priorSortString+orderIdxString+"%1 %2;").arg(column+1).arg(order == Qt::AscendingOrder?"ASC":"DESC");
    }
    else
    {
        p_sortStr = QString(" ORDER BY "+priorSortString+" %1 %2, "+orderIdxString+"%3 %4;").arg(column+1).arg(order == Qt::AscendingOrder?"ASC":"DESC")
                .arg(p_fixedSortColumn+1).arg(p_fixedSortOrder == Qt::AscendingOrder?"ASC":"DESC");
    }
}

void ESqlQueryModel::sort(int column, Qt::SortOrder order)
{
    setSort(column, order);
    if(boundValues.isEmpty())
        setQuery(p_queryStr);
    else
    {
        if(p_queryStr.isEmpty())
            return;

        ESqlQuery query;
        query.prepare(p_queryStr+p_filterStr+p_groupStr+p_sortStr);
        QMapIterator<QString, QVariant> i(boundValues);
        while (i.hasNext())
        {
            i.next();
            query.addBindValue(i.value());
        }
        query.exec();
        setQuery(query);
    }
}

void ESqlQueryModel::setFilter(const QString &filter)
{
    if(filter.isEmpty())
        p_filterStr = QString();
    else
        p_filterStr = " WHERE "+filter;
}

void ESqlQueryModel::setGroup(const QString &group)
{
    if(group.isEmpty())
        p_groupStr = QString();
    else
        p_groupStr = " GROUP BY "+group;
}
