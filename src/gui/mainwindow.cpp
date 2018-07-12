#include "mainwindow.h"

#include "database/CSVReader.h"
#include "database/QSortListIO.h"
#include "database/RecordsManager.h"
#include "datamodel/TreeModel.h"
#include "datamodel/GrantFundingTreeModel.h"
#include "datamodel/PresentationTreeModel.h"
#include "datamodel/PublicationTreeModel.h"
#include "datamodel/TeachingTreeModel.h"
#include "gui/collabwebgraph.h"
#include "gui/pinfilter.h"
#include "database/sessionsavingio.h"
#include <iostream>
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QFile>
#include "gui/categoryfilter.h"
using namespace std;

#define FUNDORDER_SAVE  "fundsortorder.dat"
#define PRESORDER_SAVE  "pressortorder.dat"
#define PUBORDER_SAVE   "pubsortorder.dat"
#define TEACHORDER_SAVE "teachsortorder.dat"
#define SAVE "save.dat"

std::vector<std::string> MainWindow::GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
std::vector<std::string> MainWindow::PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
std::vector<std::string> MainWindow::PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
std::vector<std::string> MainWindow::TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    record_manager(NULL) {
    // draw GUI elements
    ui->setupUi(this);

//    int idd = QFontDatabase::addApplicationFont(":/Fonts/HelveticaNeueMedium.ttf");
//    QString family = QFontDatabase::applicationFontFamilies(idd).at(0);
//    QFont mediumFont(family);
//    ui->filesLabel_2->setFont(mediumFont);

    // Make the tab's hover cursor the hand point
    QList<QTabBar *> tabBar = this->ui->categoryTab->findChildren<QTabBar *>();
    tabBar.at(0)->setCursor(Qt::PointingHandCursor);

    this->showMaximized();

    // set up the logo                  <-- This is not needed anymore
    //QPixmap logo(":/logo.png");
    //ui->logo_label->setPixmap(logo);

    // set up application icon
    QIcon icon(":/icon32.ico");
    MainWindow::setWindowIcon(icon);

    // set the date format
    QString dFormat("yyyy");
    ui->FromDate->setDisplayFormat(dFormat);
    ui->ToDate->setDisplayFormat(dFormat);

    // set start year to be 1950
    QDate startDate(1950, 1, 1);

    // set ranges for spinbox
    ui->FromDate->setDateRange(startDate, QDate::currentDate());
    ui->ToDate->setDateRange(startDate, QDate::currentDate());
    ui->FromDate_2->setDateRange(startDate, QDate::currentDate());
    ui->ToDate_2->setDateRange(startDate, QDate::currentDate());
    ui->FromDate_3->setDateRange(startDate, QDate::currentDate());
    ui->ToDate_3->setDateRange(startDate, QDate::currentDate());
    ui->FromDate_4->setDateRange(startDate, QDate::currentDate());
    ui->ToDate_4->setDateRange(startDate, QDate::currentDate());

    // set default dates (1950->current)
    //1
    ui->FromDate->setDate(startDate);
    ui->ToDate->setDate(QDate::currentDate());
    //0
    ui->FromDate_2->setDate(startDate);
    ui->ToDate_2->setDate(QDate::currentDate());
    //2
    ui->FromDate_3->setDate(startDate);
    ui->ToDate_3->setDate(QDate::currentDate());
    //3
    ui->FromDate_4->setDate(startDate);
    ui->ToDate_4->setDate(QDate::currentDate());
    // set some member variables to the current date values
    yearStart = 1950;
    yearEnd = QDate::currentDate().year();

    //default print is disabled
    ui->teachPrintButton->setEnabled(false);                    // Ryan: THERE ARE A LOT OF DISABLED BUTTONS IN MANY OF THE TABS EVEN AFTER LOADING FILES; I THINK THIS IS WHERE THE PROBLEM LIES - HAVE TO ENABLE ALL BUTTONS FOR EACH TAB ONCE A FILE IS LOADED?
    ui->fundPrintButton->setEnabled(false);
    ui->pubPrintButton->setEnabled(false);
    ui->presPrintButton->setEnabled(false);

    ui->teachExportButton->setEnabled(false);
    ui->fundExportButton->setEnabled(false);
    ui->pubExportButton->setEnabled(false);
    ui->presExportButton->setEnabled(false);

    //Setup printer
    printer = new QPrinter();

    dateChanged = {false, false, false, false};

    //For Mac: Disable blue focus outline on widget itmes
    ui->filesListWidgetTeach->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->filesListWidgetFund->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->filesListWidgetPres->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->filesListWidgetPub->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->FromDate->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->FromDate_2->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->FromDate_3->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->FromDate_4->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ToDate->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ToDate_2->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ToDate_3->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ToDate_4->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->teachTreeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pubTreeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->presTreeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->fundTreeView->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->teach_load_file->setFocus();
    ui->pub_load_file->setFocus();
    ui->pres_load_file->setFocus();
    ui->fund_load_file->setFocus();

    // Initalize the pinned filters
    this->createDefaultPinnedFilter();

}

MainWindow::~MainWindow() {
    //Sam: This is where I'm going to implement my session saving
    // Confirmed that all_readers can be accessed/ this occurs on program shut down
    saveSession();
    delete ui;

    delete record_manager;
    delete printer;
}

//method that will save the session
// This is what i'm thinking, first save the paths of the files
// then save the 3 orders, and then save the current index opened, shouldn't be too
// hard
void MainWindow::saveSession(){
    //lets begin by seeing what all_readers holds, each index holds a seperate file
    data << QString::number(all_readers.size());
    if (all_readers.size() != 0){
        for (int i = 0; i < all_readers.size(); i++){
            // stores the path of the files opened
            std::string path = all_readers[i]->getPath();
            //TODO Save the path to a file, so first I am going to save the file paths
            //data = data + QString::fromStdString(path) + "/n";
            data << QString::fromStdString(path);
        }
        //now that that's out of the way, lemme try to find the current index that was open
        currentIndex = ui->categoryTab->currentIndex();
        // confirmed that current index now holds the tab that was last opened
        data << QString::number(currentIndex);
        for (int i = 0; i < 4; i++){
            saveFilter(i);
        }
    }
    bool ok;
    QString text = QInputDialog::getText(this, tr("Save Session"),
                                             tr("Type name if you would like to save:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty()){
        SessionSavingIO saveFile(text.toStdString() + ".dat");
        saveFile.write(data);
    }

}

void MainWindow::loadSession(QString fileName){
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            qDebug() << line;
            int numOfFiles = line.toInt();
            qDebug() << "The number of files is " << QString::number(numOfFiles);
            for (int i = 0; i < numOfFiles; i++){
                line = in.readLine();
                QString file = line;
                qDebug() << "The file to open is " << file;
                reader = new CSVReader(file.toStdString());
                current_readers.push_back(reader);
                all_readers.push_back(reader);
                record_manager = current_readers[i]->getRecordManager();
                determine_load_file_type(current_readers[i]->getIndexFileType(),file);
            }
            current_readers.clear();
            line = in.readLine();
            int currentIndex = line.toInt();
            qDebug() << "The current index is " << QString::number(currentIndex);
            ui->categoryTab->setCurrentIndex(currentIndex);
            line = in.readLine();
            int isTeach = line.toInt();
            qDebug() << "Is there a teaching file opened? " << QString::number(isTeach);
            if (isTeach == 1) {
                ui->FromDate_2->setDate(QDate::fromString(in.readLine()));
                ui->ToDate_2->setDate(QDate::fromString(in.readLine()));
                ui->teach_filter_from->setText(in.readLine());
                ui->teach_filter_to->setText(in.readLine());
                int numOfOrders = in.readLine().toInt();
                for (int i = 0; i < numOfOrders; i++){
                    in.readLine();
                }
            }
            line = in.readLine();
            int isPub = line.toInt();
            if (isPub == 1){
                ui->FromDate_2->setDate(QDate::fromString(in.readLine()));
                ui->ToDate_2->setDate(QDate::fromString(in.readLine()));
                ui->pub_filter_from->setText(in.readLine());
                ui->pub_filter_to->setText(in.readLine());
                int numOfOrders = in.readLine().toInt();
                for (int i = 0; i < numOfOrders; i++){
                    in.readLine();
                }
            }
            line = in.readLine();
            int isPres = line.toInt();
            if (isPres == 1){
                ui->FromDate_2->setDate(QDate::fromString(in.readLine()));
                ui->ToDate_2->setDate(QDate::fromString(in.readLine()));
                ui->pres_filter_from->setText(in.readLine());
                ui->pres_filter_to->setText(in.readLine());
                int numOfOrders = in.readLine().toInt();
                for (int i = 0; i < numOfOrders; i++){
                    in.readLine();
                }
            }
            line = in.readLine();
            int isFund = line.toInt();
            if (isFund == 1){
                ui->FromDate_2->setDate(QDate::fromString(in.readLine()));
                ui->ToDate_2->setDate(QDate::fromString(in.readLine()));
                ui->fund_filter_from->setText(in.readLine());
                ui->fund_filter_to->setText(in.readLine());
                int numOfOrders = in.readLine().toInt();
                for (int i = 0; i < numOfOrders; i++){
                    in.readLine();
                }
            }
            break;
        }
        for (int i = 0; i < 4; i++){
            refresh(i);
        }
        inputFile.close();
    }
}

