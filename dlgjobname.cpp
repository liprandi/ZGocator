#include "dlgjobname.h"
#include "ui_dlgjobname.h"

DlgJobName::DlgJobName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgJobName)
{
    ui->setupUi(this);
}

DlgJobName::~DlgJobName()
{
    delete ui;
}
const QString DlgJobName::jobName()
{
    return ui->leJobName->text();
}
