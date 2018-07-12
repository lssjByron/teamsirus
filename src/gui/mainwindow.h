#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "CustomSort.h"
#include "ErrorEditDialog.h"

#include <string>
#include <QtWidgets/QMainWindow>
#include <vector>

#include <QDate>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QPrintDialog>
#include <QPrinter>
#include <QString>
#include <QAbstractItemModel>

class RecordsManager;
class CSVReader;
class TreeModel;
class CustomSort;
class PieChartWidget;
class QListWidget;
class QTreeView;
class QCustomPlot;
class QPrinter;
class QPainter;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*
     * Temporary for QTUnitTesting
     */
    int checkFile(int index, QString filePath);
    int checkFileFixed(int index, QString filePath);
    QString pubPathNameWebGraph;
    void loadSession(QString file);



private slots:
    void load_file();

    void saveSession();
    void saveFilter(int index);
    void on_pres_load_file_clicked();
    void on_pub_load_file_clicked();
    void on_fund_load_file_clicked();
    void on_teach_load_file_clicked();

    void on_teach_new_sort_clicked();
    void on_pub_new_sort_clicked();
    void on_pres_new_sort_clicked();
    void on_fund_new_sort_clicked();

    void on_actionLoad_file_triggered();

    void on_FromDate_dateChanged(const QDate &date);
    void on_ToDate_dateChanged(const QDate &date);
    void on_FromDate_2_dateChanged(const QDate &date);
    void on_ToDate_2_dateChanged(const QDate &date);
    void on_FromDate_3_dateChanged(const QDate &date);
    void on_ToDate_3_dateChanged(const QDate &date);
    void on_FromDate_4_dateChanged(const QDate &date);
    void on_ToDate_4_dateChanged(const QDate &date);



    void fromDateChanged(const QDate &date);
    void toDateChanged(const QDate &date);

    void refresh(int tabIndex);

    void on_teach_sort_currentIndexChanged(int index);
    void on_pub_sort_currentIndexChanged(int index);
    void on_pres_sort_currentIndexChanged(int index);
    void on_fund_sort_currentIndexChanged(int index);

    void on_teach_delete_sort_clicked();
    void on_pub_delete_sort_clicked();
    void on_pres_delete_sort_clicked();
    void on_fund_delete_sort_clicked();

    void on_teach_line_button_toggled();
    void on_teach_pie_button_toggled();
    void on_teach_bar_button_toggled();
    void on_pub_line_button_toggled();
    void on_pub_pie_button_toggled();
    void on_pub_bar_button_toggled();
    void on_pres_line_button_toggled();
    void on_pres_pie_button_toggled();
    void on_pres_bar_button_toggled();
    void on_fund_line_button_toggled();
    void on_fund_pie_button_toggled();
    void on_fund_bar_button_toggled();

    void on_teachTreeView_clicked(const QModelIndex &index);
    void on_pubTreeView_clicked(const QModelIndex &index);
    void on_presTreeView_clicked(const QModelIndex &index);
    void on_fundTreeView_clicked(const QModelIndex &index);
    void on_treeView_click(const QModelIndex &index,
                           QString type_clicked_name,
                           std::vector<std::string> type_sort_order,
                           PieChartWidget *pieChart,
                           QCustomPlot *barChart,
                           QCustomPlot *lineChart,
                           QListWidget *pieList,
                           QLabel *graphTitle,
                           QStackedWidget *graph_stackedWidget,
                           int file_type);

    void on_categoryTab_currentChanged();

    void on_teach_filter_from_textChanged();
    void on_teach_filter_to_textChanged();
    void on_pub_filter_from_textChanged();
    void on_pub_filter_to_textChanged();
    void on_pres_filter_from_textChanged();
    void on_pres_filter_to_textChanged();
    void on_fund_filter_from_textChanged();
    void on_fund_filter_to_textChanged();

    void on_teachPrintButton_clicked();

    void on_fundPrintButton_clicked();

    void on_presPrintButton_clicked();

    void on_pubPrintButton_clicked();

    void on_teachExportButton_clicked();

    void on_fundExportButton_clicked();

    void on_presExportButton_clicked();

    void on_pubExportButton_clicked();

    void on_filesListWidgetTeach_itemClicked(QListWidgetItem *item);
    void on_filesListWidgetPub_itemClicked(QListWidgetItem *item);
    void on_filesListWidgetPres_itemClicked(QListWidgetItem *item);
    void on_filesListWidgetFund_itemClicked(QListWidgetItem *item);



    void on_webGraphButton_clicked();

    void on_teach_bar_button_clicked();

    void on_teach_line_button_clicked();

    void on_teach_pie_button_clicked();

    void on_pub_bar_button_clicked();

    void on_pub_line_button_clicked();

    void on_pub_pie_button_clicked();

    void on_pres_bar_button_clicked();

    void on_pres_line_button_clicked();

    void on_pres_pie_button_clicked();

    void on_fund_bar_button_clicked();

    void on_fund_line_button_clicked();

    void on_fund_pie_button_clicked();

    void on_teachCloseFileButton_clicked();
    void on_pubCloseFileButton_clicked();
    void on_presCloseFileButton_clicked();
    void on_fundCloseFileButton_clicked();

    // --------
    QStringList updateMapOfSelected(QStringList currentPinFilter, int index);

    



    void on_teach_new_onTheFly_clicked();
    void on_teach_pinned_filter_currentIndexChanged(int index);
    void on_teach_delete_onTheFly_clicked();
  
   
    void on_pub_new_onTheFly_clicked();
    void on_pub_delete_onTheFly_clicked();
    void on_pub_pinned_filter_currentIndexChanged(int index);
    
    void on_pres_new_onTheFly_clicked();
    void on_pres_delete_onTheFly_clicked();
    void on_pres_pinned_filter_currentIndexChanged(int index);
    
    void on_fund_new_onTheFly_clicked();
    void on_fund_delete_onTheFly_clicked();
    void on_fund_pinned_filter_currentIndexChanged(int index);

    // ----------
    QStringList updateMapOfCategory(QStringList currentCategoryFilter, int index);

    void on_teach_CategorySort_clicked();
    void on_teach_category_filter_currentIndexChanged(int index);
    void on_teach_delete_categoryFilter_clicked();

    //----
    void on_webGraphButton_pressed();