void MainWindow::saveFilter(int index){
    bool flag = false;
    switch (index){
    case TEACH:
        for (int i = 0; i < all_readers.size(); i++){
            qDebug() << QString::number(all_readers[i]->getIndexFileType());
            if (all_readers[i]->getIndexFileType() == 0){
                data << QString::number(1);
                flag = true;
                break;
            }
        }
        qDebug() << flag;
        if (flag){
            data << (teachSaveFrom.toString());
            qDebug() << QString::number(teachYears[0]);
            qDebug() << QString::number(teachYears[1]);
            data<< (teachSaveTo.toString());
            data << QChar(getFilterStartChar(TEACH));
            data << QChar(getFilterEndChar(TEACH));
            data << QString::number(allTeachOrders.size());
            for (int i = 0; i < allTeachOrders.size(); i++){
                QString teachOrder = allTeachOrders[i].join(",");
                data << teachOrder;
            }
        }
        else {
            data << QString::number(0);
        }
        break;
    case PRESENTATIONS:
        for (int i = 0; i < all_readers.size(); i++){
            if (all_readers[i]->getIndexFileType() == 2){
                data << QString::number(1);
                flag = true;
                break;
            }
        }
        if (flag){
            data << (presSaveFrom.toString());
            data<< (presSaveTo.toString());
            data << QChar(getFilterStartChar(PRESENTATIONS));
            data << QChar(getFilterEndChar(PRESENTATIONS));
            data << QString::number(allPresOrders.size());
            for (int i = 0; i < allPresOrders.size(); i++){
                QString presOrder = allPresOrders[i].join(",");
                data << presOrder;
            }
        }
        else {
            data << QString::number(0);
        }
        break;
    case PUBLICATIONS:
        for (int i = 0; i < all_readers.size(); i++){
            if (all_readers[i]->getIndexFileType() == 1){
                data << QString::number(1);
                flag = true;
                break;
            }
        }
        if (flag){
            data << (pubSaveFrom.toString());
            data<< (pubSaveTo.toString());
            data << QChar(getFilterStartChar(PUBLICATIONS));
            data << QChar(getFilterEndChar(PUBLICATIONS));
            data << QString::number(allPubOrders.size());
            for (int i = 0; i < allPubOrders.size(); i++){
                QString pubOrder = allPubOrders[i].join(",");
                data << pubOrder;
            }
        }
        else {
            data << QString::number(0);
        }
        break;
    case FUNDING:
        for (int i = 0; i < all_readers.size(); i++) {
            if (all_readers[i]->getIndexFileType() == 3){
                data << QString::number(1);
                flag = true;
                break;
            }
        }
        if (flag){
            data << (fundSaveFrom.toString());
            data<< (fundSaveTo.toString());
            data << QChar(getFilterStartChar(FUNDING));
            data << QChar(getFilterEndChar(FUNDING));
            data << QString::number(allFundOrders.size());
            for (int i = 0; i < allFundOrders.size(); i++){
                QString fundOrder = allFundOrders[i].join(",");
                data << fundOrder;
            }
        }
        else {
            data << QString::number(0);
        }
        break;
    }
}

void MainWindow::on_actionLoad_file_triggered() {
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          "Select one or more files to load",
                                                          QDir::currentPath(),
                                                          tr("CSV (*.csv);; All files (*.*)"));
    if (filePaths.size() > 0) {
        const int NUM_TABS = 4;
        bool all_loaded[NUM_TABS] = {false, false, false, false};
        int sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
        QStringList::Iterator it = filePaths.begin();
        while (sum != NUM_TABS && it != filePaths.end()) {
            QString path = it[0];
            //note short-circuit eval
            if (!all_loaded[FUNDING] && load_fund(path)) {
                all_loaded[FUNDING] = true;
            } else if (!all_loaded[TEACH] && load_teach(path)) {
                all_loaded[TEACH] = true;
            } else if (!all_loaded[PUBLICATIONS] && load_pub(path)) {
                all_loaded[PUBLICATIONS] = true;
            } else if (!all_loaded[PRESENTATIONS] && load_pres(path)) {
                all_loaded[PRESENTATIONS] = true;
            }
            sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
            ++it;
        }
    }
}


/*
 * When the load file button is clicked a file open dialog box
 * is opened.  If a file name is successfully returned, makeTree()
 * is called.
 */
void MainWindow::load_file() {
    QStringList filePath = QFileDialog::getOpenFileNames(this, "Open File", QDir::currentPath(),
                                                    tr("CSV (*.csv);; All files (*.*)"));

    if (!filePath.isEmpty()) {
        for(int i = 0; i < filePath.size(); i++){
            reader = new CSVReader(filePath[i].toStdString());
            current_readers.push_back(reader);
            all_readers.push_back(reader);
        }

        bool is_valid_files = true;
        for(int i = 0; i < filePath.size(); i++){
            if( checkFile(i, filePath[i]) == -1){
                QMessageBox::critical(this, "Invalid File", "Not a valid file.");
                is_valid_files = false;
            }
        }
        if(is_valid_files){
            for(int i = 0; i < filePath.size(); i++){
                record_manager = current_readers[i]->getRecordManager();
                determine_load_file_type(current_readers[i]->getIndexFileType(), filePath[i]);
            }
        }

         current_readers.clear();
    }
}

void MainWindow::refresh(int tabIndex) {
    // if we've loaded in a file, update that data
    switch (tabIndex) {
    case FUNDING:
        if (!fundPath.isEmpty()) {
            current_path = ui->filesListWidgetFund->currentItem()->text().toStdString();
            makeTree(FUNDING);
            ui->fund_graph_stackedWidget->hide();
            ui->fundGraphTitle->clear();
        }
        break;

    case PRESENTATIONS:
        if (!presPath.isEmpty()) {
            current_path = ui->filesListWidgetPres->currentItem()->text().toStdString();
            makeTree(PRESENTATIONS);
            ui->pres_graph_stackedWidget->hide();
            ui->presGraphTitle->clear();
        }
        break;
    case PUBLICATIONS:
        if (!pubPath.isEmpty()) {
            current_path = ui->filesListWidgetPub->currentItem()->text().toStdString();
            makeTree(PUBLICATIONS);
            ui->pubGraphTitle->clear();
        }
        break;

    case TEACH:
        if (!teachPath.isEmpty()) {
            current_path = ui->filesListWidgetTeach->currentItem()->text().toStdString();
            makeTree(TEACH);
            ui->teach_graph_stackedWidget->hide();
            ui->teachGraphTitle->clear();
        }
        break;
    }
}

int MainWindow::checkFile(int index, QString filePath) {

    // NOTE: This is how team peach would check the validity of each input file
    // std::find(header.begin(), header.end(), searchstring) != header.end()


    int index_type = current_readers[index]->getIndexFileType();


    switch (index_type) {

    case TEACH:
        // read if first or if a new file is loaded
        if (!teachPath.isEmpty() || !filePath.compare(teachPath))
            return TEACH;

        ui->teachPrintButton->setEnabled(true);
        ui->teachExportButton->setEnabled(true);
        break;

    case PUBLICATIONS:
        // read if first or if a new file is loaded
        if (!pubPath.isEmpty() || !filePath.compare(presPath))
            return PUBLICATIONS;

        ui->pubPrintButton->setEnabled(true);
        ui->pubExportButton->setEnabled(true);
        break;

    case PRESENTATIONS:
        // read if first or if a new file is loaded
        if (!presPath.isEmpty() || !filePath.compare(presPath))
            return PRESENTATIONS;

        ui->presPrintButton->setEnabled(true);
        ui->presExportButton->setEnabled(true);
        break;

    case FUNDING:
        // read if first or if a new file is loaded
        if (!fundPath.isEmpty() || !filePath.compare(fundPath))
            return FUNDING;

        ui->fundPrintButton->setEnabled(true);
        ui->fundExportButton->setEnabled(true);
        break;
    case -1:
        return -1;
    }

    std::vector<std::vector<std::string>*> f_errs = current_readers[index]->getFileErrors();
    int sortHeaderIndex = current_readers[index]->getSortHeaderIndex();


    if(index == current_readers.size() - 1){


        if (f_errs.size() > 0) {
            if(handle_field_errors(f_errs)) {
                for(int counter = 0; counter < current_readers.size(); counter++){
                    f_errs = current_readers[counter]->getFileErrors();
                    record_manager = current_readers[counter]->getRecordManager();
                    for (unsigned int i = 0; i < f_errs.size(); i++) {
                        record_manager->addRecord(reader->parseDateString((*(f_errs[i]))[sortHeaderIndex]), f_errs[i]);
                    }
                }
            }
        }
    }

    return index;

}

void MainWindow::createDefaultSortOrder(int tabIndex) {
    QStringList defaultOrder;
    defaultOrder << "Default";

    switch (tabIndex) {
    case FUNDING:
        // specify default sort order
        defaultOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";

        // add default list to member variable
        allFundOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveFundSort(FUNDORDER_SAVE);
        saveFundSort.saveList(allFundOrders);
    }

        break;
    case PRESENTATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPresOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePresSort(PRESORDER_SAVE);
        savePresSort.saveList(allPresOrders);
    }

        break;
    case PUBLICATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPubOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePubSort(PUBORDER_SAVE);
        savePubSort.saveList(allPubOrders);
    }

        break;
    case TEACH:
        // specify default sort order
        defaultOrder << "Member Name" << "Program";

        // add default list to member variable
        allTeachOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveTeachSort(TEACHORDER_SAVE);
        saveTeachSort.saveList(allTeachOrders);
    }

        break;
    }
}
void MainWindow::createDefaultPinnedFilter() {
    QStringList defaultPinFilter;
    defaultPinFilter << "Default";

    allTeachPin << defaultPinFilter;
    allPubPin << defaultPinFilter;
    allPresPin << defaultPinFilter;
    allFundPin << defaultPinFilter;

}

/*
 * err: vector of pointers to record entries.
 * headers: vector of strings containing the db headers
 * mandatory: vector of strings containing the mandatory fields in the db
 *
 * Returns true when the passed err vector contains edited fields.
 * Returns false when no fields have been edited.
 */
bool MainWindow::handle_field_errors(std::vector<std::vector<std::string>*>& err) {
    std::vector<std::string> headers = current_readers[0]->getHeaders();
    std::vector<std::string> mandatory = current_readers[0]->getMandatoryFields();

    //Return false; there are no errors to correct
    if (err.size() == 0) {
        return false;
    }
    QMessageBox prompt;
    QString mainText = "File contains ";
    mainText.append(QString::number(err.size()));
    mainText.append(" records with missing mandatory fields.");
    prompt.setText(mainText);
    prompt.setInformativeText("Do you want to edit these entries or discard?");
    prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    prompt.setDefaultButton(QMessageBox::Yes);
    prompt.setButtonText(QMessageBox::Yes, "Edit");
    prompt.setButtonText(QMessageBox::No, "Discard");
    prompt.setWindowIcon(QIcon(":/icon32.ico"));
    int ret = prompt.exec();

    switch (ret) {
    case QMessageBox::Yes: {

        std::vector<std::vector<std::string>*> errs = current_readers[0]->getFileErrors();
        ErrorEditDialog diag(this, errs, headers, mandatory, current_readers);

        if(diag.exec()) {
            return true;
        }
        return false;
    }

    case QMessageBox::No:
    default:
        return false;
    }
}

/*
 * Builds a TreeModel for the ui's embedded treeview.
 */
