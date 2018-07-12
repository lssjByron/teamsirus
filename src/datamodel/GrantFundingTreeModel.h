#ifndef GRANTFUNDINGTREEMODEL_H
#define GRANTFUNDINGTREEMODEL_H

#include "datamodel/TreeModel.h"

class GrantFundingTreeModel : public TreeModel
{
public:
    GrantFundingTreeModel(RecordsManager*, QObject *parent = 0);
    ~GrantFundingTreeModel();

    void setupModel(int start, int end, std::vector<std::string> sortFields, char filterStart, char filterEnd, std::map<int, std::vector<std::string>> map_filtered_data, std::map<int, std::vector<std::string>> map_filtered_category_data);

};

#endif // GRANTFUNDINGTREEMODEL_H
