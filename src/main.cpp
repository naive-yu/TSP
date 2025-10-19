#include "TSPWindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>


#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
  // Windows下显示控制台窗口
#ifdef Q_OS_WIN
  if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    std::cout.clear();
    std::cerr.clear();
  }
#endif

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