void MainWindow::makeTree(int tabIndex) {
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }

    std::vector<std::string> filter_data;
    std::map<int, std::vector<std::string>> map_filtered_data;
    std::map<int, std::vector<std::string>> map_filtered_category_data;

    switch (tabIndex) {
    case TEACH:

        //delete teachTree; <-- not needed anymore since the close file does it for us - and because we now have multi file loading
        //teachTree = new TeachingTreeModel(record_manager);
        currentTree = new TeachingTreeModel(record_manager);

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->teachTreeView;

        // analyze the data into a tree

        currentTree->setupModel(yearStart, yearEnd, teachSortOrder, getFilterStartChar(TEACH), getFilterEndChar(TEACH),mapOfSelection,map_filtered_data);
        teachYears.push_back(yearStart);
        teachYears.push_back(yearEnd);


        ui->teach_pie_button->toggle(); ui->teach_pie_button->click();  // ui->teach_bar_button->setChecked(false);  ui->teach_line_button->setChecked(false); ui->teach_pie_button->setChecked(true);

        break;

    case PUBLICATIONS:
        //delete pubTree;
        //pubTree = new PublicationTreeModel(record_manager);
        currentTree = new PublicationTreeModel(record_manager);

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->pubTreeView;

        // analyze the data into a tree

        currentTree->setupModel(yearStart, yearEnd, pubSortOrder, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS), mapOfSelection, map_filtered_data);


        ui->pub_pie_button->toggle();  ui->pub_pie_button->click(); // ui->pub_bar_button->setChecked(false);  ui->pub_line_button->setChecked(false); ui->pub_pie_button->setChecked(true);

        break;
    case PRESENTATIONS:
        //delete presTree;

        //presTree = new PresentationTreeModel(record_manager);
        currentTree = new PresentationTreeModel(record_manager);

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->presTreeView;

        // analyze the data into a tree

        currentTree->setupModel(yearStart, yearEnd, presSortOrder, getFilterStartChar(PRESENTATIONS), getFilterEndChar(PRESENTATIONS), mapOfSelection, map_filtered_data);


        ui->pres_pie_button->toggle();  ui->pres_pie_button->click(); // ui->pres_bar_button->setChecked(false);  ui->pres_line_button->setChecked(false); ui->pres_pie_button->setChecked(true);

        break;

    case FUNDING:
        //delete fundTree;
        //fundTree = new GrantFundingTreeModel(record_manager);
        currentTree = new GrantFundingTreeModel(record_manager);

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->fundTreeView;

        // analyze the data into a tree

        currentTree->setupModel(yearStart, yearEnd, fundSortOrder, getFilterStartChar(FUNDING), getFilterEndChar(FUNDING), mapOfSelection, map_filtered_data);


        ui->fund_pie_button->toggle(); ui->fund_pie_button->click(); // ui->fund_bar_button->setChecked(false);  ui->fund_line_button->setChecked(false); ui->fund_pie_button->setChecked(true);

        break;
    }

    // set up the tree in our UI
    currentView->setModel(currentTree);

    // set resize property to stretch
    currentView->header()->resizeSections(QHeaderView::Stretch);
}

void MainWindow::setupPieChart(PieChartWidget* pieChart, QListWidget *pieListWidget, std::vector<std::pair <std::string, double>> pieChartList) {
    // draws the pie graph by sending piechartwidget a vector of name, presentation count
    int pieSize = (int) pieChartList.size();
    QVector<QColor> colorList(pieSize);
    pieListWidget->clear();
    for (int i = 0; i < pieSize; i++) {
        colorList[i] = (QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        pieListWidget->addItem(QString::fromStdString(pieChartList[i].first));

        // set legend colors
        QPixmap pixmap(100, 100);
        pixmap.fill(QColor(colorList[i]));
        QIcon tempIcon(pixmap);
        pieListWidget->item(i)->setIcon(tempIcon);
    }

    pieChart->setData(pieChartList, colorList); //passes vector list to piechartwidget
}

void MainWindow::setupBarChart(QCustomPlot *barChart, std::vector<std::pair <std::string, double>> barChartList) {
    // create empty bar chart objects:
    QCPBars *yLabels = new QCPBars(barChart->yAxis, barChart->xAxis);
    barChart->addPlottable(yLabels);

    // set names and colors:
    QPen pen;
    pen.setWidthF(1.2);
    yLabels->setName("Type");
    pen.setColor(QColor(255, 131, 0));
    yLabels->setPen(pen);
    yLabels->setBrush(QColor(255, 131, 0, 50));

    //get label list
    int barSize = (int) barChartList.size();
    double maxCount = 0;
    double scaledCount;
    QVector<double> ticks;
    QVector<QString> ylabels;
    QVector<double> count;

    //add label list to y axis labels
    for (int i = 0; i < barSize; i++){
        ticks << (i+1);
        ylabels << QString::fromStdString(barChartList[i].first);
        if (barChartList[i].second>1000000){
            scaledCount = barChartList[i].second/1000000;
        } else if (barChartList[i].second>1000){
            scaledCount = barChartList[i].second/1000;
        } else{
            scaledCount = barChartList[i].second;
        }
        count <<scaledCount;

        if (maxCount < barChartList[i].second)
            maxCount = barChartList[i].second;
    }

    //setup Y Axis
    barChart->yAxis->setAutoTicks(false);
    barChart->yAxis->setAutoTickLabels(false);
    barChart->yAxis->setTickVector(ticks);
    barChart->yAxis->setTickVectorLabels(ylabels);
    barChart->yAxis->setTickLabelPadding(1);
    barChart->yAxis->setSubTickCount(0);
    barChart->yAxis->setTickLength(0, 1);
    barChart->yAxis->grid()->setVisible(true);
    barChart->yAxis->setRange(0, barSize+1);

    if(maxCount>1000000){
        maxCount = maxCount/1000000;
        barChart->xAxis->setLabel("Total (in Millions)");
    }else if (maxCount>1000){
        maxCount = maxCount/1000;
        barChart->xAxis->setLabel("Total (in Thousands)");
    }else{
        barChart->xAxis->setLabel("Total");
    }

    // setup X Axis
    barChart->xAxis->setAutoTicks(true);
    barChart->xAxis->setRange(0,maxCount+(maxCount*.05));
    barChart->xAxis->setAutoTickLabels(true);
    barChart->xAxis->setAutoTickStep(true);
    barChart->xAxis->grid()->setSubGridVisible(true);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    barChart->xAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    barChart->xAxis->grid()->setSubGridPen(gridPen);

    yLabels->setData(ticks, count);
}

void MainWindow::setupLineChart(QCustomPlot *lineChart, std::vector<std::pair<std::string, double> > lineChartList) {
    //create empty line chart objects
    QCPGraph *xLabels = new QCPGraph(lineChart->xAxis, lineChart->yAxis);
    lineChart->addPlottable(xLabels);

    //set names and colors
    QPen pen;
    pen.setWidthF(1.2);
    xLabels->setName("Type");
    pen.setColor(QColor(255,131,0,50));
    xLabels->setPen(pen);
    xLabels->setBrush((QColor(255,131,0,50)));

    //get label list
    int lineSize = (int) lineChartList.size();
    double maxCount = 0;
    double scaledCount;
    QVector<double> ticks;
    QVector<QString> xlabels;
    QVector<double> count;



    //add label list to x axis labels
    for (int i = 0; i < lineSize; i++){
        ticks << (i+1);
        xlabels << QString::fromStdString(lineChartList[i].first);
        if (lineChartList[i].second>1000000){
            scaledCount = lineChartList[i].second/1000000;
        } else if (lineChartList[i].second>1000){
            scaledCount = lineChartList[i].second/1000;
        } else {
            scaledCount = lineChartList[i].second;
        }
        count << scaledCount;

        if (maxCount < lineChartList[i].second)  maxCount = lineChartList[i].second;
    }

    //add label to y axis
    if (maxCount>1000000){
        maxCount = maxCount/1000000;
        lineChart->yAxis->setLabel("Total (in Millions)");
    }else if (maxCount>1000){
        maxCount = maxCount/1000;
        lineChart->yAxis->setLabel("Total (in Thousands)");
    }else {
        lineChart->yAxis->setLabel("Total");
    }

        //setup X Axis
        lineChart->xAxis->setAutoTicks(false);
        lineChart->xAxis->setAutoTickLabels(false);
        lineChart->xAxis->setTickVector(ticks);
        lineChart->xAxis->setTickVectorLabels(xlabels);
        lineChart->xAxis->setTickLabelRotation(90);
        //lineChart->xAxis->setTickLabelPadding(1);
        lineChart->xAxis->setSubTickCount(0);
        lineChart->xAxis->setTickLength(0,1);
        lineChart->xAxis->setRange(0,lineSize+1);
        lineChart->xAxis->grid()->setVisible(true);


        //setUp Y Axis
        lineChart->yAxis->setAutoTicks(true);
        lineChart->yAxis->setRange(0,maxCount+(maxCount*0.5));
        lineChart->yAxis->setAutoTickLabels(true);
        lineChart->yAxis->setAutoTickStep(true);
        lineChart->yAxis->grid()->setSubGridVisible(true);

        //show data points
        QCPScatterStyle* dotStyle =
                new QCPScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,6));
        xLabels->setScatterStyle(*dotStyle);

        QPen gridPen;
        gridPen.setStyle(Qt::SolidLine);
        gridPen.setColor(QColor(0,0,0,25));
        lineChart->yAxis->grid()->setPen(gridPen);
        gridPen.setStyle(Qt::DotLine);
        lineChart->yAxis->grid()->setSubGridPen(gridPen);

        xLabels->setData(ticks,count);

}

