#include "PlistTreeModel.h"


PlistTreeModel::PlistTreeModel(const QVariant &data, QObject *parent) : QAbstractItemModel(parent)
{
    _invisibleRootItem = new PlistTreeItem(PlistTreeItem::PlistInvisibleRoot);

    if ( data.isValid() && !data.isNull() )
    {
        _invisibleRootItem->aendChild(new PlistTreeItem(data));
    }
}


PlistTreeModel::PlistTreeModel(PlistTreeItem *root, QObject *parent) : QAbstractItemModel(parent)
{
    _invisibleRootItem = new PlistTreeItem(PlistTreeItem::PlistInvisibleRoot);

    if ( root != nullptr ) {
        _invisibleRootItem->aendChild(root);
    }
}


PlistTreeModel::PlistTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
    _invisibleRootItem = new PlistTreeItem(PlistTreeItem::PlistInvisibleRoot);
    _invisibleRootItem->aendChild(new PlistTreeItem(PlistTreeItem::PlistDictionary));
}


PlistTreeModel::~PlistTreeModel()
{
    delete _invisibleRootItem;
    _invisibleRootItem = nullptr;
}


PlistTreeItem * PlistTreeModel::itemAtIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return nullptr;
    }

    PlistTreeItem *item = static_cast<PlistTreeItem*>(index.internalPointer());
    return item;
}


// http://qt-project.org/doc/qt-4.8/itemviews-simpletreemodel.html
QModelIndex PlistTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    PlistTreeItem *parentItem;

    if (!parent.isValid()) {
        parentItem = _invisibleRootItem;
    } else {
        parentItem = static_cast<PlistTreeItem*>(parent.internalPointer());
    }

    PlistTreeItem *childItem = parentItem->child(row);

    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}


QModelIndex PlistTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    PlistTreeItem *childItem = static_cast<PlistTreeItem*>(index.internalPointer());
    PlistTreeItem *parentItem = childItem->parent();

    if (parentItem == _invisibleRootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}


int PlistTreeModel::rowCount(const QModelIndex &parent) const
{
    PlistTreeItem *parentItem;

    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = _invisibleRootItem;
    } else {
        parentItem = static_cast<PlistTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}


int PlistTreeModel::columnCount(const QModelIndex &parent) const
{
    // Always 3, but reluctant to hard code into model
    if (parent.isValid()) {
        return static_cast<PlistTreeItem*>(parent.internalPointer())->columnCount();
    } else {
        return _invisibleRootItem->columnCount();
    }
}


QVariant PlistTreeModel::data(const QModelIndex &index, int role) const
{
    PlistTreeItem *item = itemAtIndex(index);

    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::CheckStateRole) {
        return QVariant();
    }

    if ( item == nullptr ) {
        return QVariant();
    }

    if ( role == Qt::CheckStateRole )
    {
        if ( index.column() == PlistTreeItem::COLUMN_VALUE && item->plistType() == PlistTreeItem::PlistBoolean ) {
            return (item->data(index.column()).toBool()) ? Qt::Checked : Qt::Unchecked;
        } else {
            return QVariant();
        }
    }

    return item->data(index.column());
}


Qt::ItemFlags PlistTreeModel::flags(const QModelIndex &index) const
{
    PlistTreeItem *item = itemAtIndex(index);

    if ( item ) {
        return item->flags(index.column());
    }

    return 0;
}


QVariant PlistTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section)
        {
        case PlistTreeItem::COLUMN_KEY: return QString("Key");
        case PlistTreeItem::COLUMN_TYPE: return QString("Type");
        case PlistTreeItem::COLUMN_VALUE: return QString("Value");
        }
    }

    return QVariant();
}


bool PlistTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    PlistTreeItem *item = static_cast<PlistTreeItem*>(index.internalPointer());
    bool didChange = item->setData(index.column(), value);

    if ( didChange ) {
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 2));
    }

    return didChange;
}


bool PlistTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    PlistTreeItem *item = itemAtIndex(parent);

    if ( item == nullptr ) {
        return false;
    }

    emit beginInsertRows(parent, row, row + count);
    int insertIndex = row;

    for( int i = 0; i < count; ++i ) {
        bool didInsert = item->insertChild(insertIndex, new PlistTreeItem(PlistTreeItem::PlistString));

        if ( didInsert ) {
            insertIndex++;
        }
    }

    emit endInsertRows();
    emit dataChanged(parent.sibling(parent.row(), 0), parent.sibling(parent.row(), 2));
    return true;
}


bool PlistTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    PlistTreeItem *item = itemAtIndex(parent);

    if ( item == nullptr ) {
        return false;
    }

    emit beginRemoveRows(parent, row, row + count);
    int removeIndex = row;

    for( int i = 0; i < count; ++i ) {
        item->removeChildAtIndex(removeIndex);
    }

    emit endRemoveRows();
    return true;
}


PlistTreeItem * PlistTreeModel::visibleRoot()
{
    if ( _invisibleRootItem == nullptr || _invisibleRootItem->child(0) == nullptr ) {
        return nullptr;
    }

    return _invisibleRootItem->child(0);
}


bool PlistTreeModel::insertItem(PlistTreeItem *item, int row, const QModelIndex &parent)
{
    PlistTreeItem *parentItem = itemAtIndex(parent);

    if ( item == nullptr || parentItem == nullptr ) {
        return false;
    }

    emit beginInsertRows(parent, row, row + 1);
    parentItem->insertChild(row, item);
    emit endInsertRows();

    emit dataChanged(parent.sibling(parent.row(), 0), parent.sibling(parent.row(), 2));
    return true;
}


int PlistTreeModel::findReplace(QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode)
{
    if ( find.isEmpty() ) {
        return 0;
    }

    QModelIndex startIndex = index(0,0);
    int count = doFindReplace(startIndex, find, replace, target, mode);
    return count;
}


int PlistTreeModel::doFindReplace(QModelIndex &idx, QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode)
{
    int count = 0;
    PlistTreeItem *item = itemAtIndex(idx);
    PlistTreeItem *parent = item->parent();

    if ( item == nullptr ) {
        return 0;
    }

    if ( target == ReplaceKey || target == ReplaceAll )
    {
        if ( parent != nullptr && parent->shouldChildrenHaveKey() )
        {
            QModelIndex editIndex = idx.sibling(idx.row(), PlistTreeItem::COLUMN_KEY);
            QString origKey = data(editIndex, Qt::EditRole).toString();
            QString key = origKey;
            key.replace(find, replace);

            if ( origKey.compare(key) != 0 ) {
                setData(editIndex, key, Qt::EditRole);
                count++;
            }
        }
    }

    if ( target == ReplaceValue || target == ReplaceAll )
    {
        if ( item->plistType() == PlistTreeItem::PlistString )
        {
            QModelIndex editIndex = idx.sibling(idx.row(), PlistTreeItem::COLUMN_VALUE);
            QString origValue = data(editIndex, Qt::EditRole).toString();
            QString value = origValue;
            value.replace(find, replace);

            if ( origValue.compare(value) != 0 ) {
                setData(editIndex, value, Qt::EditRole);
                count++;
            }
        }
    }

    for ( int i = 0; i < item->childCount(); ++i )
    {
        QModelIndex childIndex = idx.child(i, 0);
        count += doFindReplace(childIndex, find, replace, target, mode);
    }

    return count;
}

