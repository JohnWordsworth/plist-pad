#include "FindReplaceDialog.h"
#include "ui_FindReplaceDialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) : QDialog(parent), ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);

    ui->comboBoxMode->addItem("Normal");

    ui->comboBoxTarget->addItem("Keys and Values");
    ui->comboBoxTarget->addItem("Values Only");
    ui->comboBoxTarget->addItem("Keys Only");
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::on_pushButton_clicked()
{
    ReplaceTarget target = ReplaceAll;
    ReplaceMode mode = ReplaceModeNormal;

    switch( ui->comboBoxTarget->currentIndex() )
    {
    case 0: target = ReplaceAll; break;
    case 1: target = ReplaceValue; break;
    case 2: target = ReplaceKey; break;
    }

    emit doFindReplace(ui->lineEditFind->text(), ui->lineEditReplace->text(), target, mode);
}
