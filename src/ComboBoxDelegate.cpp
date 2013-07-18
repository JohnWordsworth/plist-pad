#include "ComboBoxDelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QStringList &values, QObject *parent) : QItemDelegate(parent)
{
    _values = values;
}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(_values);

    QPalette pal = editor->palette();
    pal.setBrush(editor->backgroundRole(), Qt::transparent);
    editor->setPalette(pal);

    return editor;
}


void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    comboBox->setCurrentText(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString value = comboBox->currentText();
    model->setData(index, value);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(100, 22);
}