void MainWindow::on_teach_new_sort_clicked() {
    if (record_manager != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(TEACH_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allTeachOrders << newSortOrder;
            ui->teach_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}
void MainWindow::on_pub_new_sort_clicked() {
    if (record_manager != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PUBS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPubOrders << newSortOrder;
            ui->pub_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}
void MainWindow::on_pres_new_sort_clicked() {
    if (record_manager != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PRES_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPresOrders << newSortOrder;
            ui->pres_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}
void MainWindow::on_fund_new_sort_clicked() {
    if (record_manager != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(GRANTS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allFundOrders << newSortOrder;
            ui->fund_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}
void MainWindow::on_teach_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allTeachOrders[index];
        teachSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            teachSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->teach_filter->setText(QString::fromStdString(teachSortOrder[0]));
        refresh(TEACH);
    }
}
void MainWindow::on_pub_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPubOrders[index];
        pubSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            pubSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pub_filter->setText(QString::fromStdString(pubSortOrder[0]));
        refresh(PUBLICATIONS);
    }
}
void MainWindow::on_pres_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPresOrders[index];
        presSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            presSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pres_filter->setText(QString::fromStdString(presSortOrder[0]));
        refresh(PRESENTATIONS);
    }
}
void MainWindow::on_fund_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allFundOrders[index];
        fundSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            fundSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->fund_filter->setText(QString::fromStdString(fundSortOrder[0]));
        refresh(FUNDING);
    }
}
void MainWindow::on_teach_delete_sort_clicked() {
    if (ui->teach_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->teach_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allTeachOrders.removeAt(ui->teach_sort->currentIndex());
            ui->teach_sort->removeItem(ui->teach_sort->currentIndex());

            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}
void MainWindow::on_pub_delete_sort_clicked() {
    if (ui->pub_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pub_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPubOrders.removeAt(ui->pub_sort->currentIndex());
            ui->pub_sort->removeItem(ui->pub_sort->currentIndex());

            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}
void MainWindow::on_pres_delete_sort_clicked() {
    if (ui->pres_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pres_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPresOrders.removeAt(ui->pres_sort->currentIndex());
            ui->pres_sort->removeItem(ui->pres_sort->currentIndex());

            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}
void MainWindow::on_fund_delete_sort_clicked() {
    if (ui->fund_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->fund_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allFundOrders.removeAt(ui->fund_sort->currentIndex());
            ui->fund_sort->removeItem(ui->fund_sort->currentIndex());

            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

// Make sure checkable graphing buttons become unchecked when user clicks on desired graph option (Ryan: since the buttons I made are kind of like my own custom-made versions of "radio buttons"using QPush buttons
void MainWindow::on_teach_bar_button_clicked() {  ui->teach_line_button->setChecked(false);  ui->teach_pie_button->setChecked(false);ui->teach_bar_button->setChecked(true); ui->teach_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_teach_line_button_clicked() { ui->teach_bar_button->setChecked(false);  ui->teach_pie_button->setChecked(false); ui->teach_line_button->setChecked(true); ui->teach_graph_stackedWidget->setCurrentIndex(2); }
void MainWindow::on_teach_pie_button_clicked() { ui->teach_bar_button->setChecked(false);  ui->teach_line_button->setChecked(false); ui->teach_pie_button->setChecked(true); ui->teach_graph_stackedWidget->setCurrentIndex(0);}

void MainWindow::on_pub_bar_button_clicked() {  ui->pub_line_button->setChecked(false);  ui->pub_pie_button->setChecked(false); ui->pub_bar_button->setChecked(true); ui->pub_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_pub_line_button_clicked() { ui->pub_bar_button->setChecked(false);  ui->pub_pie_button->setChecked(false); ui->pub_line_button->setChecked(true); ui->pub_graph_stackedWidget->setCurrentIndex(2); }
void MainWindow::on_pub_pie_button_clicked() { ui->pub_bar_button->setChecked(false);  ui->pub_line_button->setChecked(false); ui->pub_pie_button->setChecked(true); ui->pub_graph_stackedWidget->setCurrentIndex(0);}

void MainWindow::on_pres_bar_button_clicked(){  ui->pres_line_button->setChecked(false);  ui->pres_pie_button->setChecked(false); ui->pres_bar_button->setChecked(true); ui->pres_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_pres_line_button_clicked(){ ui->pres_bar_button->setChecked(false);  ui->pres_pie_button->setChecked(false); ui->pres_line_button->setChecked(true); ui->pres_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_pres_pie_button_clicked(){ ui->pres_bar_button->setChecked(false);  ui->pres_line_button->setChecked(false); ui->pres_pie_button->setChecked(true); ui->pres_graph_stackedWidget->setCurrentIndex(0);}

void MainWindow::on_fund_bar_button_clicked(){  ui->fund_line_button->setChecked(false);  ui->fund_pie_button->setChecked(false); ui->fund_bar_button->setChecked(true); ui->fund_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_fund_line_button_clicked(){ ui->fund_bar_button->setChecked(false);  ui->fund_pie_button->setChecked(false); ui->fund_line_button->setChecked(true); ui->fund_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_fund_pie_button_clicked(){ ui->fund_bar_button->setChecked(false);  ui->fund_line_button->setChecked(false); ui->fund_pie_button->setChecked(true); ui->fund_graph_stackedWidget->setCurrentIndex(0);}


//RYAN: IF ANYONE MAKES ANY CHANGES TO THESE TOGGLE SLOTS BELOW, TAKE NOTE THAT I HAD TO COPY AND PASTE THEM INTO THE ABOVE BUTTON_CLICKED() SLOT ^^^ TO MAKE SURE THE BAR,LINE & PIE BUTTONS WORKS PROPERLY.
void MainWindow::on_teach_line_button_toggled() {ui->teach_graph_stackedWidget->setCurrentIndex(2); } //
void MainWindow::on_teach_bar_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(1); } //
void MainWindow::on_teach_pie_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(0); } //

void MainWindow::on_pub_line_button_toggled() {ui->pub_graph_stackedWidget->setCurrentIndex(2); } //
void MainWindow::on_pub_bar_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(1); } //
void MainWindow::on_pub_pie_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(0); } //

void MainWindow::on_pres_line_button_toggled() {ui->pres_graph_stackedWidget->setCurrentIndex(2); } //
void MainWindow::on_pres_bar_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(1); } //
void MainWindow::on_pres_pie_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(0); } //

void MainWindow::on_fund_line_button_toggled() {ui->fund_graph_stackedWidget->setCurrentIndex(2); } //
void MainWindow::on_fund_bar_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(1); } //
void MainWindow::on_fund_pie_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(0); } //



/* ryan: OLD - haven't confirmed if ready to delete yet:
//RYAN: IF ANYONE MAKES ANY CHANGES TO THESE TOGGLE SLOTS BELOW, TAKE NOTE THAT I HAD TO COPY AND PASTE THEM INTO THE ABOVE BUTTON_CLICKED() SLOT ^^^ TO MAKE SURE THE BAR,LINE & PIE BUTTONS WORKS PROPERLY.
void MainWindow::on_teach_line_button_toggled() {ui->teach_graph_stackedWidget->setCurrentIndex(2); ui->teach_bar_button->setChecked(false);  ui->teach_pie_button->setChecked(false); ui->teach_line_button->setChecked(true);} //
void MainWindow::on_teach_bar_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(1); ui->teach_line_button->setChecked(false);  ui->teach_pie_button->setChecked(false);ui->teach_bar_button->setChecked(true);} //
void MainWindow::on_teach_pie_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(0); ui->teach_bar_button->setChecked(false);  ui->teach_line_button->setChecked(false); ui->teach_pie_button->setChecked(true);} //

void MainWindow::on_pub_line_button_toggled() {ui->pub_graph_stackedWidget->setCurrentIndex(2); ui->pub_bar_button->setChecked(false);  ui->pub_pie_button->setChecked(false); ui->pub_line_button->setChecked(true); } //
void MainWindow::on_pub_bar_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(1);ui->pub_line_button->setChecked(false);  ui->pub_pie_button->setChecked(false); ui->pub_bar_button->setChecked(true); } //
void MainWindow::on_pub_pie_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(0); ui->pub_bar_button->setChecked(false);  ui->pub_line_button->setChecked(false); ui->pub_pie_button->setChecked(true);} //

void MainWindow::on_pres_line_button_toggled() {ui->pres_graph_stackedWidget->setCurrentIndex(2); ui->pres_bar_button->setChecked(false);  ui->pres_pie_button->setChecked(false); ui->pres_line_button->setChecked(true);} //
void MainWindow::on_pres_bar_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(1); ui->pres_line_button->setChecked(false);  ui->pres_pie_button->setChecked(false); ui->pres_bar_button->setChecked(true); } //
void MainWindow::on_pres_pie_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(0); ui->pres_bar_button->setChecked(false);  ui->pres_line_button->setChecked(false); ui->pres_pie_button->setChecked(true);} //

void MainWindow::on_fund_line_button_toggled() {ui->fund_graph_stackedWidget->setCurrentIndex(2); ui->fund_bar_button->setChecked(false);  ui->fund_pie_button->setChecked(false); ui->fund_line_button->setChecked(true);} //
void MainWindow::on_fund_bar_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(1); ui->fund_line_button->setChecked(false);  ui->fund_pie_button->setChecked(false); ui->fund_bar_button->setChecked(true);} //
void MainWindow::on_fund_pie_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(0); ui->fund_bar_button->setChecked(false);  ui->fund_line_button->setChecked(false); ui->fund_pie_button->setChecked(true);} //
*/





//similar to determine_load_file_type in it's structure, but will append the current path to that tabs particular filesListView
void MainWindow::update_tabs_filesListView(int load_type, QString path) {
    //QFileInfo filepath(path); QString filename = filepath.fileName();     //returns just the XXXX.csv filename part as a QString
    switch(load_type){
    case TEACH:
        //Go the the listView of the teach tab and append the current path to it
        ui->filesListWidgetTeach->addItem(path);
        ui->filesListWidgetTeach->setCurrentRow(ui->filesListWidgetTeach->count() - 1);
        break;
    case PUBLICATIONS:
        ui->filesListWidgetPub->addItem(path);
        ui->filesListWidgetPub->setCurrentRow(ui->filesListWidgetPub->count() - 1);
        break;
    case PRESENTATIONS:
        ui->filesListWidgetPres->addItem(path);
        ui->filesListWidgetPres->setCurrentRow(ui->filesListWidgetPres->count() - 1);
        break;
    case FUNDING:
        ui->filesListWidgetFund->addItem(path);
        ui->filesListWidgetFund->setCurrentRow(ui->filesListWidgetFund->count() - 1);
        break;
    }
}

void MainWindow::determine_load_file_type(int load_type, QString path){
    update_tabs_filesListView(load_type, path);

    switch(load_type){
    case TEACH:
        load_teach(path);
        break;
    case PUBLICATIONS:
        load_pub(path);
        break;
    case PRESENTATIONS:
        load_pres(path);
        break;
    case FUNDING:
        load_fund(path);
        break;
    }
}

// When a user clicks one of the items in the corresponding QlistWidget, it will load that particular file
void MainWindow::on_filesListWidgetTeach_itemClicked(QListWidgetItem *item) {
    //Load the path that was just clicked
    current_path = item->text().toStdString();
    load_teach(item->text());
}
void MainWindow::on_filesListWidgetPub_itemClicked(QListWidgetItem *item) {
    //Load the path that was just clicked
    current_path = item->text().toStdString();
    load_pub(item->text());
}
void MainWindow::on_filesListWidgetPres_itemClicked(QListWidgetItem *item) {
    //Load the path that was just clicked
    current_path = item->text().toStdString();
    load_pres(item->text());
}
void MainWindow::on_filesListWidgetFund_itemClicked(QListWidgetItem *item) {
    //Load the path that was just clicked
    current_path = item->text().toStdString();
    load_fund(item->text());
}


void MainWindow::on_teach_load_file_clicked() {
    load_file();
}

bool MainWindow::load_teach(QString path) {
    //select teach tab
    ui->categoryTab->setCurrentIndex(0);
    // enable gui elements
    ui->teach_sort->setEnabled(true);
    ui->teach_delete_sort->setEnabled(true);
    ui->teach_new_sort->setEnabled(true);
    ui->teach_filter_from->setEnabled(true);
    ui->teach_filter_to->setEnabled(true);
    ui->teach_pie_button->setEnabled(true);
    ui->teach_bar_button->setEnabled(true);
    ui->teach_line_button->setEnabled(true);
    ui->teach_to_label->setEnabled(true);
    ui->teach_filter->setEnabled(true);
    ui->teach_filter_label->setEnabled(true);
    ui->teachCloseFileButton->setEnabled(true);

    ui->teach_pinned_filter->setEnabled(true);
    ui->teach_delete_onTheFly->setEnabled(true);
    ui->teach_new_onTheFly->setEnabled(true);

    // added for  the category filter
    //ui->teach_CategorySort->setEnabled(false);
    //ui->teach_delete_categoryFilter->setEnabled(false);

    // load save order
    QSortListIO teachSaveOrder(TEACHORDER_SAVE);
    allTeachOrders = teachSaveOrder.readList();
    ui->teach_sort->clear();
    for (int i = 0; i < allTeachOrders.size(); i++) {
        ui->teach_sort->addItem(allTeachOrders.at(i).at(0));
    }

    // ---------------------------- Load the pin filter ---------------------------------------


    // ----------------------------------------------------------------------------------------

    // create default sort order if none are loaded
    if (ui->teach_sort->currentIndex() < 0) {
        createDefaultSortOrder(TEACH);
        ui->teach_sort->addItem(allTeachOrders[0][0]);
    }

    // create the tree
    teachPath = path;
    makeTree(TEACH);
    QString teachPathName = teachPath;
    teachPathName.replace("/","  >  ");
    ui->teach_file_label->setText(teachPathName);


    return true;
}

void MainWindow::on_pub_load_file_clicked() {
    load_file();
}

bool MainWindow::load_pub(QString path) {
    //select publications tab
    ui->categoryTab->setCurrentIndex(1);
    // enable gui elements
    ui->pub_sort->setEnabled(true);
    ui->pub_delete_sort->setEnabled(true);
    ui->pub_new_sort->setEnabled(true);
    ui->pub_filter_from->setEnabled(true);
    ui->pub_filter_to->setEnabled(true);
    ui->pub_pie_button->setEnabled(true);
    ui->pub_bar_button->setEnabled(true);
    ui->pub_line_button->setEnabled(true);
    ui->pub_to_label->setEnabled(true);
    //ui->pub_sort_label->setEnabled(true);
    ui->pub_filter->setEnabled(true);
    ui->pub_filter_label->setEnabled(true);
    ui->pubCloseFileButton->setEnabled(true);
    ui->webGraphButton->setEnabled(true);

    ui->pub_pinned_filter->setEnabled(true);
    ui->pub_delete_onTheFly->setEnabled(true);
    ui->pub_new_onTheFly->setEnabled(true);

    // load save order
    QSortListIO pubSaveOrder(PUBORDER_SAVE);
    allPubOrders = pubSaveOrder.readList();
    ui->pub_sort->clear();
    for (int i = 0; i < allPubOrders.size(); i++) {
        ui->pub_sort->addItem(allPubOrders.at(i).at(0));
    }

    // create default sort order if none are loaded
    if (ui->pub_sort->currentIndex() < 0) {
        createDefaultSortOrder(PUBLICATIONS);
        ui->pub_sort->addItem(allPubOrders[0][0]);
    }

    // create the tree
    pubPath = path;
    makeTree(PUBLICATIONS);
    QString pubPathName = pubPath;
    pubPathName.replace("/","  >  ");
    ui->pub_file_label->setText(pubPathName);
    pubPathNameWebGraph=pubPathName;

    return true;
}

void MainWindow::on_pres_load_file_clicked() {
    load_file();
}

bool MainWindow::load_pres(QString path) {
    //select presentation tab
    ui->categoryTab->setCurrentIndex(2);
    // enable gui elements
    ui->pres_sort->setEnabled(true);
    ui->pres_delete_sort->setEnabled(true);
    ui->pres_new_sort->setEnabled(true);
    ui->pres_filter_from->setEnabled(true);
    ui->pres_filter_to->setEnabled(true);
    ui->pres_pie_button->setEnabled(true);
    ui->pres_bar_button->setEnabled(true);
    ui->pres_line_button->setEnabled(true);
    ui->pres_to_label->setEnabled(true);
    ui->pres_filter->setEnabled(true);
    ui->pres_filter_label->setEnabled(true);
    ui->presCloseFileButton->setEnabled(true);

    ui->pres_pinned_filter->setEnabled(true);
    ui->pres_delete_onTheFly->setEnabled(true);
    ui->pres_new_onTheFly->setEnabled(true);

    // load save order
    QSortListIO presSaveOrder(PRESORDER_SAVE);
    allPresOrders = presSaveOrder.readList();
    ui->pres_sort->clear();
    for (int i = 0; i < allPresOrders.size(); i++) {
        ui->pres_sort->addItem(allPresOrders.at(i).at(0));
    }

    // create default sort order if none are loaded
    if (ui->pres_sort->currentIndex() < 0) {
        createDefaultSortOrder(PRESENTATIONS);
        ui->pres_sort->addItem(allPresOrders[0][0]);
    }

    // create the tree
    presPath = path;
    makeTree(PRESENTATIONS);
    QString presPathName = presPath;
    presPathName.replace("/","  >  ");
    ui->pres_file_label->setText(presPathName);

    return true;
}

void MainWindow::on_fund_load_file_clicked() {
    load_file();
}

bool MainWindow::load_fund(QString path) {
    //select fund tab
    ui->categoryTab->setCurrentIndex(3);
    // enable gui elements
    ui->fund_sort->setEnabled(true);
    ui->fund_delete_sort->setEnabled(true);
    ui->fund_new_sort->setEnabled(true);
    ui->fund_filter_from->setEnabled(true);
    ui->fund_filter_to->setEnabled(true);
    ui->fund_pie_button->setEnabled(true);
    ui->fund_bar_button->setEnabled(true);
    ui->fund_line_button->setEnabled(true);
    ui->fund_to_label->setEnabled(true);
    ui->fund_filter->setEnabled(true);
    ui->fund_filter_label->setEnabled(true);
    ui->fundCloseFileButton->setEnabled(true);

    ui->fund_pinned_filter->setEnabled(true);
    ui->fund_delete_onTheFly->setEnabled(true);
    ui->fund_new_onTheFly->setEnabled(true);

    // load save order
    QSortListIO fundSaveOrder(FUNDORDER_SAVE);
    allFundOrders = fundSaveOrder.readList();
    ui->fund_sort->clear();
    for (int i = 0; i < allFundOrders.size(); i++) {
        ui->fund_sort->addItem(allFundOrders.at(i).at(0));
    }

    // create default sort order if none are loaded
    if (ui->fund_sort->currentIndex() < 0) {
        createDefaultSortOrder(FUNDING);
        ui->fund_sort->addItem(allFundOrders[0][0]);
    }

    // create the tree
    fundPath = path;
    makeTree(FUNDING);
    QString fundPathName = fundPath;
    fundPathName.replace("/","  >  ");
    ui->fund_file_label->setText(fundPathName);

    return true;
}

void MainWindow::on_FromDate_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate->setMinimumDate(date);
    dateChanged = {false, true, false, false};
    fromDateChanged(date);
    pubSaveFrom = date;
}
void MainWindow::on_ToDate_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_2->setMinimumDate(ui->FromDate->date());
    dateChanged = {false, true, false, false};
    toDateChanged(date);
    pubSaveTo = date;


}
void MainWindow::on_FromDate_2_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_2->setMinimumDate(date);
    dateChanged = {true, false, false, false};
    fromDateChanged(date);
    teachSaveFrom = date;
}
void MainWindow::on_ToDate_2_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_2->setMinimumDate(ui->FromDate_2->date());
    dateChanged = {true, false, false, false};

    toDateChanged(date);
    teachSaveTo = date;
}
void MainWindow::on_FromDate_3_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_3->setMinimumDate(date);
    dateChanged = {false, false, true, false};
    fromDateChanged(date);
    presSaveFrom = date;
}
void MainWindow::on_ToDate_3_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_3->setMinimumDate(ui->FromDate_3->date());
    dateChanged = {false, false, true, false};

    toDateChanged(date);
    presSaveTo = date;
}
void MainWindow::on_FromDate_4_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_4->setMinimumDate(date);
    dateChanged = {false, false, false, true};
    fromDateChanged(date);
    fundSaveFrom = date;
}
void MainWindow::on_ToDate_4_dateChanged(const QDate &date) {
    // update end date spinbox to not fall below that year
    ui->ToDate_4->setMinimumDate(ui->FromDate_4->date());
    dateChanged = {false, false, false, true};

    toDateChanged(date);
    fundSaveTo = date;
}

