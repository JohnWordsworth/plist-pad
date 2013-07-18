#include "PlistTreeItem.h"


//
// Object Lifecycle
//

// Constructor
PlistTreeItem::PlistTreeItem(const QVariant &value, const QString &key)
{
    _parentItem = nullptr;
    _key = key;

    setValueAndType(value);
}


PlistTreeItem::PlistTreeItem(const PlistType &type, const QString &key)
{
    _parentItem = nullptr;
    _key = key;
    _type = type;
    _value = QVariant();
}


PlistTreeItem::PlistTreeItem(const PlistTreeItem &item)
{
    _parentItem = nullptr;
    _key = item.key();
    _type = item.plistType();
    _value = item._value;

    for( int i = 0; i < item.childCount(); i++ )
    {
        PlistTreeItem *childItem = new PlistTreeItem(*item.child(i));
        aendChild(childItem);
    }
}


// Destructor
PlistTreeItem::~PlistTreeItem()
{
    removeAllChildren();
}


//
// Children
//


bool PlistTreeItem::canAddChild() const
{
    // The 'Invisible Root' only allows for one child item
    if ( _type == PlistInvisibleRoot && _childItems.count() == 0 )
    {
        return true;
    }

    // Otherwise, only arrays and dictionaries allow child items
    if ( _type == PlistArray || _type == PlistDictionary )
    {
        return true;
    }

    return false;
}


bool PlistTreeItem::aendChild(PlistTreeItem *child)
{
    if ( !canAddChild() ) {
        return false;
    }

    child->setParent(this);
    _childItems.append(child);

    if ( shouldChildrenHaveKey() ) {
        if ( !isChildKeyValid(child->key(), child) ) {
            child->setKey(nextChildKey());
        }
    } else {
        child->setKey(QString());
    }

    return true;
}


bool PlistTreeItem::insertChild(int index, PlistTreeItem *child)
{
    if ( !canAddChild() ) {
        return false;
    }

    child->setParent(this);
    _childItems.insert(index, child);

    if ( shouldChildrenHaveKey() ) {
        if ( !isChildKeyValid(child->key(), child) ) {
            child->setKey(nextChildKey());
        }
    } else {
        child->setKey(QString());
    }

    return true;
}


void PlistTreeItem::removeAllChildren()
{
    qDeleteAll(_childItems);
    _childItems.clear();
}


bool PlistTreeItem::removeChildAtIndex(int index)
{
    if ( index >= _childItems.count() ) {
        return false;
    }

    PlistTreeItem *item = _childItems.at(index);
    delete item;
    _childItems.removeAt(index);

    return true;
}


PlistTreeItem * PlistTreeItem::child(int row) const
{
    return _childItems.value(row);
}


int PlistTreeItem::childCount() const
{
    return _childItems.count();
}


QString PlistTreeItem::nextChildKey() const
{
    if ( _type != PlistDictionary ) {
        return QString();
    }

    int index = 1;
    QString key = QString("Key %1").arg(index++);

    while( !isChildKeyValid(key) && index < 1000 ) {
        key = QString("Key %1").arg(index++);
    }

    return key;
}


//
// Getters / Setters
//

void PlistTreeItem::setValueAndType(const QVariant &value)
{
    // Check that a valid value has been provided
    if ( !value.isValid() ) {
        return;
    }

    // Whatever we do, this will kill all children as we're setting the value
    removeAllChildren();

    // Switch dependant on the variant type
    if ( value.type() == QVariant::Type::String )
    {
        _type = PlistString;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::Int || value.type() == QVariant::Type::LongLong )
    {
        _type = PlistInteger;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::Double )
    {
        _type = PlistReal;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::Bool )
    {
        _type = PlistBoolean;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::Date )
    {
        _type = PlistDate;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::BitArray )
    {
        _type = PlistData;
        _value = value;
    }
    else if ( value.type() == QVariant::Type::List )
    {
        _type = PlistArray;
        _value = QVariant();

        QList<QVariant> list = value.toList();

        for ( QList<QVariant>::Iterator it = list.begin(); it != list.end(); ++it )
        {
           PlistTreeItem *child = new PlistTreeItem((*it), QString());
           aendChild(child);
        }
    }
    else if ( value.type() == QVariant::Type::Map )
    {
        _type = PlistDictionary;
        _value = QVariant();

        QMap<QString,QVariant> map = value.toMap();

        for ( QMap<QString,QVariant>::Iterator it = map.begin(); it != map.end(); ++it)
        {
            PlistTreeItem *child = new PlistTreeItem(it.value(), it.key());
            aendChild(child);
        }
    }
}


