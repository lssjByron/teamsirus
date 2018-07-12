#include "ui_ErrorEditDialog.h"
#include "ErrorEditDialog.h"

#include <QFileInfo>
#include <QTableWidgetItem>
#include <QStringList>
#include <QDebug>
#include <QBrush>
#include <QColor>
#include <QAbstractButton>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPointF>

#include <iostream>

/*
 * Load data contained in the errors vector into a QWidgetTable
 * Fields will be marked red and editable if they are a mandatory field
 * and editable.  Otherwise all other fields will not be editable.
 * Clicking Save in the dialog will return the corrected entries to the main
 * program through the errors parameter.  If not all marked fields are edited
 * then a warning message will be displayed.  If cancel is clicked all errors
 * are discarded.
 */
ErrorEditDialog::ErrorEditDialog(QWidget *parent,
                                 std::vector<std::vector<std::string>*>& errors,
                                 std::vector<std::string>& headers,
                                 std::vector<std::string>& mandatory,
                                 std::vector<CSVReader*> vector_reader) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    vector_errordialog_reader = vector_reader;
    for(int i = 0; i < vector_reader.size(); i++){
        errorList.push_back(vector_reader[i]->getFileErrors());
    }
    //errorList.push_back(errors);
    headerList = headers;
    mandatoryList = mandatory;
    count_next_and_prev_btn_clicked = 0;
    ui = new Ui::ErrorEditDialog;
    ui->setupUi(this);
    QString temp_path = QString::fromStdString(vector_errordialog_reader[count_next_and_prev_btn_clicked]->getPath());
    ui->label_full_filename->setText(temp_path);
    QFileInfo filepath(temp_path);
    ui->label_filename->setText(filepath.fileName());

    //tableWidget shadow
//    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
//    shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(35.0);
//    ui->tableWidget->setGraphicsEffect(shadowEffect);



    setupUI(errorList[0], headerList, mandatoryList);

    //SHOW FULL SCREEN WINDOW
    this->showFullScreen();

}

void ErrorEditDialog::setupUI(std::vector<std::vector<std::string>*>& errors,
                              std::vector<std::string>& headers,
                              std::vector<std::string>& mandatory){
    // To remove any possible Segmentation Faults, we disconnect all signals prior to the setup of the UI
    ui->tableWidget->blockSignals(true);


    ui->tableWidget->setRowCount((int) errors.size());
    ui->tableWidget->setColumnCount((int) headers.size());

    // not used -- for now
    //Update the current amount of errors
    //errorSize.push_back(errors.size());



    //Note: this seems pointless, just use headers

    QStringList listHeaders;
    for (int i = 0; i < (int) headers.size(); i++) {
        listHeaders << headers[i].c_str();
    }

    ui->tableWidget->setHorizontalHeaderLabels(listHeaders);
    int tmpErrAmount = 0;
    errorAmount = 0;

    QTableWidgetItem* item;
    QBrush brush(QColor(255, 47, 156, 50));
    std::vector<std::vector<std::string>*>::iterator it;

    // create a vector of the postion (x,y) for each red cell
    std::vector<int> v(2);
    v = { 0,0 };
    std::vector<std::vector<int>> errs;
    int row = 0;
    for (it = errors.begin(); it != errors.end(); it++) {
        for (int col = 0; col < (int) headers.size() && col < (int) (*it)->size(); col++) {
            item = new QTableWidgetItem();
            Qt::ItemFlags flag = item->flags();
            //item->setFlags(Qt::ItemIsEditable);   <-- Not needed anymore since ALL the fields in the tableWidget must now be editable.
            item->setText((*it)->at(col).c_str());
            for (int i = 0; i < (int) mandatory.size(); i++) {
                if (mandatory[i].compare(headers.at(col)) == 0
                        && (*it)->at(col).compare("") == 0) {
                    item->setBackground(brush);
                    item->setFlags(flag);
                    // Here we increase the amount of errors
                    tmpErrAmount++;
                    v = {row, col};
                    errs.push_back(v);
                    //Since the current item is a red one (an error) we append it to to the errorCellPositionList
                }
            }
            ui->tableWidget->setItem(row, col, item);
        }
        row++;
    }
    if(errorCellPositionList.size() < vector_errordialog_reader.size())
        errorCellPositionList.push_back(errs);

    // update the amount of errors
    errorAmount = tmpErrAmount;
    // Set the text of the Error count
    ui->label->setText("File Contains " + QString::number(errorAmount) + " Errors");

    // //tmp
    errorCellPositionListIndex = 0;
    // //


    // Once the UI has been set back, all signals are restored
    ui->tableWidget->blockSignals(false);

}

