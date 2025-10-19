#pragma once
#include "AlgoType.h"
#include "AlgorithmWorker.h"
#include "Config.h"
#include <QThread>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>

class AlgoExecuter : public QObject {
  Q_OBJECT
public:
  explicit AlgoExecuter(std::shared_ptr<Config> config,
                        QObject *parent = nullptr);
  ~AlgoExecuter() override;

  void execute(AlgoType type, int city,
               const std::vector<std::vector<int>> &pos,
               const std::vector<std::vector<double>> &dis, bool stepByStep);
  void stop();
  void pause();
  void resume();
  void setInterval(int interval);

signals:
  void finished(const QString &result, const std::vector<double> &best,
                const std::vector<double> &avg);
  void routeUpdated(const std::vector<int> &route, int iter, int max_iter);
  void progressed(int iter, int max_iter);
  void errorOccured(const QString &error);

private:
  QThread *worker_thread_{nullptr};
  AlgorithmWorker *worker_{nullptr};
  std::shared_ptr<Config> cfg_;
};