void PlistTreeItem::setValueRetainType(const QVariant &value)
{
    if ( _type == PlistArray || _type == PlistDictionary || _type == PlistInvisibleRoot ) {
        _value = QVariant();
        return;
    }

    _value = QVariant();

    if ( _type == PlistString )
    {
        _value = value.canConvert(QVariant::String) ? value.toString() : QString();
    }
    else if ( _type == PlistReal )
    {
        _value = value.canConvert(QVariant::Double) ? value.toDouble() : QVariant(0.0);
    }
    else if ( _type == PlistInteger )
    {
        _value = value.canConvert(QVariant::Int) ? value.toInt() : QVariant(0);
    }
    else if ( _type == PlistBoolean )
    {
        _value = value.canConvert(QVariant::Bool) ? value.toBool() : QVariant(true);
    }
    else if ( _type == PlistDate )
    {
        _value = value.canConvert(QVariant::Date) ? QVariant(value.toDateTime()) : QVariant(QDateTime());
    }
    else if ( _type == PlistData )
    {
        _value = value.canConvert(QVariant::BitArray) ? QVariant(value.toBitArray()) : QVariant(QBitArray());
    }
}


QVariant PlistTreeItem::getValue()
{
    QVariant result;

    switch( _type ) {
    case PlistString: result = _value.toString(); break;
    case PlistReal: result = _value.toDouble(); break;
    case PlistInteger: result = _value.toInt(); break;
    case PlistBoolean: result = _value.toBool(); break;
    case PlistDate: result = _value.toDate(); break;
    case PlistData: result = _value.toBitArray(); break;

    case PlistArray:
        {
            QList<QVariant> list;

            for( int i = 0; i < childCount(); ++i ) {
                list.append(child(i)->getValue());
            }

            result = list;
            break;
        }

    case PlistDictionary:
        {
            QMap<QString, QVariant> map;

            for( int i = 0; i < childCount(); ++i ) {
                map.insert(child(i)->key(), child(i)->getValue());
            }

            result = map;
            break;
        }
    }

    return result;
}


PlistTreeItem * PlistTreeItem::parent() const
{
    return _parentItem;
}


QString PlistTreeItem::key() const
{
    return _key;
}


void PlistTreeItem::setType(PlistType type)
{
    QVariant value = getValue();
    _type = type;
    setValueRetainType(value);
}


void PlistTreeItem::setParent(PlistTreeItem *parent)
{
    _parentItem = parent;
}


bool PlistTreeItem::shouldChildrenHaveKey() const
{
    if ( _type == PlistDictionary ) {
        return true;
    }

    return false;
}


bool PlistTreeItem::isChildKeyValid(QString &aString, PlistTreeItem *ignoreItem) const
{
    if ( aString.isNull() || aString.isEmpty() ) {
        return false;
    }

    for( QList<PlistTreeItem *>::const_iterator it = _childItems.begin(); it != _childItems.end(); ++it )
    {
        if ( (*it) != ignoreItem && (*it)->key().compare(aString) == 0 ) {
            return false;
        }
    }

    return true;
}


bool PlistTreeItem::setKey(QString &aString)
{
    if ( !_parentItem || !_parentItem->shouldChildrenHaveKey() ) {
        _key = QString();
        return aString.isEmpty();
    }

    if ( !_parentItem->isChildKeyValid(aString, this) ) {
        return false;
    }

    _key = aString;
    return true;
}


//
// Data Model
//


int PlistTreeItem::row() const
{
    if (_parentItem) {
        return _parentItem->_childItems.indexOf(const_cast<PlistTreeItem*>(this));
    }

    return 0;
}


int PlistTreeItem::columnCount() const
{
    return 3;
}


bool PlistTreeItem::isRoot() const
{
    return ( _parentItem && _parentItem->plistType() == PlistInvisibleRoot );
}