// A master method to handle the date filtering
void MainWindow::fromDateChanged(const QDate &date) {
    // set the member variable to the new date
    yearStart = date.year();

    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}
void MainWindow::toDateChanged(const QDate &date) {
    // set the member variable to the new date
    yearEnd = date.year();
    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}


void MainWindow::on_categoryTab_currentChanged() {
    if (dateChanged[ui->categoryTab->currentIndex()] == true) {
        refresh(ui->categoryTab->currentIndex());
        dateChanged[ui->categoryTab->currentIndex()] = false;
    }
}

void MainWindow::on_teachTreeView_clicked(const QModelIndex &index) {

    //Currently i just want to see if we can get the list of the elements currently selected

    current_path = ui->filesListWidgetTeach->currentItem()->text().toStdString();
    on_treeView_click(index, teachClickedName, teachSortOrder, ui->teachPieChart, ui->teachBarChart,ui->teachLineChart, ui->teachPieList, ui->teachGraphTitle, ui->teach_graph_stackedWidget, TEACH);
}

void MainWindow::on_pubTreeView_clicked(const QModelIndex &index) {
    current_path = ui->filesListWidgetPub->currentItem()->text().toStdString();
    on_treeView_click(index, pubClickedName, pubSortOrder, ui->pubPieChart, ui->pubBarChart, ui->pubLineChart, ui->pubPieList, ui->pubGraphTitle, ui->pub_graph_stackedWidget, PUBLICATIONS);
}

