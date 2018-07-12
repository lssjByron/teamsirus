#ifndef SESSIONSAVINGIO_H
#define SESSIONSAVINGIO_H

#include <string>

class SessionSavingIO
{
public:
    SessionSavingIO(std::string _filename);

    void write(QStringList data);
    QString read();

private:
    std::string filename;
};

#endif // SESSIONSAVINGIO_H
