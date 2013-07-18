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

#ifndef PLISTTREEITEM_H
#define PLISTTREEITEM_H

#include <QVariant>
#include <QDate>
#include <QBitArray>
#include <QStringList>


/**
 * @brief Represents a single row in the Plist item tree.
 *
 * Note that we store the items in the Plist structure as tree items with a vector
 * of children regardless of the type of object that this item represents (even if
 * it is a dictionary). This allows us to preserve the order of the elements in a
 * dictionary. Note that order is lost if you convert this object (and it's children)
 * to a QVariant and back again.
 */
class PlistTreeItem
{
public:
    enum PlistType {
        PlistString,
        PlistReal,
        PlistInteger,
        PlistBoolean,
        PlistDate,
        PlistData,
        PlistArray,
        PlistDictionary,
        PlistInvisibleRoot,
        PlistError,
    };

    static const int VALID_PLIST_TYPE_COUNT = 8;      // Not including fake types like Invisible Root
    static const int COLUMN_KEY = 0;
    static const int COLUMN_TYPE = 1;
    static const int COLUMN_VALUE = 2;

    friend class PlistTreeItem;

public:
    //
    // Object Lifecycle
    //

    PlistTreeItem(const QVariant &value, const QString &key = QString());
    PlistTreeItem(const PlistType &type, const QString &key = QString());
    PlistTreeItem(const PlistTreeItem &item);
    ~PlistTreeItem();

    //
    // Children
    //

    /** Does this item accept adding children? */
    bool canAddChild() const;

    /** Aend the given node as a child of this node. Fails and returns NO if not an array or dictionary. */
    bool aendChild(PlistTreeItem *child);

    /** Insert node as a child at a given index. */
    bool insertChild(int index, PlistTreeItem *child);

    /** Clear all children. */
    void removeAllChildren();

    /** Remove a child at the given index. */
    bool removeChildAtIndex(int index);

    /** Get the child at the given row. */
    PlistTreeItem *child(int row) const;

    /** How many children do we have? */
    int childCount() const;

    /** For a dictionary, generaet the 'next' unique child key. */
    QString nextChildKey() const;

    //
    // Getters and Setters
    //

    /** Set the value of this item based on the type of the given variant. */
    void setValueAndType(const QVariant &value);

    /** Set the value of this item and convert the provided value to the current item's type. */
    void setValueRetainType(const QVariant &value);

    /** Get the current value of this item as a QVariant, wraing children up in lists/maps as required. */
    QVariant getValue();

    /** Change this item to the current type, modifying or destroying the underlying data as required. */
    void setType(PlistType type);

    /** Set Parent, for use in aendChild. */
    void setParent(PlistTreeItem *parent);

    /** Should this node have a key? */
    bool shouldChildrenHaveKey() const;

    /** Is the provided key valid for this item? */
    bool isChildKeyValid(QString &aString, PlistTreeItem *ignoreItem = nullptr) const;

    /** Set Key. */
    bool setKey(QString &aString);

    /** Return the parent item. */
    PlistTreeItem *parent() const;

    /** Get the raw key value. */
    QString key() const;

    //
    // Data Model
    //

    /** What is the row position of this item frmo the parent's view? */
    int row() const;

    /** How many 'columns' does this item have? For Plist data, this is fixed to 3. */
    int columnCount() const;

    /** Is this the real root item of the plist? */
    bool isRoot() const;

    /** What is the data in the given 'column' in model terms? We have 3 columns: Key, Type, Value. */
    QVariant data(int column) const;

    /** Get the Plist Type for this item. */
    PlistType plistType() const;

    /** Get the 'Key' display value, which might not be the actual key if this item exists in an array say. */
    QString keyDescription() const;

    /** Get the 'Type' as a string, which can be displayed in the tree view. */
    QString typeDescription() const;

    /** Flags for the given cell (is it editable or readonly?). */
    Qt::ItemFlags flags(int column) const;

    /** Set the data for a given column. */
    bool setData(int column, QVariant data);



    //
    // Private Variables
    //

private:
    PlistTreeItem *_parentItem;            // Link to parent, or nullptr
    QList<PlistTreeItem*> _childItems;     // List of child nodes

    QString _key;                       // Key, if in dictionary
    PlistType _type;                    // Plist Type of value
    QVariant _value;                    // Value, if not a container


    //
    // Static methods
    //

public:
    /** Convert a Plist type value into a string. */
    static QString PlistTypeToString(PlistType plistType);

    /** Convert a String value into a Plist type enum value. */
    static PlistType StringToPlistType(QString aValue);

    /** Is the given plist type a 'container' type? */
    static bool IsContainerType(PlistType plistType);

    /** Get a full list of strings for Plist Types to display in the type dropdown. */
    static QStringList ComboBoxTypeStrings();
};

#endif // PLISTTREEITEM_H
