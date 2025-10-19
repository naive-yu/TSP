#include "TSPWindow.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QLocale>
#include <QLoggingCategory>
#include <QTextStream>
#include <QTranslator>

// #ifdef Q_OS_WIN
// #include <iostream>
// #include <windows.h>
// #endif

void customLogHandler(QtMsgType type, const QMessageLogContext &context,
                      const QString &msg) {
  static QFile logFile("tsp.log");
  if (!logFile.isOpen()) {
    logFile.open(QIODevice::Append | QIODevice::Text);
  }
  QTextStream stream(&logFile);
  QString logLevel;
  switch (type) {
  case QtDebugMsg:
    logLevel = "DEBUG";
    break;
  case QtInfoMsg:
    logLevel = "INFO";
    break;
  case QtWarningMsg:
    logLevel = "WARNING";
    break;
  case QtCriticalMsg:
    logLevel = "CRITICAL";
    break;
  case QtFatalMsg:
    logLevel = "FATAL";
    break;
  }
  stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
         << " [" << logLevel << "] " << "[" << context.category << "] " << msg
         << "\n";
  stream.flush();
}

int main(int argc, char *argv[]) {
  // Windows下显示控制台窗口
  // #ifdef Q_OS_WIN
  //   if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
  //     freopen("CONOUT$", "w", stdout);
  //     freopen("CONOUT$", "w", stderr);
  //     std::cout.clear();
  //     std::cerr.clear();
  //   }
  // #endif

  QApplication a(argc, argv);
  // 安装自定义日志处理器
  qInstallMessageHandler(customLogHandler);

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
