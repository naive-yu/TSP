#pragma once
#include "AlgoType.h"
#include "Algorithm.h"
#include "Config.h"
#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <memory>
#include <vector>


// Worker runs inside a QThread. All public slots will run in the worker thread.
class AlgorithmWorker : public QObject {
  Q_OBJECT
public:
  explicit AlgorithmWorker(std::shared_ptr<Config> cfg,
                           QObject *parent = nullptr);
  ~AlgorithmWorker() override;

public slots:
  void startWork(AlgoType type, int city,
                 const std::vector<std::vector<int>> &pos,
                 const std::vector<std::vector<double>> &dis, bool stepByStep);
  void requestStop();
  void setInterval(int interval);
  void pause();
  void resume();

signals:
  void finished(const QString &result, const std::vector<double> &best,
                const std::vector<double> &avg);
  void routeUpdated(const std::vector<int> &route, int iter, int max_iter);
  void progressed(int iter, int max_iter);
  void errorOccured(const QString &error);

private:
  int interval_{50};
  std::shared_ptr<Config> cfg_;
  std::atomic<bool> stopRequested_;
  std::atomic<bool> paused_;
  QMutex pauseMutex_;
  QWaitCondition pauseCond_;
};
