#include <QString>
#include <QtTest/QTest>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../gui/mainwindow.h"
#include "../database/QSortListIO.h"
#include "../database/QSortListIO.h"
#include "../database/TestCSVReader.h"
#include "../database/CSVReader.h"
#include "mainwindow.cpp"
//#include "ui_mainwindow.h"
//#include "TestCSVReader.cpp"
//#include "mainwindow.cpp"
#include "../build-TeamPeach-Desktop_Qt_5_7_0_MSVC2015_64bit-Debug/ui_mainwindow.h"

class UnittestTest : public QObject
{
    Q_OBJECT

public:
    UnittestTest();

private Q_SLOTS:
    void CSV_READER_TEST_01();
    void CSV_READER_TEST_02();
    void CSV_READER_TEST_03();
    void CSV_READER_TEST_04();
    void CSV_READER_TEST_05();
    void CSV_READER_TEST_06();
    void CSV_READER_TEST_07();
    void CSV_READER_TEST_08();
    void mainwindow_test();
    void testCSVReaderGetDataFailed();
//    void testCSVReaderGetDataPasses();
    void testCSVReaderGetHeader();
    void testCSVReaderInvalidFilePasses();
    void testCSVReaderInvalidFileFailed();
    void testQSortListInitiation_01();
    void testQSortListInitiation_02();
    void testQSortListSaveList_01();
    void testQSortListSaveList_02();
    //void testQSortListReadList();

    //testing functions created
    void testValidPresentationHeaders();
    void testValidGrantsHeaders();
    void testValidTeachingHeaders();
    void testValidPublicationHeaders();

};

UnittestTest::UnittestTest(){}

void UnittestTest::CSV_READER_TEST_01()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_01();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_02()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_02();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_03()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_03();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_04()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_04();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_05()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_05();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_06()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_06();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_07()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_07();
    QCOMPARE(true, pass);
}

void UnittestTest::CSV_READER_TEST_08()
{
    TestCSVReader testReader;
    bool pass = testReader.CSV_READER_TEST_08();
    QCOMPARE(true, pass);
}

void UnittestTest::mainwindow_test(){
    //QApplication app();

    //Ui::MainWindow::teach_load_file;
    //win.show();
   // win.checkFile(0, "../../Project Information/Sample Data/Program_Teaching_expanded.csv");
}

//void UnittestTest::testCSVReaderGetDataPasses()
//{
//    bool is_fixed = true;
//    CSVReader reader("../../Project Information/Sample Data/Publications_sample.csv", is_fixed);
//    size_t size = reader.getData().size();
//    size_t expected_size = 510;
//    QCOMPARE(size, expected_size);
//}

void UnittestTest::testCSVReaderGetDataFailed()
{
    bool is_fixed = false;
    CSVReader reader("../../Project Information/Sample Data/Presentations_sample.csv", is_fixed);
    size_t size = reader.getData().size();
    size_t expected_size = 140;
    QCOMPARE(size, expected_size);
}

void UnittestTest::testCSVReaderGetHeader()
{
    CSVReader reader("../../Project Information/Sample Data/Presentations_sample.csv");
    size_t size = reader.getHeaders().size();
    size_t expected_size = 30;
    QCOMPARE(size, expected_size);
}

void UnittestTest::testCSVReaderInvalidFilePasses(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Sprint03.xlsx", is_fixed);
    QVERIFY2(true, "Failure");
}

void UnittestTest::testCSVReaderInvalidFileFailed(){
    bool is_fixed = false;
    try{
        CSVReader reader("../../Project Information/Sample Data/Sprint03.xlsx", is_fixed);
        is_fixed = true;
    }
    catch(...){
        QVERIFY2(is_fixed, "loadCSV crashed before is_fixed is assigned true");
    }
}

void UnittestTest::testQSortListInitiation_01(){
    bool is_fixed = false;
    try{
        QSortListIO Qlist("../../Project Information/Sample Data/Sprint03.xlsx");
        is_fixed = true;
    }
    catch(...){
        QVERIFY2(is_fixed, "Initiating QSortList crashed with bad file");
    }
    QVERIFY2(is_fixed, "Initiating QSortList crashed with bad file");
}

void UnittestTest::testQSortListInitiation_02(){
    bool is_fixed = false;
    try{
        QSortListIO Qlist("../../Project Information/Sample Data/Publications_sample.csv");
        is_fixed = true;
    }
    catch(...){
        QVERIFY2(is_fixed, "Initiating QSortList crashed with good file");
    }
    QVERIFY2(is_fixed, "Initiating QSortList crashed with good file");
}

void UnittestTest::testQSortListSaveList_01(){
    bool is_fixed = false;
    try{
        QStringList defaultOrder;
        defaultOrder << "Default";
        //defaultOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";
        QSortListIO Qlist("../../Project Information/Sample Data/Publications_sample.csv");
        QList<QStringList> testList;
        testList.append(defaultOrder);
        Qlist.saveList(testList);
        is_fixed = true;
    }
    catch(...){
        QVERIFY2(is_fixed, "Calling QSortList savelist crashed with default value passed");
    }
    QVERIFY2(is_fixed, "Calling QSortList savelist crashed with default value passed");
}

void UnittestTest::testQSortListSaveList_02(){
    bool is_fixed = false;
    try{
        QStringList defaultOrder;
        defaultOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";
        QSortListIO Qlist("../../Project Information/Sample Data/Publications_sample.csv");
        QList<QStringList> testList;
        testList.append(defaultOrder);
        Qlist.saveList(testList);
        is_fixed = true;
    }
    catch(...){
        QVERIFY2(is_fixed, "Calling QSortList savelist crashed with default value passed");
    }
    QVERIFY2(is_fixed, "Calling QSortList savelist crashed with default value passed");
}

void UnittestTest::testValidPresentationHeaders(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Presentations_sample.csv", is_fixed);
    int presentation = reader.validHeaders();
    QCOMPARE(presentation, 2); //2 is for presentation
}

void UnittestTest::testValidGrantsHeaders(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/GrantsClinicalFunding_sample.csv", is_fixed);
    int grant = reader.validHeaders();
    QCOMPARE(grant, 3); //2 is for grants
}

void UnittestTest::testValidPublicationHeaders(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Publications_expanded.csv", is_fixed);
    int publication = reader.validHeaders();
    QCOMPARE(publication, 1); //1 is for publication
}

void UnittestTest::testValidTeachingHeaders(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Teaching_sample.csv", is_fixed);
    int teach = reader.validHeaders();
    QCOMPARE(teach, 0); //0 is for teach
}

//-------------------------------------------------//

void UnittestTest::testValidPresentationHeadersExpanded(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Presentations_expanded.csv", is_fixed);
    int presentation = reader.validHeaders();
    QCOMPARE(presentation, 2); //2 is for presentation
}

void UnittestTest::testValidGrantsHeadersExpanded(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Grants_expanded.csv", is_fixed);
    int grant = reader.validHeaders();
    QCOMPARE(grant, 3); //2 is for grants
}

void UnittestTest::testValidPublicationHeadersExpanded(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Publications_expanded.csv", is_fixed);
    int publication = reader.validHeaders();
    QCOMPARE(publication, 1); //1 is for publication
}

void UnittestTest::testValidTeachingHeadersExpanded(){
    bool is_fixed = true;
    CSVReader reader("../../Project Information/Sample Data/Program_Teaching_expanded.csv", is_fixed);
    int teach = reader.validHeaders();
    QCOMPARE(teach, 0); //0 is for teach
}

QTEST_APPLESS_MAIN(UnittestTest)

#include "tst_unittesttest.moc"