void MainWindow::on_presTreeView_clicked(const QModelIndex &index) {
    current_path = ui->filesListWidgetPres->currentItem()->text().toStdString();
    on_treeView_click(index, presClickedName, presSortOrder, ui->presPieChart, ui->presBarChart,ui->presLineChart, ui->presPieList, ui->presGraphTitle, ui->pres_graph_stackedWidget, PRESENTATIONS);
}

void MainWindow::on_fundTreeView_clicked(const QModelIndex &index) {
    current_path = ui->filesListWidgetFund->currentItem()->text().toStdString();
    on_treeView_click(index, fundClickedName, fundSortOrder, ui->fundPieChart, ui->fundBarChart, ui->fundLineChart, ui->fundPieList, ui->fundGraphTitle, ui->fund_graph_stackedWidget, FUNDING);
}

void MainWindow::on_treeView_click(const QModelIndex &index, QString type_clicked_name, std::vector<std::string> type_sort_order,
                                   PieChartWidget *pieChart, QCustomPlot *barChart, QCustomPlot *lineChart, QListWidget *pieList, QLabel *graphTitle, QStackedWidget *graph_stackedWidget, int file_type){
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getIndexFileType() == file_type && all_readers[i]->getPath() == current_path){
            //std::string checker = ui->filesListWidgetTeach->currentItem()->text().toStdString();
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==type_clicked_name || index.column()!=0) { return;}

    std::vector<std::string> parentsList;
    QModelIndex current = index;
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=type_sort_order.size()) {
        if (clickedName != type_clicked_name) {
            type_clicked_name = clickedName;
            std::vector<std::string> sortOrder(type_sort_order.begin(), type_sort_order.begin()+parentsList.size()+1);
            std::vector<std::pair <std::string, int>> list;
            std::vector<std::pair <std::string, double>> chartList;
            if(file_type == FUNDING){
                 chartList = record_manager->getTotalsTuple(yearStart, yearEnd, sortOrder, parentsList, "Total Amount", getFilterStartChar(file_type), getFilterEndChar(file_type));
            }
            else{
                list = record_manager->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(file_type), getFilterEndChar(file_type));
                for (int i = 0; i < (int) list.size(); i++) {
                    chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
                }
            }


            if (!chartList.empty()) {
                barChart->clearPlottables();
                setupBarChart(barChart, chartList);
                barChart->replot();

                lineChart->clearPlottables();
                setupLineChart(lineChart, chartList);
                lineChart->replot();

                setupPieChart(pieChart, pieList, chartList);

                if (parentsList.size()>1) {
                    graphTitle->setText("Total " + clickedName + " Grants & Funding by " +
                                                QString::fromStdString(type_sort_order[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
                } else {
                    graphTitle->setText("Total Grants & Funding by " + QString::fromStdString(parentsList[0]));
                }
                graph_stackedWidget->show();
            }
        } else {
            graph_stackedWidget->hide();
            graphTitle->clear();
            type_clicked_name.clear();
        }
    }
}

void MainWindow::on_teachPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted)
    {
        QCPPainter painter;
        painter.begin(printer);

        //painter.scale(15,15);

        if(ui->teach_bar_button->isChecked())
        {
            ui->teachBarChart->render(&painter);
            //painter.scale(0.83, 0.83);
            ui->teachGraphTitle->render(&painter);

        }
        else if(ui->teach_line_button->isChecked())
        {
            ui->teachLineChart->render(&painter);
            //painter.scale(0.83, 0.83);
            ui->teachGraphTitle->render(&painter);
        }
        else
        {
            painter.scale(0.4, 0.4);
            ui->teachChartFrame->render(&painter);

            ui->teachGraphTitle->render(&painter);
        }
    }
}

void MainWindow::on_fundPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->fundChartFrame->render(&painter);
    }
}

void MainWindow::on_presPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->presChartFrame->render(&painter);
    }
}

void MainWindow::on_pubPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);




        ui->pubChartFrame->render(&painter);
    }
}



/////  EXPORTING ///////




void MainWindow::on_teachExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),  QDir::homePath(), tr("PDF (*.pdf)"));

    if (fileName.contains("") != 0)
    {
        QPdfWriter writer(fileName);

        //Setup each PDF page attributes (creator of PDF, pagae orientation)
        writer.setCreator("NVIZION");
        writer.setPageOrientation(QPageLayout::Portrait);
        //Setup QPainter object that paints the widgets on the pages
        QPainter painter;

        painter.begin(&writer);
        painter.scale(15,15);

        if(ui->teach_bar_button->isChecked())
        {
            ui->teachBarChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->teachGraphTitle->render(&painter);

        }
        else if(ui->teach_line_button->isChecked())
        {
            ui->teachLineChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->teachGraphTitle->render(&painter);
        }
        else
        {
            ui->teachPieChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->teachGraphTitle->render(&painter);
            writer.newPage();
            painter.scale(1, 1);
            ui->teachPieList->render(&painter);
        }
    }
}

void MainWindow::on_fundExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(), tr("PDF (*.pdf)"));

    if (fileName.contains("") != 0)
    {
        QPdfWriter writer(fileName);

        //Setup each PDF page attributes (creator of PDF, pagae orientation)
        writer.setCreator("NVIZION");
        writer.setPageOrientation(QPageLayout::Portrait);
        //Setup QPainter object that paints the widgets on the pages
        QPainter painter;

        painter.begin(&writer);
        painter.scale(15,15);

        if(ui->fund_bar_button->isChecked())
        {
            ui->fundBarChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->fundGraphTitle->render(&painter);

        }
        else if(ui->fund_line_button->isChecked())
        {
            ui->fundLineChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->fundGraphTitle->render(&painter);
        }
        else
        {
            ui->fundPieChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->fundGraphTitle->render(&painter);
            writer.newPage();
            painter.scale(1, 1);
            ui->fundPieList->render(&painter);
        }
    }
}

void MainWindow::on_presExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0)
    {
        QPdfWriter writer(fileName);

        //Setup each PDF page attributes (creator of PDF, pagae orientation)
        writer.setCreator("NVIZION");
        writer.setPageOrientation(QPageLayout::Portrait);
        //Setup QPainter object that paints the widgets on the pages
        QPainter painter;

        painter.begin(&writer);
        painter.scale(15,15);

        if(ui->pres_bar_button->isChecked())
        {
            ui->presBarChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->presGraphTitle->render(&painter);

        }
        else if(ui->pres_line_button->isChecked())
        {
            ui->presLineChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->presGraphTitle->render(&painter);
        }
        else
        {
            ui->presPieChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->presGraphTitle->render(&painter);
            writer.newPage();
            painter.scale(1, 1);
            ui->presPieList->render(&painter);
        }
    }
}

void MainWindow::on_pubExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(), tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0)
    {
        QPdfWriter writer(fileName);

        //Setup each PDF page attributes (creator of PDF, pagae orientation)
        writer.setCreator("NVIZION");
        writer.setPageOrientation(QPageLayout::Portrait);
        //Setup QPainter object that paints the widgets on the pages
        QPainter painter;

        painter.begin(&writer);
        painter.scale(15,15);

        if(ui->pub_bar_button->isChecked())
        {
            ui->pubBarChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->pubGraphTitle->render(&painter);

        }
        else if(ui->pub_line_button->isChecked())
        {
            ui->pubLineChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->pubGraphTitle->render(&painter);
        }
        else
        {
            ui->pubPieChart->render(&painter);
            painter.scale(0.83, 0.83);
            ui->pubGraphTitle->render(&painter);
            writer.newPage();
            painter.scale(1, 1);
            ui->pubPieList->render(&painter);
        }
    }
}


char MainWindow::getFilterStartChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_from->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_from->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_from->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_from->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else {
        // otherwise, default is '*'
        return '*';
    }
}

char MainWindow::getFilterEndChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_to->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_to->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_to->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_to->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else if (charInField == '*') {
        // otherwise, is it '*'?
        return charInField;
    } else {
        // otherwise, default is 'Z'
        return 'Z';
    }
}

void MainWindow::on_teach_filter_from_textChanged() { refresh(TEACH);}
void MainWindow::on_teach_filter_to_textChanged() { refresh(TEACH);}
void MainWindow::on_pub_filter_from_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pub_filter_to_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pres_filter_from_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_pres_filter_to_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_fund_filter_from_textChanged() { refresh(FUNDING);}
void MainWindow::on_fund_filter_to_textChanged() { refresh(FUNDING);}



void MainWindow::on_webGraphButton_clicked()
{
//if failed conditions
    if(current_path == ""){
        int filename_index = ui->filesListWidgetPub->selectedItems().count() - 1;
        current_path = ui->filesListWidgetPub->selectedItems().at(filename_index)->text().toStdString();
    }
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            //std::string checker = ui->filesListWidgetTeach->currentItem()->text().toStdString();
            if(all_readers[i]->getCollaborations().size() == 0)
                all_readers[i]->createCollaborations();
            else
                all_readers[i]->popupCollaborations();
            break;
        }
    }


}


