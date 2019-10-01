#ifndef ECOLORSQLQUERYMODEL_H
#define ECOLORSQLQUERYMODEL_H
#include "esqlquerymodel.h"
#include <QSet>

class EColorSqlQueryModel : public ESqlQueryModel
{
    Q_OBJECT
public:
    explicit EColorSqlQueryModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setColorField(int toField, bool enabled = true, const int fromField = -1);
private:
    QHash<int,int> colorFieldHash;
};

#endif // ECOLORSQLQUERYMODEL_H
