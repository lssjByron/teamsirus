#ifndef TESTFILEHEADERS_H
#define TESTFILEHEADERS_H
#include <QtTest/QtTest>

class TestFileHeaders: public QObject
{
    Q_OBJECT
private slots:
    void validHeaders();
    void validHeaders_data();
};

#endif // TESTFILEHEADERS_H