//Closing the file currently open with the button
void MainWindow::on_teachCloseFileButton_clicked()
{
    //Delete the item from the list view and update the
    ui->filesListWidgetTeach->takeItem(ui->filesListWidgetTeach->currentRow());

    delete currentTree; //Delete the model from the MVC

    ui->teach_graph_stackedWidget->setVisible(false);
    ui->teachGraphTitle->clear();


    // disable the GUI elements
    ui->teach_sort->setEnabled(false);
    ui->teach_delete_sort->setEnabled(false);
    ui->teach_new_sort->setEnabled(false);
    ui->teach_filter_from->setEnabled(false);
    ui->teach_filter_to->setEnabled(false);
    ui->teach_pie_button->setEnabled(false);
    ui->teach_bar_button->setEnabled(false);
    ui->teach_line_button->setEnabled(false);
    ui->teach_to_label->setEnabled(false);
    ui->teach_filter->setEnabled(false);
    ui->teach_filter_label->setEnabled(false);
    ui->teachCloseFileButton->setEnabled(false);

    ui->FromDate_2->setEnabled(false);
    ui->ToDate_2->setEnabled(false);

    ui->teach_category_filter->setEnabled(false);
    ui->teach_pinned_filter->setEnabled(false);

    ui->teachPrintButton->setEnabled(false);
    ui->teachExportButton->setEnabled(false);

    ui->teach_pinned_filter->setEnabled(false);
    ui->teach_delete_onTheFly->setEnabled(false);
    ui->teach_new_onTheFly->setEnabled(false);

    ui->teach_file_label->setText("Please Select a File");

}
void MainWindow::on_pubCloseFileButton_clicked()
{
    //Delete the item from the list view and update the
    ui->filesListWidgetPub->takeItem(ui->filesListWidgetPub->currentRow());

    delete currentTree; //Delete the model from the MVC

    ui->pub_graph_stackedWidget->setVisible(false);
    ui->pubGraphTitle->clear();


    // disable the GUI elements
    ui->pub_sort->setEnabled(false);
    ui->pub_delete_sort->setEnabled(false);
    ui->pub_new_sort->setEnabled(false);
    ui->pub_filter_from->setEnabled(false);
    ui->pub_filter_to->setEnabled(false);
    ui->pub_pie_button->setEnabled(false);
    ui->pub_bar_button->setEnabled(false);
    ui->pub_line_button->setEnabled(false);
    ui->pub_to_label->setEnabled(false);
    ui->pub_filter->setEnabled(false);
    ui->pub_filter_label->setEnabled(false);
    ui->pubCloseFileButton->setEnabled(false);
    ui->FromDate->setEnabled(false);
    ui->ToDate->setEnabled(false);
    ui->pub_category_filter->setEnabled(false);
    ui->pub_pinned_filter->setEnabled(false);
    ui->pubPrintButton->setEnabled(false);
    ui->pubExportButton->setEnabled(false);

    ui->pub_pinned_filter->setEnabled(false);
    ui->pub_delete_onTheFly->setEnabled(false);
    ui->pub_new_onTheFly->setEnabled(false);


    ui->pub_file_label->setText("Please Select a File");
}
void MainWindow::on_presCloseFileButton_clicked()
{
    //Delete the item from the list view and update the
    ui->filesListWidgetPres->takeItem(ui->filesListWidgetPres->currentRow());

    delete currentTree; //Delete the model from the MVC

    ui->pres_graph_stackedWidget->setVisible(false);
    ui->presGraphTitle->clear();


    // disable the GUI elements
    ui->pres_sort->setEnabled(false);
    ui->pres_delete_sort->setEnabled(false);
    ui->pres_new_sort->setEnabled(false);
    ui->pres_filter_from->setEnabled(false);
    ui->pres_filter_to->setEnabled(false);
    ui->pres_pie_button->setEnabled(false);
    ui->pres_bar_button->setEnabled(false);
    ui->pres_line_button->setEnabled(false);
    ui->pres_to_label->setEnabled(false);
    ui->pres_filter->setEnabled(false);
    ui->pres_filter_label->setEnabled(false);
    ui->presCloseFileButton->setEnabled(false);
    ui->FromDate_3->setEnabled(false);
    ui->ToDate_3->setEnabled(false);
    ui->pres_category_filter->setEnabled(false);
    ui->pres_pinned_filter->setEnabled(false);
    ui->presPrintButton->setEnabled(false);
    ui->presExportButton->setEnabled(false);

    ui->pres_pinned_filter->setEnabled(false);
    ui->pres_delete_onTheFly->setEnabled(false);
    ui->pres_new_onTheFly->setEnabled(false);


    ui->pres_file_label->setText("Please Select a File");
}
void MainWindow::on_fundCloseFileButton_clicked()
{
    //Delete the item from the list view and update the
    ui->filesListWidgetFund->takeItem(ui->filesListWidgetFund->currentRow());

    delete currentTree; //Delete the model from the MVC

    ui->fund_graph_stackedWidget->setVisible(false);
    ui->fundGraphTitle->clear();


    // disable the GUI elements
    ui->fund_sort->setEnabled(false);
    ui->fund_delete_sort->setEnabled(false);
    ui->fund_new_sort->setEnabled(false);
    ui->fund_filter_from->setEnabled(false);
    ui->fund_filter_to->setEnabled(false);
    ui->fund_pie_button->setEnabled(false);
    ui->fund_bar_button->setEnabled(false);
    ui->fund_line_button->setEnabled(false);
    ui->fund_to_label->setEnabled(false);
    ui->fund_filter->setEnabled(false);
    ui->fund_filter_label->setEnabled(false);
    ui->fundCloseFileButton->setEnabled(false);
    ui->FromDate_4->setEnabled(false);
    ui->ToDate_4->setEnabled(false);
    ui->fund_category_filter->setEnabled(false);
    ui->fund_pinned_filter->setEnabled(false);
    ui->fundPrintButton->setEnabled(false);
    ui->fundExportButton->setEnabled(false);

    ui->fund_pinned_filter->setEnabled(false);
    ui->fund_delete_onTheFly->setEnabled(false);
    ui->fund_new_onTheFly->setEnabled(false);


    ui->fund_file_label->setText("Please Select a File");
}


//The following is the functionality for the onTheFlyPinned filter
QStringList MainWindow::updateMapOfSelected(QStringList currentPinFilter, int index)
{
    // erase the current mapOfSelected
    this->mapOfSelection.clear();
    QStringList copyOfFilter = currentPinFilter;
    
    //Do this only if the index being changed to is NOT the first one. ie: default.
    if (index > 0 ) {
        // Handle the selection that  is selected and pass it to record manager
        QModelIndexList selectedIndexes = this->currentView->selectionModel()->selectedIndexes();
        int columnCount = this->currentTree->columnCount();
        
        std::vector<std::string> vectorOfRowsSelected;
        int currentHeaderIndex;
        
        // Determine the header index for the current leftmost column
        //This must be done in 4 cases
        switch(ui->categoryTab->currentIndex()) {
        case 0:
            currentHeaderIndex = record_manager->getHeaderIndex(this->allTeachOrders.at(ui->teach_sort->currentIndex()).at(1).toStdString());
            break;
        case 1:
            currentHeaderIndex = record_manager->getHeaderIndex(this->allPubOrders.at(ui->pub_sort->currentIndex()).at(1).toStdString());
            break;
        case 2:
            currentHeaderIndex = record_manager->getHeaderIndex(this->allPresOrders.at(ui->pres_sort->currentIndex()).at(1).toStdString());
            break;
        case 3:
            currentHeaderIndex = record_manager->getHeaderIndex(this->allFundOrders.at(ui->fund_sort->currentIndex()).at(1).toStdString());
            break;
        }

        //2 if statements.one for
        copyOfFilter.removeAt(0);
        
        //if there ARE elements in the pinned shit...
        if (copyOfFilter.size() != 0) {
            //populate vector of Rows
            for (int i=0; i < copyOfFilter.size(); i++) {
                vectorOfRowsSelected.push_back(copyOfFilter.at(i).toStdString());
            }
        }
        
        if (vectorOfRowsSelected.empty()) {
            for (int i=0; i < selectedIndexes.count(); i = i + columnCount) {
                std::cout << this->currentTree->data(selectedIndexes.at(i),0).toString().toStdString() << std::endl;
                //Add these identifiers to the vector of strings
                vectorOfRowsSelected.push_back(this->currentTree->data(selectedIndexes.at(i),0).toString().toStdString());
                
                //Add the current name identifier of the row into the currentPinFilter
                currentPinFilter << this->currentTree->data(selectedIndexes.at(i),0).toString();
            }
            
        }
        
        mapOfSelection.insert(std::make_pair(currentHeaderIndex, vectorOfRowsSelected));
    }
    
    return currentPinFilter;
}

void MainWindow::on_teach_new_onTheFly_clicked()
{
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }
    
    if (record_manager != NULL) {
        // run the window and prompt the user for a selection
        PinFilter* pinFilterDialog = new PinFilter();
        
        int ret = pinFilterDialog->exec();
        
        if (ret) {
            //Here is the case that occurrs when the user has inputted a file name in the dialog and they have clicked save
            QString pinFilterName = pinFilterDialog->getPinnedFilterName();
            
            //Make a QStringList with all the data for the current selection
            QStringList  currentPinFilter;
            currentPinFilter << pinFilterName; // add the name of the filter as the first element
            
            //Call the updateMapOfSelected
            currentPinFilter = updateMapOfSelected(currentPinFilter,ui->teach_pinned_filter->count());
            
            allTeachPin << currentPinFilter; // add the total data needed for the current pinned filter to the allFundPin this
            ui->teach_pinned_filter->addItem(currentPinFilter.at(0));
        }
        delete pinFilterDialog;
    }
    else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
    
}
void MainWindow::on_teach_delete_onTheFly_clicked()
{
    if (ui->teach_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->teach_pinned_filter->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (prompt.exec()==QMessageBox::Yes) {
            
            // Here we will remove the given pinned filter from the list and update the saveSort
            
            allTeachPin.removeAt(ui->teach_pinned_filter->currentIndex());
            ui->teach_pinned_filter->removeItem(ui->teach_pinned_filter->currentIndex());
        }
    }
    else {
        QMessageBox::critical(this, "", "Cannot delete");
    }
}
void MainWindow::on_teach_pinned_filter_currentIndexChanged(int index)
{
    if(index != -1) {
        QStringList pinFilter = allTeachPin[index];
        
        //Now that teachPinFilter has been repopulated with the selected one, we make the mapOfSelected
        // call updateMapOfSelected()
        pinFilter = this->updateMapOfSelected(pinFilter, index);
        
        refresh(TEACH);
    }
}

