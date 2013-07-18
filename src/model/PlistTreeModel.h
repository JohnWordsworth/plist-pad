/****************************************************************************
** Copyright (c) 2013 "John Wordsworth"
** Contact: http://www.johnwordsworth.com/
**
** This file is part of Plist Pad.
**
** Plist Pad is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef PLISTTREEMODEL_H
#define PLISTTREEMODEL_H

#include <QAbstractItemModel>
#include <QUndoStack>
#include <QtGui>
#include "PlistTreeItem.h"


enum ReplaceMode {
    ReplaceModeNormal
};

enum ReplaceTarget {
    ReplaceKey,
    ReplaceValue,
    ReplaceAll
};


/**
 * @brief Qt Item Model for Plist Tree
 *
 * The model should handle all interactions between the underlying tree and the user.
 */
class PlistTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //
    // Object Lifecycle
    //

    /** Constructor which accepts some data as a QVariant an converts that to the internal format. */
    PlistTreeModel(const QVariant &data, QObject *parent = 0);

    /** Constructor which accepts a root node (without the invisible parent). */
    PlistTreeModel(PlistTreeItem *root, QObject *parent = 0);

    /** Constructor without any data, will create an 'empty' model with a single Dictionary root node. */
    PlistTreeModel(QObject *parent = 0);

    /** Destructor. */
    ~PlistTreeModel();


    //
    // Helper Methods
    //

    /** Get the PlistTreeItem at a given index. */
    PlistTreeItem *itemAtIndex(const QModelIndex &index) const;


    //
    // Model Methods
    //

    /** Get data from the model at the given index, with the given role. */
    QVariant data(const QModelIndex &index, int role) const;

    /** Item flags for the item at the given index. */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /** Header data at a given section. */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /** Index method. */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /** Parent for the given index. */
    QModelIndex parent(const QModelIndex &index) const;

    /** Row count for the item at the given index. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /** Column count for the item at the given index. */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /** For editable cells. */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /** Ability to insert rows. */
    bool insertRows(int row, int count, const QModelIndex &parent);

    /** Ability to remove rows. */
    bool removeRows(int row, int count, const QModelIndex &parent);


    //
    // Public Methods for Alication Use
    //

    /** Get the visible root node. */
    PlistTreeItem *visibleRoot();

    /** Insert a full PlistTreeItem into the tree and reset the data model afterwards. */
    bool insertItem(PlistTreeItem *item, int row, const QModelIndex &parent);

    /** Do a find/replace across the whole tree. */
    int findReplace(QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode);

    
signals:
    
public slots:

protected:
    int doFindReplace(QModelIndex &idx, QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode);


private:
    PlistTreeItem *_invisibleRootItem;
    
};

#endif // PLISTTREEMODEL_H