//Clean up allocated memory for the table items
ErrorEditDialog::~ErrorEditDialog()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            delete ui->tableWidget->item(i,j);
        }
    }
    delete ui;
}

//Save the new data entered by the user via the error reference var
void ErrorEditDialog::saveData() {
    for(int i = 0; i < vector_errordialog_reader.size(); i++){
        //errorList = vector_errordialog_reader[i]->getFileErrors();
        std::vector<std::string> heads = vector_errordialog_reader[i]->getHeaders();
        std::vector<std::vector<std::string>*> f_errs = vector_errordialog_reader[i]->getFileErrors();
        for (int row = 0; row < f_errs.size(); row++) {
            int checker = (int) errorList[i][row]->size();
            for (int col = 0; col < heads.size() && col < (int) errorList[i][row]->size(); col++) {
                std::vector<std::string>::iterator it = errorList[i][row]->begin()+col;
                int dblcheck = (int)f_errs[row]->size();
                if (errorList[i][row]->at(col).compare("") == 0) {
                    it = errorList[i][row]->erase(it);
                    //std::string checker = ui->tableWidget->item(row, col)->text().toStdString();
                    if(col < f_errs[row]->size()){
                        errorList[i][row]->insert(it, f_errs[row]->at(col));
                    }
                }

            }
        }
    }
    //errorList = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getFileErrors();
    accept();
}

void ErrorEditDialog::on_save_clicked()
{
    bool search = true;
    unsigned int j;
    for(int counter = 0; counter < vector_errordialog_reader.size(); counter++){
        std::vector<std::vector<std::string>*> f_errs = vector_errordialog_reader[counter]->getFileErrors();
        std::vector<std::string> mand_fields = vector_errordialog_reader[counter]->getMandatoryFields();
        RecordsManager* record_manager = vector_errordialog_reader[counter]->getRecordManager();
        for (int i = 0; i < (int) f_errs.size(); i++) {
            for (j = 0; j < mand_fields.size(); j++) {
                int index = record_manager->getHeaderIndex(mand_fields[j]);
                if (f_errs[i]->at(index).compare("") == 0) {
                    QMessageBox::critical(this, "Error", "Mandatory fields are still empty.");
                    search = false;
                    break;
                }
            }
            if(search == false)
                break;
        }
    }
    if (search) {
        saveData();
    }
}

void ErrorEditDialog::on_cancel_clicked()
{
    reject();
}

/*
 * Since the customer needs all the fields editable, we now must reimplement the old functionality to decresase the error amount:
 *  1) Check if the current cell is red
 *  2) Check if the current cell is null and is changed to something other than null.
 *
 * If 1) and 2) are met, we decrease the error amount and make that error cell no an error cell (ie: change the color of the cell from red to std)
 *
 * Notes:
 * This signal is sent only once the QTableWidget has had setData() issued to it. So this fires once the user has entered new text and either clicked on another cell or clicked "enter"
 */
void ErrorEditDialog::on_tableWidget_cellChanged(int row, int column)
{
    // First check if we are not dereferencing a null pointer
    if (ui->tableWidget->item(row,column) != 0) {
        // Check if current cell being changed is red or not
        if (ui->tableWidget->item(row,column)->background().color() == QColor(255, 47, 156, 50) ) {
            //If the cell IS red, we check if it's being changed from "" --> "something"
            // If the cell is being changed from "" --> something meaningful
            if ( (vector_errordialog_reader[count_next_and_prev_btn_clicked]->getFileErrors()[row]->at(column) == "") &&
                    !(ui->tableWidget->currentItem()->text().isEmpty())) {

                errorAmount = errorAmount - 1;
                ui->tableWidget->currentItem()->setBackground(QColor(0,0,0,0));

                // TODO: Now that the error has been processed, we remove it from the list of errors
                // Now that the error has been processed...we check if the error count is now 0. if It is we hide the navigation button

                if (errorAmount == 0) {
                    //just make the All elements transparent with CSS
                    ui->nextErrButton->setStyleSheet("QPushButton#prevErrButton{background-color: rgba(0,0,0,0); color: rgba(0,0,0,0);}");
                    ui->prevErrButton->setStyleSheet("QPushButton#prevErrButton{background-color: rgba(0,0,0,0); color: rgba(0,0,0,0);}");
                }
            }
        }
        // update the label
        if(row > -1 && errorAmount >= 0){
            vector_errordialog_reader[count_next_and_prev_btn_clicked]->setFileErrors(row,column, ui->tableWidget->currentItem()->text().toStdString());
            ui->label->setText("File Contains " + QString::number(errorAmount) + " Errors");

            //Since the errorAmount is still
            ui->nextErrButton->setStyleSheet("QPushButton#prevErrButton{background-color: rgba(0,0,0,0); color: rgba(0,0,0,0);}");
            ui->prevErrButton->setStyleSheet("QPushButton#prevErrButton{background-color: rgb(3, 41, 85);; color: white;}");
        }
    }
}

