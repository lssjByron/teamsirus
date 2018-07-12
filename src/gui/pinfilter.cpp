#include "pinfilter.h"
#include "ui_pinfilter.h"
#include <QMessageBox>

PinFilter::PinFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PinFilter)
{
    ui->setupUi(this);
}

PinFilter::~PinFilter()
{
    delete ui;
}

QString PinFilter::getPinnedFilterName() {
    return pinnedFilterName;
}

void PinFilter::on_buttonBox_accepted()
{
    if (ui->pinName->text()!="") {
        // save the current filter shit

        pinnedFilterName = ui->pinName->displayText();
    }
    else {
        QMessageBox::critical(this, "", "Please enter a name.");
    }
}

void PinFilter::on_buttonBox_rejected()
{
    done(0);
}
