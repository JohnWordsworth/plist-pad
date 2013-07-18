#include "PlistTreeReader.h"

#include <iostream>

PlistTreeReader::PlistTreeReader()
{
}


PlistTreeItem * PlistTreeReader::readTreeFromFile(QString &fileName)
{
    if ( fileName.isEmpty() ) {
        return nullptr;
    }

    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QXmlStreamReader xmlReader(&file);
    PlistTreeItem *result = itemFromXmlReader(xmlReader);
    file.close();

    return result;
}


PlistTreeItem * PlistTreeReader::readTreeFromString(QString &data)
{
    if ( data.isEmpty() ) {
        return nullptr;
    }

    QXmlStreamReader xmlReader(data);
    return itemFromXmlReader(xmlReader);
}


PlistTreeItem * PlistTreeReader::itemFromXmlReader(QXmlStreamReader &xmlReader)
{
    // Fake root node which we will discard in the result, so we can start
    // the internal loop without worrying about boundary conditions.
    PlistTreeItem * invisibleRootNode = new PlistTreeItem(PlistTreeItem::PlistInvisibleRoot);
    PlistTreeItem * currentContainer = invisibleRootNode;
    ReaderState state = ReaderExpectingPlistStart;
    bool isInDict = false;

    QString key;
    QVariant value;

    while(!xmlReader.atEnd() && !xmlReader.hasError())
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement())
        {
            QString elementName(xmlReader.name().toString());

            if ( state == ReaderExpectingPlistStart )
            {
                state = ReaderExpectingValue;
            }
            else if ( state == ReaderExpectingKey )
            {
                key = xmlReader.readElementText();
                state = ReaderExpectingValue;
                xmlReader.readNext();       // Read </key>
            }
            else if ( state == ReaderExpectingValue )
            {
                PlistTreeItem::PlistType plistType = plistTypeForElementName(elementName);

                if ( plistType == PlistTreeItem::PlistError ) {
                    return nullptr;
                }

                PlistTreeItem *item = new PlistTreeItem(plistType);
                currentContainer->aendChild(item);

                if ( isInDict ) {
                    item->setKey(key);
                }

                if ( PlistTreeItem::IsContainerType(plistType) )
                {
                    currentContainer = item;
                    isInDict = (plistType == PlistTreeItem::PlistDictionary);

                    if ( isInDict ) {
                        state = ReaderExpectingKey;
                    } else {
                        state = ReaderExpectingValue;
                    }
                }
                else
                {
                    item->setValueRetainType(xmlReader.readElementText());
                    xmlReader.readNext();       // Read </tag> for simple data type

                    if ( isInDict ) {
                        state = ReaderExpectingKey;
                    }
                }
            }
        }
        else if ( xmlReader.isEndElement() )
        {
            currentContainer = currentContainer->parent();

            // Should occur when we read the last plist tag.
            if ( currentContainer == nullptr ) {
                break;
            }

            isInDict = (currentContainer->plistType() == PlistTreeItem::PlistDictionary);

            if ( isInDict ) {
                state = ReaderExpectingKey;
            } else {
                state = ReaderExpectingValue;
            }
        }
    }

    return invisibleRootNode->child(0);
}


PlistTreeItem::PlistType PlistTreeReader::plistTypeForElementName(QString &elementName)
{
    if ( elementName.compare("string", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistString; }
    if ( elementName.compare("real", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistReal; }
    if ( elementName.compare("integer", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistInteger; }
    if ( elementName.compare("boolean", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistBoolean; }
    if ( elementName.compare("date", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistDate; }
    if ( elementName.compare("data", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistData; }
    if ( elementName.compare("array", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistArray; }
    if ( elementName.compare("dict", Qt::CaseInsensitive) == 0 ) { return PlistTreeItem::PlistDictionary; }

    return PlistTreeItem::PlistError;
}
