#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>
#include <QString>
#include <QList>

#include "gui/collabwebgraph.h"
#include "database/RecordsManager.h"
//class RecordsManager;

class CSVReader {
private:
    //first map string contains author, second map string contains title, second map vector contains people who collaborated on title
    QList<CollabWebGraph::CollabData> allCollaboData; //stores all collaboration data: each CollabData  represents a node ie. a unique author.
    std::string path;
    std::vector<std::string> mandatory_fields;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> all_data;
    int sortHeaderIndex;
    int index_file_type;
    RecordsManager* record_manager;
    static std::vector<std::string> GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;
    enum TABS {
        TEACH, PUBLICATIONS, PRESENTATIONS, FUNDING
    };
    std::vector<std::vector<std::string>*> file_errors;
public:
    CSVReader();
    CSVReader(std::string);
    void loadCSV(std::string);
    void loadCSVFixed(std::string file_name);
    void createCollaborations();//used for publications only
    void popupCollaborations();//used for publications only
    int parseDateString(std::string dateString);
    int validHeaders();
    std::vector<std::string> getHeaders();
    std::vector<std::vector<std::string>> getData();
    std::vector<std::string> getMandatoryFields();
    int getSortHeaderIndex();
    int getIndexFileType();
    QList<CollabWebGraph::CollabData> getCollaborations();
    std::string getPath();
    CSVReader(std::string file, bool is_fixed);
    RecordsManager* getRecordManager();
    void addFileErrors();
    std::vector<std::vector<std::string>*> getFileErrors();
    void setFileErrors(int row, int col, std::string value);
    std::vector<std::vector<std::string>> getUniqueMembersForHeaders(std::vector<std::string>);
};

#endif // CSVREADER_H
