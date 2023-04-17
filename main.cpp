#include "tspwindow.h"
#include "Helper.h"
#include "Particle.h"
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
    //最佳：
    //Helper helper=Helper(70);
    //helper.Ant_run();
    //helper.Gen_run();
    TSPWindow w;
    w.show();
//    Particle p=Particle(48,150,400,1,1,0.8,0.1,0.075);
//    p.init();
//    p.run();
//    p.output();
    //Genetic gen=Genetic(48, 100, 250, 2, 0.1);
    //gen.init();
    //gen.run();
    //gen.output();
//    return 0;
    return a.exec();
}
