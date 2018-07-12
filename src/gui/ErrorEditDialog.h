#ifndef ERROREDITDIALOG_H
#define ERROREDITDIALOG_H

#include <QtWidgets/QDialog>
#include <vector>
#include <string>
#include "database/CSVReader.h"
class QAbstractButton;

namespace Ui {
    class ErrorEditDialog;
}

class ErrorEditDialog : public QDialog
{
    Q_OBJECT

public:
     ErrorEditDialog(QWidget *parent,
                     std::vector<std::vector<std::string>*>& errors,
                     std::vector<std::string>& headers,
                     std::vector<std::string>& mandatory,
                     std::vector<CSVReader*> vector_reader);
     void setupUI(std::vector<std::vector<std::string>*>& errors,
                  std::vector<std::string>& headers,
                  std::vector<std::string>& mandatory);
    ~ErrorEditDialog();

private slots:
     void on_save_clicked();
     void on_cancel_clicked();

     void on_tableWidget_cellChanged(int row, int column);
     //void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);



     void on_nextButton_clicked();

     void on_prevButton_clicked();

     void on_nextErrButton_clicked();
     void on_prevErrButton_clicked();

private:
    std::vector<CSVReader*> vector_errordialog_reader;
    std::vector<std::vector<std::vector<std::string>*>> errorList;
    std::vector<int> errorSize;
    std::vector<std::vector<std::vector<int>>> errorCellPositionList; // Will be used to store the positions of any red cells in the QTabelWidget.
    std::vector<std::string> headerList;
    std::vector<std::string> mandatoryList;
    Ui::ErrorEditDialog *ui;

    int errorAmount;
    int count_next_and_prev_btn_clicked;
    int errorCellPositionListIndex;

    void saveData();
};

#endif // ERROREDITDIALOG_H