QVariant PlistTreeItem::data(int column) const
{
    switch(column)
    {
    case COLUMN_KEY: return keyDescription();
    case COLUMN_TYPE: return typeDescription();
    case COLUMN_VALUE:
        if ( IsContainerType(_type) ) {
            return QString("%1 Items").arg(childCount());
        } else {
            return _value;
        }
    }

    return QVariant();
}


PlistTreeItem::PlistType PlistTreeItem::plistType() const
{
    return _type;
}


QString PlistTreeItem::keyDescription() const
{
    if (!_parentItem || isRoot()) {
        return QString("Root");
    }

    if (_parentItem->plistType() == PlistArray)
    {
        return QString("%1").arg(row());
    }
    else if ( _parentItem->plistType() == PlistDictionary)
    {
        return _key;
    }

    return QString();
}


QString PlistTreeItem::typeDescription() const
{
    return PlistTreeItem::PlistTypeToString(_type);
}


Qt::ItemFlags PlistTreeItem::flags(int column) const
{
    if ( column == COLUMN_KEY )
    {
        if ( _parentItem && _parentItem->plistType() == PlistDictionary )
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
    }
    else if ( column == COLUMN_TYPE )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else if ( column == COLUMN_VALUE )
    {
        if ( _type == PlistBoolean ) {
            return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
        }

        if ( _type == PlistString || _type == PlistReal || _type == PlistInteger || _type == PlistDate )
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }

        if ( _type == PlistDictionary || _type == PlistArray )
        {
            return 0;
        }
    }

    // Default
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


bool PlistTreeItem::setData(int column, QVariant data)
{
    if ( !(flags(column) & Qt::ItemIsEditable) ) {
        return false;
    }

    if ( column == COLUMN_KEY )
    {
        return setKey(data.toString());
    }
    else if ( column == COLUMN_TYPE )
    {
        PlistType type = PlistTreeItem::StringToPlistType(data.toString());

        if ( type != PlistError ) {
            setType(type);
            return true;
        }
    }
    else if ( column == COLUMN_VALUE )
    {
        setValueRetainType(data);
        return true;
    }

    return false;
}


//
// Static Methods
//


QString PlistTreeItem::PlistTypeToString(PlistType plistType)
{
    switch(plistType)
    {
    case PlistString: return QString("String");
    case PlistReal: return QString("Real");
    case PlistInteger: return QString("Integer");
    case PlistBoolean: return QString("Boolean");
    case PlistDate: return QString("Date");
    case PlistData: return QString("Data");
    case PlistArray: return QString("Array");
    case PlistDictionary: return QString("Dictionary");
    case PlistInvisibleRoot: return QString("Internal Invisible Root Object");
    }

    return QString();
}


PlistTreeItem::PlistType PlistTreeItem::StringToPlistType(QString aValue)
{
    if ( aValue.compare("String", Qt::CaseInsensitive) == 0 ) { return PlistString; }
    if ( aValue.compare("Real", Qt::CaseInsensitive) == 0 ) { return PlistReal; }
    if ( aValue.compare("Integer", Qt::CaseInsensitive) == 0 ) { return PlistInteger; }
    if ( aValue.compare("Boolean", Qt::CaseInsensitive) == 0 ) { return PlistBoolean; }
    if ( aValue.compare("Date", Qt::CaseInsensitive) == 0 ) { return PlistDate; }
    if ( aValue.compare("Data", Qt::CaseInsensitive) == 0 ) { return PlistData; }
    if ( aValue.compare("Array", Qt::CaseInsensitive) == 0 ) { return PlistArray; }
    if ( aValue.compare("Dictionary", Qt::CaseInsensitive) == 0 ) { return PlistDictionary; }

    return PlistError;
}


bool PlistTreeItem::IsContainerType(PlistType plistType)
{
    return ( plistType == PlistDictionary || plistType == PlistArray );
}


QStringList PlistTreeItem::ComboBoxTypeStrings()
{
    QStringList list;

    for( int i = 0; i < PlistInvisibleRoot; ++i )
    {
        list << PlistTypeToString((PlistTreeItem::PlistType)i);
    }

    return list;
}
