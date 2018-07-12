#include "database/RecordsManager.h"
#include "datamodel/TreeItem.h"
#include "datamodel/TeachingTreeModel.h"

TeachingTreeModel::TeachingTreeModel(RecordsManager* db, QObject *parent):
    TreeModel(db, parent) {
}

TeachingTreeModel::~TeachingTreeModel() {
}


void TeachingTreeModel::setupModel(int yearStart, int yearEnd, std::vector<std::string> sortFields, char filterStart, char filterEnd, std::map<int, std::vector<std::string>> map_filtered_data, std::map<int, std::vector<std::string>> map_filtered_category_data) {
    rootItem = new TreeItem(dataObj->createHeadersListForTeaching(sortFields[0]));
    setupModelData(dataObj->createQStringForTeaching(yearStart, yearEnd, sortFields, filterStart, filterEnd, map_filtered_data, map_filtered_category_data).split(QString("\n")), rootItem);

}

