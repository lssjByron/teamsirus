#ifndef PRESENTATIONTREEMODEL_H
#define PRESENTATIONTREEMODEL_H

#include "datamodel/TreeModel.h"

class PresentationTreeModel : public TreeModel
{
public:
    PresentationTreeModel(RecordsManager*, QObject *parent = 0);
    ~PresentationTreeModel();

    void setupModel(int start, int end, std::vector<std::string> sortFields, char filterStart, char filterEnd, std::map<int, std::vector<std::string>> map_filtered_data, std::map<int, std::vector<std::string>> map_filtered_category_data);

};

#endif // PRESENTATIONTREEMODEL_H
