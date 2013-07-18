#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>


const QString kATitle = QString("PlistPad");
const QString kAVersion = QString("0.1.0");


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showTreeViewContextMenu(const QPoint&)));

    QString title = QString("%1 (v%2)").arg(kATitle, kAVersion);
    setWindowTitle(title);

    _findReplaceDialog = nullptr;
    _treeModel = nullptr;
    newFile();
}


MainWindow::~MainWindow()
{
    if ( _treeModel != nullptr ) {
        delete _treeModel;
    }

    delete ui;
}


//
// Slots
//


void MainWindow::showTreeViewContextMenu(const QPoint& pos)
{
    QPoint globalPos = ui->treeView->mapToGlobal(pos);

    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu menu;
    menu.addAction(QIcon(), "Add Child", this, SLOT(treeViewAddChildToSelection()));
    menu.addAction(QIcon(), "Add Sibling (Before)", this, SLOT(treeViewAddSiblingBeforeSelection()));
    menu.addAction(QIcon(), "Add Sibling (After)", this, SLOT(treeViewAddSiblingAfterSelection()));
    menu.addAction(QIcon(), "Delete Item", this, SLOT(treeViewRemoveSelectedRow()));
    menu.exec(globalPos);
}


void MainWindow::treeViewAddChildToSelection()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() > 0 ) {
        QModelIndex index = sel.at(0);
        index = index.sibling(index.row(), 0);

        int children = ui->treeView->model()->rowCount(index);
        ui->treeView->model()->insertRow(children, index);
    }
}


void MainWindow::treeViewAddSiblingBeforeSelection()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() > 0 ) {
        QModelIndex index = sel.at(0);
        index = index.sibling(index.row(), 0);

        ui->treeView->model()->insertRow(index.row(), index.parent());
    }
}


void MainWindow::treeViewAddSiblingAfterSelection()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() > 0 ) {
        QModelIndex index = sel.at(0);
        index = index.sibling(index.row(), 0);

        ui->treeView->model()->insertRow(index.row()+1, index.parent());
    }
}



void MainWindow::treeViewRemoveSelectedRow()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() > 0 ) {
        QModelIndex index = sel.at(0);
        index = index.sibling(index.row(), 0);

        ui->treeView->model()->removeRow(index.row(), index.parent());
    }
}


void MainWindow::newFile()
{
    _openFileName = QString();
    setModel(new PlistTreeModel());
}


void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Plist File"), QString(), "Plist Files (*.plist)");

    if ( !filename.isEmpty() )
    {
        PlistTreeReader itemReader = PlistTreeReader();
        PlistTreeItem *item = itemReader.readTreeFromFile(filename);
        setModel(new PlistTreeModel(item));
        on_action_CollapseAll_triggered();
        _openFileName = filename;
    }
}


void MainWindow::saveFile()
{
    if ( _openFileName.isEmpty() ) {
        saveFileAs();
        return;
    }

    PlistTreeWriter itemWriter = PlistTreeWriter();
    itemWriter.writeTreeToFile(_treeModel->visibleRoot(), _openFileName);
}


void MainWindow::saveFileAs()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Plist File"), QString(), "Plist Files (*.plist)");

    if ( !filename.isEmpty() )
    {
        PlistTreeWriter itemWriter = PlistTreeWriter();
        itemWriter.writeTreeToFile(_treeModel->visibleRoot(), filename);
        _openFileName = filename;
    }
}


void MainWindow::treeViewRowCopy()
{
    QModelIndex selectedIndex = getSelectedIndex();

    if ( !selectedIndex.isValid() ) {
        return;
    }

    PlistTreeItem *item = _treeModel->itemAtIndex(selectedIndex);

    if ( item != nullptr ) {
        QString xml;

        PlistTreeWriter itemWriter = PlistTreeWriter();
        itemWriter.writeTreeToString(item, &xml);

        QApplication::clipboard()->setText(xml);
    }
}


void MainWindow::treeViewRowCut()
{
    treeViewRowCopy();
    treeViewRemoveSelectedRow();
}


void MainWindow::treeViewRowPaste()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() == 0 ) {
        return;
    }

    QModelIndex index = sel.at(0);
    PlistTreeItem *selectedItem = _treeModel->itemAtIndex(index);

    if ( selectedItem == nullptr ) {
        return;
    }

    QString xml = QApplication::clipboard()->text();

    PlistTreeReader itemReader = PlistTreeReader();
    PlistTreeItem *insertItem = itemReader.readTreeFromString(xml);

    if ( insertItem == nullptr ) {
        return;
    }

    QModelIndex containerIndex;
    int insertRow = 0;
    bool result = false;

    if ( PlistTreeItem::IsContainerType(selectedItem->plistType()) && ui->treeView->isExpanded(index) ) {
        containerIndex = index;
    } else {
        containerIndex = index.parent();
        insertRow = index.row() + 1;
    }

    result = _treeModel->insertItem(insertItem, insertRow, containerIndex);

    if ( !result ) {
        delete insertItem;
    } else {
        ui->treeView->selectionModel()->clearSelection();
        ui->treeView->selectionModel()->select(containerIndex.child(insertRow,0), QItemSelectionModel::Select);
    }
}