private:
    std::string current_path;
    static std::vector<std::string> GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;

    enum TABS {
        TEACH, PUBLICATIONS, PRESENTATIONS, FUNDING
    };

    struct field_error;

    Ui::MainWindow* ui;
    QPrinter* printer;
    QStringList data;
    QList<QStringList> allTeachOrders, allPubOrders, allPresOrders, allFundOrders;

    
    QDate teachSaveFrom, teachSaveTo, pubSaveFrom, pubSaveTo, presSaveFrom, presSaveTo, fundSaveFrom,fundSaveTo;


    QList<QStringList> allTeachPin, allPubPin, allPresPin, allFundPin; //for the pinned filter list
    QList<QStringList> allTeachCat, allPubCat, allPresCat, allFundCat; //for the pinned filter list

    std::map<int, std::vector<std::string> > mapOfSelection;
    std::map<int, std::vector<std::string> > mapOfCategory; //int is the left drop down, and the vector will only have the one thing from the right drop down



    QString teachPath, pubPath, presPath, fundPath;
    TreeModel *fundTree, *presTree, *pubTree, *teachTree;
    RecordsManager *record_manager;
    CSVReader *reader;
    std::vector<CSVReader*> current_readers;
    std::vector<CSVReader*> all_readers;
    std::vector<std::vector<std::string>> fundData, presData, pubData, teachData;

    std::vector<std::string> teachSortOrder, pubSortOrder, presSortOrder, fundSortOrder;
    std::vector<std::string> teachPinFilter, pubPinFilter, presPinFilter, fundPinFilter;

    TreeModel* currentTree;
    QTreeView* currentView;
    int currentIndex;
    QString teachClickedName, pubClickedName, presClickedName, fundClickedName;

    QList<bool> dateChanged;

    std::vector<std::string> getParentsList(QModelIndex &index);
    char getFilterStartChar(int type);
    char getFilterEndChar(int type);

    int yearStart, yearEnd;
    std::vector<int> teachYears, pubYears, presYears, fundYears;

    // Temp for QTunitTesting
    //int checkFile(int index, QString filePath);

    //std::vector<std::string> getSelectedSortOrder(int tabIndex);
    void createDefaultSortOrder(int tabIndex);
    void createDefaultPinnedFilter(); // this will be used to initalize the pinned filter vectors

    void makeTree(int tabIndex);

    void setupPieChart(PieChartWidget *pieChart, QListWidget *pieListWidget, std::vector<std::pair<std::string, double> > pieChartList);
    void setupBarChart(QCustomPlot *barChart, std::vector<std::pair<std::string, double> > barChartList);
    void setupLineChart(QCustomPlot *lineChart, std::vector<std::pair<std::string,double>> lineChartList);


    bool handle_field_errors(std::vector<std::vector<std::string>*>& errors);
    void update_tabs_filesListView(int load_type, QString path);
    void determine_load_file_type(int load_type, QString path);
    bool load_fund(QString path);
    bool load_pub(QString path);
    bool load_teach(QString path);
    bool load_pres(QString path);
};

#endif // MAINWINDOW_H
