#include "testfileheader.h"
#include "../database/CSVReader.h"

void TestFileHeader::validHeaders(){
    QFETCH(QString, string);
    CSVReader reader(string.toStdString());
    bool flag = reader.validHeaders();
    QVERIFY(flag == true);
}

void TestFileHeader::validHeaders_data(){
    QTest::addColumn<QString>("string");

    QTest::newRow("Grants Expanded") << "../../../../Project Information/Sample Data/Grants_expanded.csv";
    QTest::newRow("Grants Sample") << "../../../../Project Information/Sample Data/GrantsClinicalFunding_sample.csv";
    QTest::newRow("Presentations Expanded") << "../../../../Project Information/Sample Data/Presentations_expanded.csv";
    QTest::newRow("Presentations Sample") << "../../../../Project Information/Sample Data/Presentations_sample.csv";
    QTest::newRow("Program Teaching Expanded") << "../../../../Project Information/Sample Data/Program_Teaching_expanded.csv";
}
