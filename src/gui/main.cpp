#include <iostream>
#include <stdio.h>
#include <QApplication>
#include <QTreeView>
#include <QDebug>
#include "startup.h"
#include "gui/mainwindow.h"
#include "gui/startup.h"
#include "database/TestCSVReader.h"
using namespace std;

int main(int argc, char *argv[]) {
    int test = 0;   /* test off */

    qDebug() << "Hello this is a test";
    if (test) {
        TestCSVReader testReader;
        bool pass = testReader.CSV_READER_TEST_01();
        if (pass) cout << "TEST_01 PASSED !\n";
        else cout << "TEST_01 FAILED !\n";
        pass = testReader.CSV_READER_TEST_02();
        if (pass) cout << "TEST_02 PASSED !\n";
        else cout << "TEST_02 FAILED !\n";
        pass = testReader.CSV_READER_TEST_03();
        if (pass) cout << "TEST_03 PASSED !\n";
        else cout << "TEST_03 FAILED !\n";
        pass = testReader.CSV_READER_TEST_04();
        if (pass) cout << "TEST_04 PASSED !\n";
        else cout << "TEST_04 FAILED !\n";
        pass = testReader.CSV_READER_TEST_05();
        if (pass) cout << "TEST_05 PASSED !\n";
        else cout << "TEST_05 FAILED !\n";
        pass = testReader.CSV_READER_TEST_06();
        if (pass) cout << "TEST_06 PASSED !\n";
        else cout << "TEST_06 FAILED !\n";
        pass = testReader.CSV_READER_TEST_07();
        if (pass) cout << "TEST_07 PASSED !\n";
        else cout << "TEST_07 FAILED !\n";
        pass = testReader.CSV_READER_TEST_08();
        if (pass) cout << "TEST_08 PASSED !\n";
        else cout << "TEST_08 FAILED !\n";
        testReader.CSV_READER_TEST_ALL();
    }

    QApplication app(argc, argv);

    //Ryan: Adding in my fonts:
    /*
    int id = QFontDatabase::addApplicationFont(":/Fonts/HelveticaNeueLight.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont lightFont(family);
    QApplication::setFont(lightFont);
    //End of adding fonts.
   */


    //MainWindow w;  //to make frameless main window (don't do this) //.setWindowFlags(Qt::Window | Qt::FramelessWindowHint); //w.setWindowFlags(Qt::Window | Qt:: FramelessWindowHint);

    // I may be wrong, but i think this is where we have to add the functionality for the startup window
    Startup s;
    s.exec();

    // Here we would add some if statements to check what's being passed from the startup above and see if they clicked the new session or load session button
    QString pathFromStartupWindow = s.getPathOfSession(); // This will be the path that comes from the Startup window

    //Sam - i think lines 66 and 67 should be in this following if and else clause
    if (pathFromStartupWindow.toStdString() == "~New") {
        //Do stuff here if the user makes a new session
        //cout << pathFromStartupWindow.toStdString() << endl;      <-- test
    }
    else {
        //Do stuff here if the user loads a new session
        //cout << pathFromStartupWindow.toStdString() << endl;      <-- test
    }

    // Sam -- i think all that has to be done now is that MainWindow
    // has an overloaded constructor or a new method that loads the
    // session with the pathFromStartupWindow variable from above
    // I leave this to you since you know how the sesssion from MainWindow is working.
    MainWindow w;
    if (pathFromStartupWindow != "~New" && s.exitCode==1) {
        w.loadSession(pathFromStartupWindow);
    }

    if(s.exitCode != 3 )
    {
        w.show();
        return app.exec();
    }
}