void MainWindow::on_pub_new_onTheFly_clicked()
{
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }
    
    if (record_manager != NULL) {
        // run the window and prompt the user for a selection
        PinFilter* pinFilterDialog = new PinFilter();
        
        int ret = pinFilterDialog->exec();
        
        if (ret) {
            //Here is the case that occurrs when the user has inputted a file name in the dialog and they have clicked save
            QString pinFilterName = pinFilterDialog->getPinnedFilterName();
            
            //Make a QStringList with all the data for the current selection
            QStringList  currentPinFilter;
            currentPinFilter << pinFilterName; // add the name of the filter as the first element
            
            //Call the updateMapOfSelected
            currentPinFilter = updateMapOfSelected(currentPinFilter,ui->pub_pinned_filter->count());

            allPubPin << currentPinFilter; // add the total data needed for the current pinned filter to the allFundPin this
            ui->pub_pinned_filter->addItem(currentPinFilter.at(0));
        }
        delete pinFilterDialog;
    }
    else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
    
}
void MainWindow::on_pub_delete_onTheFly_clicked()
{
    if (ui->pub_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pub_pinned_filter->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (prompt.exec()==QMessageBox::Yes) {
            
            // Here we will remove the given pinned filter from the list and update the saveSort
            
            allPubPin.removeAt(ui->pub_pinned_filter->currentIndex());
            ui->pub_pinned_filter->removeItem(ui->pub_pinned_filter->currentIndex());
        }
    }
    else {
        QMessageBox::critical(this, "", "Cannot delete");
    }
}
void MainWindow::on_pub_pinned_filter_currentIndexChanged(int index)
{
    if(index != -1) {
        QStringList pinFilter = allPubPin[index];
        

        //Now that teachPinFilter has been repopulated with the selected one, we make the mapOfSelected
        // call updateMapOfSelected()
        pinFilter = this->updateMapOfSelected(pinFilter, index);


        refresh(PUBLICATIONS);
    }
}

void MainWindow::on_pres_new_onTheFly_clicked()
{
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }
    
    if (record_manager != NULL) {
        // run the window and prompt the user for a selection
        PinFilter* pinFilterDialog = new PinFilter();
        
        int ret = pinFilterDialog->exec();
        
        if (ret) {
            //Here is the case that occurrs when the user has inputted a file name in the dialog and they have clicked save
            QString pinFilterName = pinFilterDialog->getPinnedFilterName();

            //Make a QStringList with all the data for the current selection
            QStringList  currentPinFilter;
            currentPinFilter << pinFilterName; // add the name of the filter as the first element

            //Call the updateMapOfSelected
            currentPinFilter = updateMapOfSelected(currentPinFilter,ui->pub_pinned_filter->count());

            allPresPin << currentPinFilter; // add the total data needed for the current pinned filter to the allFundPin this
            ui->pres_pinned_filter->addItem(currentPinFilter.at(0));
        }
        delete pinFilterDialog;
    }
    else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
    
}
void MainWindow::on_pres_delete_onTheFly_clicked()
{
    if (ui->pres_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pres_pinned_filter->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (prompt.exec()==QMessageBox::Yes) {
            
            // Here we will remove the given pinned filter from the list and update the saveSort
            
            allPresPin.removeAt(ui->pres_pinned_filter->currentIndex());
            ui->pres_pinned_filter->removeItem(ui->pres_pinned_filter->currentIndex());
        }
    }
    else {
        QMessageBox::critical(this, "", "Cannot delete");
    }
}
void MainWindow::on_pres_pinned_filter_currentIndexChanged(int index)
{
    if(index != -1) {
        QStringList pinFilter = allPresPin[index];

        //Now that teachPinFilter has been repopulated with the selected one, we make the mapOfSelected
        // call updateMapOfSelected()
        pinFilter = this->updateMapOfSelected(pinFilter, index);

        refresh(PRESENTATIONS);
    }
}

void MainWindow::on_fund_new_onTheFly_clicked()
{
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            break;
        }
    }
    
    if (record_manager != NULL) {
        // run the window and prompt the user for a selection
        PinFilter* pinFilterDialog = new PinFilter();
        
        int ret = pinFilterDialog->exec();
        
        if (ret) {

            //Here is the case that occurrs when the user has inputted a file name in the dialog and they have clicked save
            QString pinFilterName = pinFilterDialog->getPinnedFilterName();

            //Make a QStringList with all the data for the current selection
            QStringList  currentPinFilter;
            currentPinFilter << pinFilterName; // add the name of the filter as the first element

            //Call the updateMapOfSelected
            currentPinFilter = updateMapOfSelected(currentPinFilter,ui->fund_pinned_filter->count());

            allFundPin << currentPinFilter; // add the total data needed for the current pinned filter to the allFundPin this
            ui->fund_pinned_filter->addItem(currentPinFilter.at(0));
        }
        delete pinFilterDialog;
    }
    else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
    
}
void MainWindow::on_fund_delete_onTheFly_clicked()
{
    if (ui->fund_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->fund_pinned_filter->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (prompt.exec()==QMessageBox::Yes) {
            
            // Here we will remove the given pinned filter from the list and update the saveSort
            
            allFundPin.removeAt(ui->fund_pinned_filter->currentIndex());
            ui->fund_pinned_filter->removeItem(ui->fund_pinned_filter->currentIndex());
        }
    }
    else {
        QMessageBox::critical(this, "", "Cannot delete");
    }
}
void MainWindow::on_fund_pinned_filter_currentIndexChanged(int index)
{
    if(index != -1) {
        QStringList pinFilter = allFundPin[index];
        

        //Now that teachPinFilter has been repopulated with the selected one, we make the mapOfSelected
        // call updateMapOfSelected()
        pinFilter = this->updateMapOfSelected(pinFilter, index);

        refresh(FUNDING);
    }
}

// for the category filter
QStringList MainWindow::updateMapOfCategory(QStringList currentCategoryFilter, int index)
{
    // erase the current mapOfSelected
    this->mapOfCategory.clear();
    QStringList copyOfFilter = currentCategoryFilter;
    
    //Do this only if the index being changed to is NOT the first one. ie: default.
    if (index > 0 ) {
        std::vector<std::string> vectorToMap = {copyOfFilter.at(2).toStdString()};
        
        // Determine the header index for the current leftmost column
        int currentHeaderIndex = record_manager->getHeaderIndex(copyOfFilter.at(0).toStdString());
        
        mapOfSelection.insert(std::make_pair(currentHeaderIndex, vectorToMap));
    }
    
    return copyOfFilter;
}

void MainWindow::on_teach_category_filter_currentIndexChanged(int index)
{
    
}
void MainWindow::on_teach_delete_categoryFilter_clicked()
{
    if (ui->teach_category_filter->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->teach_category_filter->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (prompt.exec()==QMessageBox::Yes) {
            
            // Here we will remove the given pinned filter from the list and update the saveSort
            ui->teach_category_filter->removeItem(ui->teach_category_filter->currentIndex());
        }
    }
    else {
        QMessageBox::critical(this, "", "Cannot delete");
    }
}
void MainWindow::on_teach_CategorySort_clicked()
{
    current_path = ui->filesListWidgetTeach->currentItem()->text().toStdString();
    
    int allReadersIndex = -1;
    for(int i = 0; i < all_readers.size(); i++){
        if(all_readers[i]->getPath() == current_path){
            record_manager = all_readers[i]->getRecordManager();
            allReadersIndex = i;
            break;
        }
    }
    
    if (record_manager != NULL) {
        //run the CategoryFilter class
        CategoryFilter* catFilterDialog = new CategoryFilter();
        
        /*
         // Another set of vectors. These were made for updated 'expanded' csv in the /projectInfromation directory had different headers
         std::vector<std::string> publicationsExpandedHeaderCheckUpdated 	= {"Department","Division","Publication Status","Type","Area","Role","Peer Reviewed?", "Journal Name | Published In | Book Title | etc.", "Author(s)"};
         std::vector<std::string> presentationsExpandedHeaderCheckUpdated	= {"Department","Division", "Type", "Role", "Activity Type", "Geographical Scope", "Host", "Country", "Province", "City", "Hours", "Authorship"};
         std::vector<std::string> grantsExpandedHeaderCheckUpdated			= {"Department","Division", "Funding Type", "Status", "Peer Reviewed?", "Industry Grant?", "Role", "Grant Purpose", "Area", "Principal Investigator", "Co-Investigators", "Administered By", "Funding Source"};
         */
        // ---------------------------------------update the values passed into the Dialog window
        std::vector<std::string> teachExpandedHeaderCheckUpdated = {"Department","Division","Program","Type of Course / Activity", "Course / Activity", "Geographical Scope", "Institution / Organization", "Faculty", "Department", "Division", "Location"};
        std::vector<std::vector<std::string>> insertTheUniqueMembersOfEachThing;
        
        
        // here we will call the thing from the csv file
        
        insertTheUniqueMembersOfEachThing = all_readers[allReadersIndex]->getUniqueMembersForHeaders(teachExpandedHeaderCheckUpdated);
        
        
        catFilterDialog->updateSubCategory_Drop(teachExpandedHeaderCheckUpdated);
        catFilterDialog->updateCategoryEnties_Drop(insertTheUniqueMembersOfEachThing);
        
        
        catFilterDialog->exec();
        
        int ret = catFilterDialog->exec();
        
        if (ret) {
            //Here is the case that occurrs when the user has inputted a file name in the dialog and they have clicked save
            QString catFilterName = catFilterDialog->getCategoryFilterName();
            
            //Make a QStringList with all the data for the current selection
            QStringList  currentCatFilter;
            currentCatFilter << catFilterName; // add the name of the filter as the first element
            
            //Here we will add the left drop down and right drop down values into the currentCatFilter
            
            currentCatFilter << catFilterDialog->getSubCategoryText();
            currentCatFilter << catFilterDialog->getCatEntitiesText();
            
            //Call the updateMapOfCategory
            currentCatFilter = updateMapOfCategory(currentCatFilter,ui->teach_category_filter->count());
            
            allTeachCat << currentCatFilter; // add the total data needed for the current pinned filter to the allFundPin this
            
            ui->teach_category_filter->addItem(currentCatFilter.at(0));
            
        }
        delete catFilterDialog;
    }
    else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}



void MainWindow::on_webGraphButton_pressed()
{
    ui->pub_graph_stackedWidget->setCurrentIndex(3);
    ui->pub_line_button->setChecked(false);
    ui->pub_pie_button->setChecked(false);
    ui->pub_bar_button->setChecked(false);
}
