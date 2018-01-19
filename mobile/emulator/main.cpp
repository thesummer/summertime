#include <QCoreApplication>
#include <QCommandLineParser>

#include "packethandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Summertime protcol emulator");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("\nThis program will wait for a connection from the\n\n"
                                     "mobile application and will act as the IoT device.\n\n"
                                     "It will acknowledge heart beat packages and send\n\n"
                                     "measurement packages during 'active' measurements.\n\n");
    parser.addHelpOption();
    parser.addPositionalArgument("ticks per minute:  ", "How many ticks the emulated device should measure", "<ticks per minute>");
    parser.addPositionalArgument("noise level:       ", "The amount of noise added to the tick count (default 0)", "[noise level]");

    QCommandLineOption listeningPortOption(QStringList() << "l" << "listen-port",
                                           "UDP port the emulator listens for incoming packets",
                                           "port", "45568");
    parser.addOption(listeningPortOption);
    parser.process(app);

    const QStringList positionalArguments = parser.positionalArguments();
    quint32 epsilon = 0;
    if (positionalArguments.size() > 1)
    {
        epsilon = positionalArguments.at(1).toInt();
    }

    PacketHandler packetHandler(&app, positionalArguments.at(0).toInt(),
                                epsilon, parser.value(listeningPortOption).toInt());
    packetHandler.connectSocket();

    return app.exec();
}
