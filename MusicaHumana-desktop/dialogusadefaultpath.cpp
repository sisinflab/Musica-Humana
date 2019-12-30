#include "dialogusadefaultpath.h"
#include "ui_dialogusadefaultpath.h"

DialogUsaDefaultPath::DialogUsaDefaultPath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUsaDefaultPath)
{
    ui->setupUi(this);

    ui->buttonBox->addButton("  Si  ", QDialogButtonBox::YesRole);
    ui->buttonBox->addButton(" No ", QDialogButtonBox::NoRole);

}

DialogUsaDefaultPath::~DialogUsaDefaultPath()
{
    delete ui;
}

bool DialogUsaDefaultPath::isCheckedNonChiederePiu()
{
    if (ui->checkBoxNonChiedere->isChecked()){
        return true;
    } else {
        return false;
    }
}
