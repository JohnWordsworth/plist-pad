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

#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include "../model/PlistTreeModel.h"


namespace Ui {
    class FindReplaceDialog;
}


/**
 * A simple find replace dialog which presents text fields for the find/replace text
 * parameters and combo boxes for the target/mode of the find replace operation.
 * Emits a doFindReplace signal whenever the user clicks on the 'Find / Replace All'
 * button.
 */
class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = 0);
    ~FindReplaceDialog();

signals:
    void doFindReplace(QString &find, QString &replace, ReplaceTarget target = ReplaceAll, ReplaceMode mode = ReplaceModeNormal);

    
private slots:
    void on_pushButton_clicked();

private:
    Ui::FindReplaceDialog *ui;
};

#endif // FINDREPLACEDIALOG_H
