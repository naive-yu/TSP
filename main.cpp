#include "tspwindow.h"
#include "ant_colony.h"
#include "genetic.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TSP_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    TSPWindow w;
    w.show();
    genetic gen=genetic(48,100,400,4,0.1);
    gen.init();
    gen.run();
    gen.output();
    return a.exec();
}
