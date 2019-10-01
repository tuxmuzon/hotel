#ifndef ESQLQUERY_H
#define ESQLQUERY_H

#include <QSqlQuery>
#include <QVariantHash>
#include <QSqlRecord>

class ESqlQuery : public QSqlQuery
{
public:
    ESqlQuery(const QString& query = QString(), QSqlDatabase db = QSqlDatabase::database()) : QSqlQuery(query,db) { this->db = db; checkConnection=true; }
    explicit ESqlQuery(QSqlDatabase db) : QSqlQuery(db) { this->db = db; checkConnection=true; }

    bool exec(const QString &query);
    bool exec();
    bool execBatch();

    void bindValues(const QVariantHash &values, QSql::ParamType type = QSql::In);

    QString executedQueryWithValues();
    QList<int> getResultsAsIntegerList(const int &column = 0);
    QStringList getResultsAsStringList(const int &column = 0);
    void setCheckConnection(const bool &on){ checkConnection = on; }
private:
    QSqlDatabase db;
    bool checkConnection;
};

#endif // ESQLQUERY_H
