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

#ifndef PLISTTREEWRITER_H
#define PLISTTREEWRITER_H

#include "PlistTreeItem.h"

#include <QXmlStreamWriter>
#include <QFile>


/**
 * @brief Class for writing a Plist Tree to a File / IODevice or String.
 */
class PlistTreeWriter
{
public:
    PlistTreeWriter();

    bool writeTreeToFile(PlistTreeItem *rootNode, QString &fileName);
    bool writeTreeToIODevice(PlistTreeItem *rootNode, QIODevice *device);
    bool writeTreeToString(PlistTreeItem *rootNode, QString *string);


protected:
    bool writeToXmlStreamWriter(PlistTreeItem *rootNode, QXmlStreamWriter &xmlWriter);
    bool writeNode(PlistTreeItem *node, QXmlStreamWriter &xmlWriter, bool isRootNode);

    QString elementNameForItem(PlistTreeItem *node);
};

#endif // PLISTTREEWRITER_H
