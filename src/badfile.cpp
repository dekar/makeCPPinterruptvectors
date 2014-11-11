#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QString>
#include "patterns.cpp"

QString printCPP(QString mcuName,QString list, QString pragma)
{
    QString out;
    out = QString(cppPattern)
        .arg(mcuName)
        .arg(list)
            .arg(pragma);
    return out;
}

QString printH(QString mcuName, QString list, QString prototypes)
{
    QString out;
    out = QString(hPattern)
            .arg(mcuName)
            .arg(list)
            .arg(prototypes);
    return out;
}
