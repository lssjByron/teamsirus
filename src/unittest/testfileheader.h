#ifndef TESTFILEHEADER_H
#define TESTFILEHEADER_H

#include <QtTest/QtTest>

class TestFileHeader: public QObject
{
    Q_OBJECT
private slots:
    void validHeaders();
    void validHeaders_data();
};
#endif // TESTFILEHEADER_H
