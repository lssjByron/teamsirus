#ifndef CATEGORYFILTER_H
#define CATEGORYFILTER_H

#include <QDialog>

namespace Ui {
class CategoryFilter;
}

class CategoryFilter : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryFilter(QWidget *parent = 0);
    ~CategoryFilter();

    QString getCategoryFilterName();
    QString getSubCategoryText();
    QString getCatEntitiesText();

    void updateSubCategory_Drop(std::vector<std::string> subCatHeaders);
    void updateCategoryEnties_Drop(std::vector<std::vector<std::string>> uniqueMembers);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_subCategory_Drop_currentIndexChanged(int index);

private:
    Ui::CategoryFilter *ui;
    QString categoryFilterName;
    std::vector<std::vector<std::string>> uniqueMembers;
};

#endif // CATEGORYFILTER_H
