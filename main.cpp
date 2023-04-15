#include "tspwindow.h"
#include "Helper.h"
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
    //最佳：att48,33523.7085;st70,678.5975
    Helper helper=Helper(70);
    helper.Ant_run();
    //helper.Gen_run();
//    TSPWindow w;
//    w.show();
    return 0;
    return a.exec();
}
