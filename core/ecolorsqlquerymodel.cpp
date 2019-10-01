#include "ecolorsqlquerymodel.h"
#include <QColor>

EColorSqlQueryModel::EColorSqlQueryModel(QObject *parent) :
    ESqlQueryModel(parent)
{
}

QVariant EColorSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(colorFieldHash.contains(index.column()))
    {
        int fromFiled = colorFieldHash.value(index.column());

        if(role==Qt::DecorationRole)
        {
            return QColor(ESqlQueryModel::data(fromFiled==-1?index:ESqlQueryModel::index(index.row(),fromFiled),Qt::DisplayRole).toString());
        }
        else if(fromFiled==-1)
        {
            return QVariant();
        }
    }

    return ESqlQueryModel::data(index,role);
}

void EColorSqlQueryModel::setColorField(const int toField, const bool enabled, const int fromField)
{
    if(enabled)
        colorFieldHash.insert(toField,fromField);
    else
        colorFieldHash.remove(toField);
}
