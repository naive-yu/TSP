#include "tspwindow.h"
#include "ant_colony.h"
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
    //w.show();
    ant_colony ant_tsp=ant_colony(48,10,40,1000,1,4,0.2);
    ant_tsp.init();
    ant_tsp.run();
    return a.exec();
}
