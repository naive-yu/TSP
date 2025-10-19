#include "AlgorithmWorker.h"
#include "Ant_colony.h"
#include "Genetic.h"
#include "Particle.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <qmutex.h>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(AlgorithmWorkerLog, "AlgorithmWorker")

AlgorithmWorker::AlgorithmWorker(std::shared_ptr<Config> cfg, QObject *parent)
    : QObject(parent), cfg_(std::move(cfg)), stopRequested_(false),
      paused_(false) {
  qCInfo(AlgorithmWorkerLog) << "AlgorithmWorker created.";
}

AlgorithmWorker::~AlgorithmWorker() {
  qCInfo(AlgorithmWorkerLog) << "AlgorithmWorker destroyed.";
}

void AlgorithmWorker::requestStop() {
  stopRequested_ = true;

  // 如果当前处于暂停状态，唤醒线程以便能够及时退出
  if (paused_) {
    QMutexLocker locker(&pauseMutex_);
    pauseCond_.wakeAll();
    qCInfo(AlgorithmWorkerLog) << "Stop requested while paused. Waking up thread.";
  } else {
    qCInfo(AlgorithmWorkerLog) << "Stop requested.";
  }
}

void AlgorithmWorker::setInterval(int interval) {
  if (interval < 1 || interval > 1000) { // 范围检查
    qCWarning(AlgorithmWorkerLog) << "Invalid interval value. Must be between 1 and 1000 ms.";
    return;
  }
  interval_ = interval;
  qCInfo(AlgorithmWorkerLog) << "Animation rate set to:" << interval_ << "ms";
}

void AlgorithmWorker::pause() {
  paused_ = true;
  qCInfo(AlgorithmWorkerLog) << "AlgorithmWorker paused.";
}

void AlgorithmWorker::resume() {
  if (paused_) {
    paused_ = false;
    pauseCond_.wakeAll();
    qCInfo(AlgorithmWorkerLog) << "AlgorithmWorker resumed.";
  }
}

void AlgorithmWorker::startWork(AlgoType type, int city,
                                const std::vector<std::vector<int>> &pos,
                                const std::vector<std::vector<double>> &dis,
                                bool stepByStep) {
  qCInfo(AlgorithmWorkerLog) << "Starting work with type:" << type << ", city:" << city;
  try {
    qCInfo(AlgorithmWorkerLog) << "AlgorithmWorker running in thread" << QThread::currentThread();
    std::unique_ptr<Algorithm> algo;
    switch (type) {
    case Ant_Type:
      algo = std::make_unique<Ant_colony>(city, cfg_->getAntParams(city));
      break;
    case Genetic_Type:
      algo = std::make_unique<Genetic>(city, cfg_->getGeneticParams(city));
      break;
    case Particle_Type:
      algo = std::make_unique<Particle>(city, cfg_->getParticleParams(city));
      break;
    default:
      throw std::runtime_error("Unknown algorithm type");
    }

    if (!algo) {
      throw std::runtime_error("Failed to create algorithm instance!");
    }

    algo->init(pos, dis);

    if (stepByStep) {
      while (!stopRequested_ && algo->runStep()) {
        if (paused_) {
          QMutexLocker locker(&pauseMutex_);
          pauseCond_.wait(&pauseMutex_);
        }
        auto routes = algo->get_route();
        if (!routes.empty()) {
          emit routeUpdated(routes[algo->get_cur_iter() - 1],
                            algo->get_cur_iter(), algo->get_max_iter());
        }
        emit progressed(algo->get_cur_iter(), algo->get_max_iter());
        QThread::msleep(interval_);
      }
    } else {
      while (algo->runStep()) {
        if (paused_) {
          QMutexLocker locker(&pauseMutex_);
          pauseCond_.wait(&pauseMutex_);
        }
      }
    }
    qCInfo(AlgorithmWorkerLog) << "stopRequested_:" << stopRequested_;
    if (!stopRequested_) {
      auto routes = algo->get_route();
      QString result = algo->output();
      if (!routes.empty()) {
        emit routeUpdated(routes.back(), algo->get_cur_iter(),
                          algo->get_max_iter());
      }
      emit finished(result, algo->get_best_aim(), algo->get_avg_aim());
    }
    qCInfo(AlgorithmWorkerLog) << "Work completed.";
  } catch (const std::exception &e) {
    qCCritical(AlgorithmWorkerLog) << "AlgorithmWorker caught exception:" << e.what();
    emit errorOccured("Error occurred in algorithm.");
  }
}
