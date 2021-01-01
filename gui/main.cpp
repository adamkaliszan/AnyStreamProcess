#include "mainwindow.h"

#include <QCommandLineParser>
#include <QApplication>

extern char *argv0;

int main(int argc, char *argv[])
{
    argv0  = argv[0];
    //QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Qeue systems");
    QCoreApplication::setApplicationVersion("0.11");

    QCommandLineParser parser;
    parser.setApplicationDescription("Tool for investigating the systems with qeues");
    parser.addHelpOption();
    parser.addVersionOption();

    // A boolean option with multiple names (-r, --read)
    QCommandLineOption interactiveOption(QStringList() << "i" << "interactive",
                                  QCoreApplication::translate("main", "interactive text mode"));

    QCommandLineOption addServerOption(QStringList() << "s" << "server",
                                  QCoreApplication::translate("main", "add server"), QCoreApplication::translate("main", "AS"));

    QCommandLineOption addBufferOption(QStringList() << "b" << "buffer",
                                  QCoreApplication::translate("main", "add buffer"), QCoreApplication::translate("main", "AS"));

    QCommandLineOption addErlangOption(QStringList() << "e" << "erlang",
                                       QCoreApplication::translate("main", "add Erlang traffic source"),
                                       QCoreApplication::translate("main", "AS:at:µ"));

    QCommandLineOption setAminOption(QStringList() << "a" << "amin",
                                       QCoreApplication::translate("main", "set minimum traffic offered to single AS"),
                                       QCoreApplication::translate("main", "ERL"));

    QCommandLineOption setAmaxOption(QStringList() << "z" << "amax",
                                       QCoreApplication::translate("main", "set maximum traffic offered to single AS"),
                                       QCoreApplication::translate("main", "ERL"));

    parser.addOption(interactiveOption);
    parser.addOption(addServerOption);
    parser.addOption(addBufferOption);
    parser.addOption(addErlangOption);
    parser.addOption(setAminOption);
    parser.addOption(setAmaxOption);

    QApplication a(argc, argv);
    parser.process(a);

    bool doReadInTextMode = parser.isSet(interactiveOption);

    MainWindow w;
    w.show();

    if (parser.isSet(setAminOption))
    {
        bool success;
        QString val = parser.value(setAminOption);
        double a = val.toDouble(&success);
        if (success)
            w.setMin_a(a);
        else
            qDebug()<<"wrong mininum traffic offered per single AS: "<<val;
    }

    if (parser.isSet(setAmaxOption))
    {
        bool success;
        QString val = parser.value(setAmaxOption);
        double a = val.toDouble(&success);
        if (success)
            w.setMax_a(a);
        else
            qDebug()<<"wrong maximum traffic offered per single AS: "<<val;
    }

    if (parser.isSet(addServerOption))
    {
        bool success;
        QStringList values = parser.values(addServerOption);
        foreach (QString val, values)
        {
            int V = val.toInt(&success);
            if (success)
                w.addServer(1, V);
            else
                qDebug()<<"wrong server capacity: "<<val;
        }
    }

    if (parser.isSet(addBufferOption))
    {
        bool success;
        QStringList values = parser.values(addBufferOption);
        foreach (QString val, values)
        {
            int V = val.toInt(&success);
            if (success)
                w.addBuffer(1, V);
            else
                qDebug()<<"wrong server capacity: "<<val;
        }
    }

    if (parser.isSet(addErlangOption))
    {
        QStringList values = parser.values(addErlangOption);
        foreach (QString val, values)
        {
            int t = 1;
            int at = 1;
            double mu = 1;

            bool successT = true, successAT = true, successMu = true;
            QStringList subVal = val.split(':');
            if (subVal.length() > 0)
                t = subVal[0].toInt(&successT);

            if (subVal.length() > 1)
                at = subVal[1].toInt(&successAT);

            if (subVal.length() > 2)
                mu = subVal[2].toDouble(&successMu);

            w.addErlangTrClass(t, at, mu);

            if (subVal.length() > 0 && !successT)
                qDebug()<<"wrong number of AS: "<<subVal[0];

            if (subVal.length() > 1 && !successAT)
                qDebug()<<"wrong number of proportion at: "<<subVal[1];

            if (subVal.length() > 2 && !successMu)
                qDebug()<<"wrong number of service intensity µ: "<<subVal[2];

        }
    }
    if (w.fillSystem() == false)
        w.readDataBase();
    w.NewTitle();


    if (doReadInTextMode)
    {
        qDebug()<<"Sorry, not implemented yet";
    }
    w.updateAlgorithmsList();
    int result = a.exec();

    TypesAndSettings::release();

    return result;
}

