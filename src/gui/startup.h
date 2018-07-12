#ifndef STARTUP_H
#define STARTUP_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QString>

namespace Ui {
class Startup;
}

class Startup : public QDialog
{
    Q_OBJECT

public:
    explicit Startup(QWidget *parent = 0);
    ~Startup();

    int exitCode=1;
    QString getPathOfSession();


private slots:
    void on_loadSessionButton_clicked();

    void on_newSessionButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::Startup *ui;

    QString pathOfSession = "~New";
};

#endif // STARTUP_H
