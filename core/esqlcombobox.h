#ifndef ESQLCOMBOBOX_H
#define ESQLCOMBOBOX_H

#include <QComboBox>
#include "ecolorsqlquerymodel.h"
//#include "../interfaces/iqvariantconvertable.h"

class ESqlComboBox : public QComboBox
        //, public IQVariantConvertable
{
    Q_OBJECT
public:
    ESqlComboBox(QWidget *parent = Q_NULLPTR);
    ESqlComboBox(const QString &query, QWidget *parent = Q_NULLPTR);
    virtual ~ESqlComboBox();
    void setQuery(const QString &query);
    int id(const int row = -1) const;
    bool setId(int value, int column = 0);
    void updateData();
    EColorSqlQueryModel *model() const { return p_model; }
    QString title() const;
    QVariant modelData(const int row, const int column, const int role = Qt::DisplayRole) const { return p_model->index(row,column).data(role); }
    QVariant currentData(const int column, const int role = Qt::DisplayRole) const { return p_model->index(currentIndex(),column).data(role); }
    bool contains(QVariant val, int column = 0){ return p_model->match(p_model->index(0,column),Qt::EditRole,val,1,Qt::MatchExactly).size()>0; }
    int firstMatch(QVariant val, int column = 0);

    void setColorField(const int fieldNum) { p_model->setColorField(1, true, fieldNum); }
    void setBlackened(const int row, const bool black);
    void setIdNullType(const int idNullType) {this->idNullType = idNullType;}

    QVariant getValueAsQVariant();
    void setValueFromQvariant(const QVariant &value);
signals:
    void currentIdChanged(int id);
private:
    int previousId;
    int idNullType;

    EColorSqlQueryModel *p_model;
private slots:
    void slot_currentIndexChanged(int index);
};

#endif // ESQLCOMBOBOX_H
