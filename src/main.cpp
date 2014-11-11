#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QString>
#include <QDebug>

QTextStream out(stdout);

QString printH(QString mcuName, QString list, QString prototypes);
QString printCPP(QString mcuName, QString list, QString pragma);

class FunctionNote
{
public:
    QString name;
    int zeroVal;
    FunctionNote()
    {
        this->zeroVal = 0;
        this->name.clear();
    }
    FunctionNote(int val)
    {
        this->zeroVal = val;
    }
    FunctionNote(QString name)
    {
        this->name = name;
        this->zeroVal = 0;
    }
};


void addFunctionList(QString nxt, QList<FunctionNote> *functionList)
{
    static int zero = 0;
    QStringList s = nxt.split(" ");
    QString n;
    if(s.size() > 1)
        n = s.at(1);
    else
        return;
    if(n.startsWith("0"))
    {
        zero ++;
        return;
    }
    if(zero)
    {
        functionList->append(FunctionNote(zero));
        zero = 0;
    }
    if(n.endsWith("_Handler"))
        n.chop(8);
    if(n.endsWith("Handler"))
        n.chop(7);
    if(n == "SysTick")
        n="SysTickTimer";

    functionList->append(FunctionNote(n));

}

int main(int argc, char *argv[])
{
  // Проверяю количество аргументов
    QCoreApplication a(argc, argv);
    if(argc < 2)
    {
        out << "ERROR: You maus show me the input file\r\n";
        return 1;
    }
  // Пытаюсь открыть файл
    QString filename = a.arguments().at(1);
    QFile *asmFile = new QFile(filename);
    if(!asmFile->open(QFile::ReadOnly))
    {
        out << "ERROR: Can't open file: "<< filename << "\r\n";
        return 1;
    }
  // Отматываю заголовок
    QString mcuName;
    while(!asmFile->atEnd())
    {
        QString nxt = asmFile->readLine().simplified();
        int p = nxt.indexOf("@file");
        if(p > 1)
        {
            mcuName = nxt.mid(p+5).simplified();
            if(mcuName.startsWith("startup_"))
                mcuName.remove(0,8);
            if (mcuName.endsWith(".s"))
                mcuName.chop(2);
        }
        if(nxt == ".word _estack")
            break;
    }
  // Разбираю список функций
    QList<FunctionNote> functionList;
    while(!asmFile->atEnd())
    {
        QString nxt = asmFile->readLine().simplified();
        if(nxt.size() == 0)
            continue;
        if(nxt.startsWith("/*"))
            continue;
        if(nxt.startsWith(".word "))
        {
            addFunctionList(nxt,&functionList);
        }
        else
            break;
    }
    asmFile->close();
    delete asmFile;

  // Формирую выход
    QString headerList;
    QString headerPrototypes;
    int headerReservedNum = 0;
    QString cppFill;
    QString cppPragma;
    foreach(FunctionNote note,functionList)
    {
        if(note.zeroVal) // Zero
        {
           //header

            QString name = QString("__unused%1[%2]").arg(headerReservedNum++).arg(note.zeroVal);
            QString space = QString(20-name.size(),' ');
            headerList  += QString("            void (*"+ name + ")" + space + "(void);\r\n");
           //cpp
            cppFill += "   {0";
            for(int i=1;i<note.zeroVal;i++)
                cppFill += ", 0";
            cppFill += "},\r\n";
        }
        else
        {
             QString space = QString(20-note.name.size(),' ');
            //header
             headerList  += "            void (*" + note.name + ")" + space + "(void);\r\n";
             headerPrototypes +="        void "+note.name + " " + space + "(void);\r\n";
            //cpp
             cppFill += "   ISR::"+note.name + ",\r\n";
             cppPragma +="#pragma weak "+ note.name + space +" = DefaultHandler \r\n";
        }

    }
  // write output files
    QFile *outFile = new QFile();

    outFile->setFileName(QString("ISR")+mcuName+QString(".cpp"));
    outFile->open(QIODevice::WriteOnly);
    outFile->write(
                printCPP(mcuName,cppFill, cppPragma)
                .toLatin1());
    outFile->close();

    outFile->setFileName(QString("ISR")+mcuName+QString(".h"));
    outFile->open(QIODevice::WriteOnly);
    outFile->write(
                printH(mcuName, headerList,headerPrototypes)
                .toLatin1());
    outFile->close();




    qDebug()<< headerList << headerPrototypes << cppFill << cppPragma;

//    return a.exec();
}
