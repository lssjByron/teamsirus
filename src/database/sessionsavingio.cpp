#include <QDataStream>
#include <QFile>
#include <QStringList>
#include <QStringListIterator>
#include <QTextStream>
#include <QDebug>

#include "sessionsavingio.h"

SessionSavingIO::SessionSavingIO(std::string _filename)
{
    filename = _filename;
}

void SessionSavingIO::write(QStringList data) {
    //open the file lfor writing
    QFile file(filename.c_str());
    file.open(QIODevice::WriteOnly);

    // serialize the data into the file
    QTextStream out(&file);
    QString text;
    QStringListIterator iterator(data);
    while (iterator.hasNext()){
        text = iterator.next();
        out << text << "\n";
    }

    file.close();
}

//QString SessionSavingIO::read() {

//}
