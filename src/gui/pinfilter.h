#ifndef PINFILTER_H
#define PINFILTER_H

#include <QDialog>

namespace Ui {
class PinFilter;
}

class PinFilter : public QDialog
{
    Q_OBJECT

public:
    explicit PinFilter(QWidget *parent = 0);
    ~PinFilter();

    QString getPinnedFilterName();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PinFilter *ui;
    QString pinnedFilterName;
};

#endif // PINFILTER_H
