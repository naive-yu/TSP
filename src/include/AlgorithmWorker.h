#pragma once
#include "AlgoType.h"
#include "Algorithm.h"
#include "Config.h"
#include <QObject>
#include <memory>
#include <vector>

// Worker runs inside a QThread. All public slots will run in the worker thread.
class AlgorithmWorker : public QObject {
  Q_OBJECT
public:
  explicit AlgorithmWorker(std::shared_ptr<Config> cfg,
                           QObject *parent = nullptr);
  ~AlgorithmWorker() override;
  void setInterval(int interval);

public slots:
  void startWork(AlgoType type, int city,
                 const std::vector<std::vector<int>> &pos,
                 const std::vector<std::vector<double>> &dis, bool stepByStep);
  void requestStop();

signals:
  void finished(const QString &result, const std::vector<double> &best,
                const std::vector<double> &avg);
  void routeUpdated(const std::vector<int> &route, int iter);
  void progressed(int iter, int max_iter);
  void errorOccured(const QString &error);

private:
  int interval_{50};
  std::shared_ptr<Config> cfg_;
  std::atomic<bool> stopRequested_;
};
