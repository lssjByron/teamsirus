#include "categoryfilter.h"
#include "ui_categoryfilter.h"
#include <QMessageBox>

CategoryFilter::CategoryFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CategoryFilter)
{
    ui->setupUi(this);
}

CategoryFilter::~CategoryFilter()
{
    delete ui;
}

QString CategoryFilter::getCategoryFilterName() {
    return categoryFilterName;
}

QString CategoryFilter::getSubCategoryText() {
    return ui->subCategory_Drop->currentText();
}

QString CategoryFilter::getCatEntitiesText() {
    return ui->categoryEnties_Drop->currentText();
}

void CategoryFilter::on_buttonBox_accepted()
{
    if (ui->saveBox->text()!="") {
        categoryFilterName = ui->saveBox->displayText();
    }
    else {
        QMessageBox::critical(this, "", "Please enter a name.");
    }
}

void CategoryFilter::updateSubCategory_Drop(std::vector<std::string> subCatHeaders) {
    std::string str;

    ui->subCategory_Drop->addItem("Division");
    /*
    for (int i=0; i < subCatHeaders.size(); i++) {
        str = subCatHeaders[i];

        ui->subCategory_Drop->addItem(QString::fromStdString(str));
    }
    */
}
void CategoryFilter::updateCategoryEnties_Drop(std::vector<std::vector<std::string>> uniqueMembersIn) {
    uniqueMembers = uniqueMembersIn;
}


void CategoryFilter::on_buttonBox_rejected()
{
    done(0);
}


// When the user clicks a new memeber on the left hand side, update the right hand side with the unique members
void CategoryFilter::on_subCategory_Drop_currentIndexChanged(int index)
{

    ui->categoryEnties_Drop->clear();

    std::string str;

    for (int i=0; i < this->uniqueMembers.at(ui->subCategory_Drop->currentIndex()).size(); i++) {

        str = this->uniqueMembers.at(ui->subCategory_Drop->currentIndex()).at(i);

        QString qstr = QString::fromStdString(str);
        ui->categoryEnties_Drop->addItem(qstr);
    }


}
