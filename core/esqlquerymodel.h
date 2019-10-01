#ifndef ESQLQUERYMODEL_H
#define ESQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlQuery>

// SQL query model with checkbox in bool fields and error logging

class ESqlQueryModel : public QSqlQueryModel
{
Q_OBJECT
public:
    explicit ESqlQueryModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setQuery(const QSqlQuery &query);
    void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
    void setSort(int column, Qt::SortOrder order);
    void setFixedSort(int column, Qt::SortOrder order) { p_fixedSortColumn = column; p_fixedSortOrder = order; }
    void setFixedPriorSort(int column, Qt::SortOrder order) { p_fixedPriorSortColumn = column; p_fixedPriorSortOrder = order; }
    virtual void sort(int column, Qt::SortOrder order);
    virtual void setFilter (const QString &filter);
    void setGroup(const QString &group);
    void addOrderIdx(int column, int orderidxColumn){ p_orderidxHash.insert(column,orderidxColumn); }
    void removeOrderIdx(int column){ p_orderidxHash.remove(column); }
    void setFlags(const QModelIndex &index, Qt::ItemFlags flags){ p_flagsHash[index]=flags; }
    void resetFlags(const QModelIndex &index){ p_flagsHash.remove(index); }
    QString sortString(){ return p_sortStr; }
private:
    QString p_queryStr;
    QString p_sortStr;
    QString p_filterStr;
    QString p_groupStr;
    int p_fixedSortColumn;
    int p_fixedPriorSortColumn;
    Qt::SortOrder p_fixedSortOrder;
    Qt::SortOrder p_fixedPriorSortOrder;
    QMap<QString, QVariant> boundValues;
    QHash<int, int> p_orderidxHash;
    QHash<QModelIndex, Qt::ItemFlags> p_flagsHash;
};

#endif // ESQLQUERYMODEL_H