void ErrorEditDialog::on_nextButton_clicked()
{
    if(count_next_and_prev_btn_clicked < vector_errordialog_reader.size() - 1)
        ++count_next_and_prev_btn_clicked;

    QString temp_path = QString::fromStdString(vector_errordialog_reader[count_next_and_prev_btn_clicked]->getPath());
    ui->label_full_filename->setText(temp_path);
    QFileInfo filepath(temp_path);
    ui->label_filename->setText(filepath.fileName());
    std::vector<std::vector<std::string>*> errs = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getFileErrors();
    std::vector<std::string> headers = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getHeaders();
    std::vector<std::string> mandatory_fields = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getMandatoryFields();
    setupUI(errs, headers, mandatory_fields);


}

void ErrorEditDialog::on_prevButton_clicked()
{
    if(count_next_and_prev_btn_clicked != 0)
        --count_next_and_prev_btn_clicked;
    QString temp_path = QString::fromStdString(vector_errordialog_reader[count_next_and_prev_btn_clicked]->getPath());
    ui->label_full_filename->setText(temp_path);
    QFileInfo filepath(temp_path);
    ui->label_filename->setText(filepath.fileName());
    std::vector<std::vector<std::string>*> errs = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getFileErrors();
    std::vector<std::string> headers = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getHeaders();
    std::vector<std::string> mandatory_fields = vector_errordialog_reader[count_next_and_prev_btn_clicked]->getMandatoryFields();
    setupUI(errs, headers, mandatory_fields);

}


//when the user clicks to advance to the next or previous error. IE: the next or previous red field in relation to the current selected one
void ErrorEditDialog::on_nextErrButton_clicked() {
    // Get the current cell and get the next RED cell and udpate the currentCell

    //Procedure on how to do it - I THINK, make a vector for this class with the following format:
    //  [ [i,j],[i,j],...,[i,j] ]
    //      where each pair represents the position of one of the red cells (this vector will be updated when the cells are being created **see line 83)
    //  Then we call the following QTableWidget function so that the user can edit it
    //      http://doc.qt.io/qt-4.8/qtablewidget.html#setCurrentCell
    ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    if (errorCellPositionListIndex < errorCellPositionList[count_next_and_prev_btn_clicked].size()-1) {
        errorCellPositionListIndex++;
    }

    ui->tableWidget->setCurrentCell(errorCellPositionList[count_next_and_prev_btn_clicked].at(this->errorCellPositionListIndex).at(0),
                                    errorCellPositionList[count_next_and_prev_btn_clicked].at(this->errorCellPositionListIndex).at(1),
                                    QItemSelectionModel::SelectCurrent);
    /*
    // go to the next largest position in the table widget
    for(std::vector<T>::iterator it = errorCellPositionList.begin(); it != errorCellPositionList.end(); ++it) {
        std::cout << *it << endl;


    }
    */
}
void ErrorEditDialog::on_prevErrButton_clicked() {
    //Do the above backwards

    ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    if (errorCellPositionListIndex != 0) {
        errorCellPositionListIndex--;
    }

    ui->tableWidget->setCurrentCell(errorCellPositionList[count_next_and_prev_btn_clicked].at(this->errorCellPositionListIndex).at(0),
                                    errorCellPositionList[count_next_and_prev_btn_clicked].at(this->errorCellPositionListIndex).at(1),
                                    QItemSelectionModel::SelectCurrent);
}


