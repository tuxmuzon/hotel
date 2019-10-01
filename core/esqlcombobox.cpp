#include "esqlcombobox.h"
#include <QtCore>
#include <QtGui>
//#include "../utility/qvariantstaticconverter.h"

ESqlComboBox::ESqlComboBox(QWidget *parent) :
    QComboBox(parent)
{
    p_model = new EColorSqlQueryModel;
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_currentIndexChanged(int)));
    previousId = -1;
    idNullType = 1;
}

ESqlComboBox::ESqlComboBox(const QString &query, QWidget *parent) :
    QComboBox(parent)
{
    p_model = new EColorSqlQueryModel;
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_currentIndexChanged(int)));
    previousId = -1;
    setQuery(query);
    idNullType = 1;
}

ESqlComboBox::~ESqlComboBox()
{
    delete p_model;
}

void ESqlComboBox::setQuery(const QString &query)
{
    p_model->setQuery(query);
    this->setModel(p_model);
    this->setModelColumn(1);
}

int ESqlComboBox::id(const int row) const
{
    int current_row = row==-1?this->currentIndex():row;
    return p_model->index(current_row,0).data().toInt();
}

bool ESqlComboBox::setId(int value, int column)
{
    this->setCurrentIndex(p_model->match(p_model->index(0,column),Qt::EditRole,value,1,Qt::MatchExactly).value(0).row());
    return true;
}

QVariant ESqlComboBox::getValueAsQVariant()
{
//    return QVariantStaticConverter::IntToQVariant(id(),idNullType);
    return  QVariant();
}

void ESqlComboBox::setValueFromQvariant(const QVariant &value)
{
  //  setId(QVariantStaticConverter::QVariantToInt(value, idNullType));
}

void ESqlComboBox::updateData()
{
    int currentId = id();    
    p_model->setQuery(p_model->query().lastQuery());
    this->setModel(p_model);
    this->setModelColumn(1);
    setId(currentId);
}

QString ESqlComboBox::title() const
{
    return p_model->index(this->currentIndex(),1).data().toString();
}

int ESqlComboBox::firstMatch(QVariant val, int column)
{
    if(contains(val,column))
        return p_model->match(p_model->index(0,column),Qt::EditRole,val,1,Qt::MatchExactly).value(0).row();
    return -1;
}

void ESqlComboBox::setBlackened(const int row, const bool black)
{
    if(black)
        p_model->setFlags(p_model->index(row,1),Q_NULLPTR);
    else
        p_model->resetFlags(p_model->index(row,1));
}

void ESqlComboBox::slot_currentIndexChanged(int index)
{
    int currentId = p_model->index(index,0).data().toInt();
    if(currentId!=previousId)
        emit currentIdChanged(currentId);
    previousId = currentId;
}
