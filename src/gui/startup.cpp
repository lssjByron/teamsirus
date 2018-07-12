#include "startup.h"
#include "ui_startup.h"
#include <QDesktopWidget>

Startup::Startup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Startup)
{
    ui->setupUi(this);


//           QSize newSize(582,591);

//           this->setGeometry(
//                  QStyle::alignedRect(
//                      Qt::LeftToRight,
//                      Qt::AlignCenter,
//                      newSize,
//                      qApp->desktop()->availableGeometry()
//                  )
//              );
            //this->setWindowFlags(Qt::Window | Qt:: FramelessWindowHint);
}

Startup::~Startup()
{
    delete ui;
    this->done(-1);
}

QString Startup::getPathOfSession() {
    return pathOfSession;
}

void Startup::on_loadSessionButton_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(),
                                                    tr("Saved Session (*.dat);; All files (*.*)"));
    // If the user selects a .dat file, then we pass on the filepath name to mainwindow so that it can be loaded
    if (filePath != NULL) {
        // Set the path of the session the user has selected
        pathOfSession = filePath;

        //close the window
        this->close();
    }

    else {
        QMessageBox::critical(this, "Invalid File", "Select a valid .dat session file");
    }


}


void Startup::on_newSessionButton_clicked()
{
    pathOfSession = "~New";
    exitCode=2;
    //close the window
    this->close();
}


void Startup::on_pushButton_clicked()
{
    exitCode=3;
    this->close();
}
