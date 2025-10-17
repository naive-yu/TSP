#include "Particle.h"
#include "TSPWindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <fstream>

void test() {
  std::ofstream f_out;
  f_out.open(".////.test.csv", std::ios::app);
  double data1, data2, data3;
  int time1, time2, time3;
  clock_t start, end;
  f_out << "bayg29" << std::endl;
  for (int j = 0; j < 50; j++) {
    Ant_colony ant = Ant_colony(29, 120, 500, 10, 0.4, 5, 0.1);
    Particle part = Particle(29, 600, 2000, 1, 0.7, 0.6, 0.05);
    Genetic gen = Genetic(29, 280, 2000, 0.2);

    start = clock();
    ant.run();
    end = clock();
    time1 = (end - start) / CLOCKS_PER_SEC;
    data1 = ant.get_best_aim().back();

    start = clock();
    part.run();
    end = clock();
    time2 = (end - start) / CLOCKS_PER_SEC;
    data2 = part.get_best_aim().back();

    start = clock();
    gen.run();
    end = clock();
    time3 = (end - start) / CLOCKS_PER_SEC;
    data3 = gen.get_best_aim().back();

    f_out << data1 << "," << data2 << "," << data3 << "," << time1 << ","
          << time2 << "," << time3 << std::endl;
  }

  f_out << "att48" << std::endl;
  for (int j = 0; j < 50; j++) {
    Ant_colony ant = Ant_colony(48, 150, 500, 1000, 0.4, 5, 0.1);
    Particle part = Particle(48, 1000, 2000, 1, 0.5, 0.5, 0.05);
    Genetic gen = Genetic(48, 380, 2000, 0.2);

    start = clock();
    ant.run();
    end = clock();
    time1 = (end - start) / CLOCKS_PER_SEC;
    data1 = ant.get_best_aim().back();

    start = clock();
    part.run();
    end = clock();
    time2 = (end - start) / CLOCKS_PER_SEC;
    data2 = part.get_best_aim().back();

    start = clock();
    gen.run();
    end = clock();
    time3 = (end - start) / CLOCKS_PER_SEC;
    data3 = gen.get_best_aim().back();

    f_out << data1 << "," << data2 << "," << data3 << "," << time1 << ","
          << time2 << "," << time3 << std::endl;
  }

  f_out << "st70" << std::endl;
  for (int j = 0; j < 50; j++) {
    Ant_colony ant = Ant_colony(70, 210, 500, 100, 0.4, 6, 0.2);
    Particle part = Particle(70, 1800, 2000, 1, 0.4, 0.5, 0.05);
    Genetic gen = Genetic(70, 480, 2000, 0.15);

    start = clock();
    ant.run();
    end = clock();
    time1 = (end - start) / CLOCKS_PER_SEC;
    data1 = ant.get_best_aim().back();

    start = clock();
    part.run();
    end = clock();
    time2 = (end - start) / CLOCKS_PER_SEC;
    data2 = part.get_best_aim().back();

    start = clock();
    gen.run();
    end = clock();
    time3 = (end - start) / CLOCKS_PER_SEC;
    data3 = gen.get_best_aim().back();

    f_out << data1 << "," << data2 << "," << data3 << "," << time1 << ","
          << time2 << "," << time3 << std::endl;
  }
}

int main(int argc, char *argv[]) {
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
  return a.exec();
}
