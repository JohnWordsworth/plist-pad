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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include "dialogs/AboutDialog.h"
#include "dialogs/FindReplaceDialog.h"
#include "model/PlistTreeModel.h"
#include "model/PlistTreeWriter.h"
#include "model/PlistTreeReader.h"
#include "ComboBoxDelegate.h"


extern const QString kATitle;
extern const QString kAVersion;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /** Process any commands that come through to the main window and weren't swallowed up by text edit boxes or the tree widget. */
    void keyPressEvent(QKeyEvent *);

public slots:
    void showTreeViewContextMenu(const QPoint& pos);
    void treeViewAddChildToSelection();
    void treeViewAddSiblingBeforeSelection();
    void treeViewAddSiblingAfterSelection();
    void treeViewRemoveSelectedRow();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void treeViewRowCopy();
    void treeViewRowCut();
    void treeViewRowPaste();
    void treeViewFindReplace(QString &find, QString &replace, ReplaceTarget target, ReplaceMode mode);

private slots:
    void on_actionSave_As_triggered();

    void on_action_Save_triggered();

    void on_action_New_triggered();

    void on_action_Open_triggered();

    void on_action_ExpandAll_triggered();

    void on_action_CollapseAll_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_actionExit_triggered();

    void on_action_About_triggered();

    void on_actionFind_Replace_triggered();

private:
    Ui::MainWindow *ui;

    FindReplaceDialog *_findReplaceDialog;

    QString _openFileName;
    PlistTreeModel *_treeModel;

    void setModel(PlistTreeModel *model);
    QModelIndex getSelectedIndex();
};

#endif // MAINWINDOW_H