void MainWindow::treeViewFindReplace(QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode)
{
    _treeModel->findReplace(find, replace, target, mode);
}


void MainWindow::on_actionSave_As_triggered()
{
    saveFileAs();
}

void MainWindow::on_action_Save_triggered()
{
    saveFile();
}

void MainWindow::on_action_New_triggered()
{
    newFile();
}

void MainWindow::on_action_Open_triggered()
{
    openFile();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "Key: " << event->key() << ", Want: " << Qt::Key_Enter << ", Text: " << event->text();

    if ( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return )
    {
        QModelIndex selectedIndex = getSelectedIndex();
        PlistTreeItem * selectedItem = _treeModel->itemAtIndex(selectedIndex);

        if ( selectedItem != nullptr )
        {
            if ( PlistTreeItem::IsContainerType(selectedItem->plistType()) )
            {
                if ( ui->treeView->isExpanded(selectedIndex)) {
                    treeViewAddChildToSelection();
                } else {
                    treeViewAddSiblingAfterSelection();
                }
            }
            else
            {
                treeViewAddSiblingAfterSelection();
            }
        }
    }

    if ( event->key() == Qt::Key_Delete )
    {
        QModelIndex selectedIndex = getSelectedIndex();

        if ( selectedIndex.column() == PlistTreeItem::COLUMN_KEY ) {
            treeViewRemoveSelectedRow();
        }
        else if ( selectedIndex.column() == PlistTreeItem::COLUMN_VALUE ) {
            _treeModel->setData(selectedIndex, QVariant(), Qt::EditRole);
        }
    }

    if ( event->matches(QKeySequence::Copy) )
    {
        QModelIndex selectedIndex = getSelectedIndex();

        if ( selectedIndex.column() == PlistTreeItem::COLUMN_KEY ) {
            treeViewRowCopy();
        }
    }

    if ( event->matches(QKeySequence::Cut) )
    {
        QModelIndex selectedIndex = getSelectedIndex();

        if ( selectedIndex.column() == PlistTreeItem::COLUMN_KEY ) {
            treeViewRowCut();
        }
    }

    if ( event->matches(QKeySequence::Paste) )
    {
        QModelIndex selectedIndex = getSelectedIndex();

        if ( selectedIndex.column() == PlistTreeItem::COLUMN_KEY ) {
            treeViewRowPaste();
        }
    }

    QWidget::keyPressEvent(event);
}



//
// Private
//

void MainWindow::setModel(PlistTreeModel *model)
{
    if (_treeModel != nullptr) {
        ui->treeView->setModel(nullptr);
        delete _treeModel;
        _treeModel = nullptr;
    }

    _treeModel = model;

    //register the model
    ui->treeView->setModel(_treeModel);
    ui->treeView->expandAll();
    ui->treeView->setItemDelegateForColumn(1, new ComboBoxDelegate(PlistTreeItem::ComboBoxTypeStrings()));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}


QModelIndex MainWindow::getSelectedIndex()
{
    QModelIndexList sel = ui->treeView->selectionModel()->selectedIndexes();

    if ( sel.count() == 0 ) {
        return QModelIndex();
    }

    QModelIndex index = sel.at(0);
    return index;
}


void MainWindow::on_action_ExpandAll_triggered()
{
    ui->treeView->expandAll();
}

void MainWindow::on_action_CollapseAll_triggered()
{
    ui->treeView->setAnimated(false);
    ui->treeView->collapseAll();

    QModelIndex rootIndex = ui->treeView->model()->index(0,0);
    ui->treeView->expand(rootIndex);
    ui->treeView->setAnimated(true);
}

void MainWindow::on_action_Copy_triggered()
{
    treeViewRowCopy();
}

void MainWindow::on_action_Paste_triggered()
{
    treeViewRowPaste();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog dialog = AboutDialog(this);
    dialog.exec();
}

void MainWindow::on_actionFind_Replace_triggered()
{
    if  ( _findReplaceDialog == nullptr ) {
        _findReplaceDialog = new FindReplaceDialog(this);
        _findReplaceDialog->setModal(false);
        connect(_findReplaceDialog, SIGNAL(doFindReplace(QString&,QString&,ReplaceTarget,ReplaceMode)), this, SLOT(treeViewFindReplace(QString&,QString&,ReplaceTarget,ReplaceMode)));
    }

    _findReplaceDialog->show();
    _findReplaceDialog->raise();
    _findReplaceDialog->activateWindow();
}
