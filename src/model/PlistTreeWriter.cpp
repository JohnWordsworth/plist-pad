#include "PlistTreeWriter.h"


PlistTreeWriter::PlistTreeWriter()
{

}


bool PlistTreeWriter::writeTreeToFile(PlistTreeItem *rootNode, QString &fileName)
{
    if ( rootNode == nullptr || fileName.isEmpty() ) {
        return false;
    }

    QFile *file = new QFile(fileName);
    bool result = writeTreeToIODevice(rootNode, file);
    file->close();
    delete file;

    return result;
}


bool PlistTreeWriter::writeTreeToIODevice(PlistTreeItem *rootNode, QIODevice *device)
{
    if ( rootNode == nullptr || device == nullptr ) {
        return false;
    }

    device->open(QIODevice::WriteOnly | QIODevice::Text);

    QXmlStreamWriter xmlWriter(device);
    bool result = writeToXmlStreamWriter(rootNode, xmlWriter);

    device->close();
    return result;
}


bool PlistTreeWriter::writeTreeToString(PlistTreeItem *rootNode, QString *string)
{
    if ( rootNode == nullptr || string == nullptr ) {
        return false;
    }

    QXmlStreamWriter xmlWriter(string);
    bool result = writeToXmlStreamWriter(rootNode, xmlWriter);

    return result;
}


//
// Protected Methods
//


bool PlistTreeWriter::writeToXmlStreamWriter(PlistTreeItem *rootNode, QXmlStreamWriter &xmlWriter)
{
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument("1.0");

    xmlWriter.writeDTD("<!DOCTYPE plist PUBLIC \"-//Ale//DTD PLIST 1.0//EN\" \"http://www.ale.com/DTDs/PropertyList-1.0.dtd\">");
    xmlWriter.writeStartElement("plist");
    xmlWriter.writeAttribute("version", "1.0");

    writeNode(rootNode, xmlWriter, true);

    xmlWriter.writeEndElement();    // Plist
    xmlWriter.writeEndDocument();

    return true;
}


bool PlistTreeWriter::writeNode(PlistTreeItem *node, QXmlStreamWriter &xmlWriter, bool isRootNode = false)
{
    if ( node == nullptr || xmlWriter.hasError() ) {
        return false;
    }

    // If this node has a valid parent, and the parent is a dictionary, we need to put out a <key>NAME</key> element.
    if ( !isRootNode && node->parent() != nullptr && node->parent()->plistType() == PlistTreeItem::PlistDictionary ) {
        xmlWriter.writeTextElement("key", node->key());
    }

    // Now output the content of this node
    QString elementName = elementNameForItem(node);

    if ( PlistTreeItem::IsContainerType(node->plistType()) )
    {
        xmlWriter.writeStartElement(elementName);

        for( int i = 0; i < node->childCount(); i++ ) {
            writeNode(node->child(i), xmlWriter);
        }

        xmlWriter.writeEndElement();
    }
    else
    {
        xmlWriter.writeTextElement(elementName, node->getValue().toString());
    }

    return true;
}


QString PlistTreeWriter::elementNameForItem(PlistTreeItem *node)
{
    switch(node->plistType())
    {
    case PlistTreeItem::PlistString: return QString("string");
    case PlistTreeItem::PlistReal: return QString("real");
    case PlistTreeItem::PlistInteger: return QString("integer");
    case PlistTreeItem::PlistBoolean: return QString("boolean");
    case PlistTreeItem::PlistDate: return QString("date");
    case PlistTreeItem::PlistData: return QString("data");
    case PlistTreeItem::PlistArray: return QString("array");
    case PlistTreeItem::PlistDictionary: return QString("dict");
    }

    return QString();
}

