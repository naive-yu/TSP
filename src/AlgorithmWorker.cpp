#include "AlgorithmWorker.h"
#include "Ant_colony.h"
#include "Genetic.h"
#include "Particle.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

AlgorithmWorker::AlgorithmWorker(std::shared_ptr<Config> cfg, QObject *parent)
    : QObject(parent), cfg_(std::move(cfg)), stopRequested_(false) {}

AlgorithmWorker::~AlgorithmWorker() = default;

void AlgorithmWorker::setInterval(int interval) { interval_ = interval; }

void AlgorithmWorker::requestStop() { stopRequested_ = true; }

void AlgorithmWorker::startWork(AlgoType type, int city,
                                const std::vector<std::vector<int>> &pos,
                                const std::vector<std::vector<double>> &dis,
                                bool stepByStep) {
  try {
    qDebug() << "AlgorithmWorker running in thread" << QThread::currentThread();
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
      throw std::runtime_error("无法创建算法实例!");
    }

    algo->init(pos, dis);

    if (stepByStep) {
      while (!stopRequested_ && algo->runStep()) {
        auto routes = algo->get_route();
        if (!routes.empty()) {
          emit routeUpdated(routes[algo->get_cur_iter() - 1],
                            algo->get_cur_iter());
        }
        emit progressed(algo->get_cur_iter(), algo->get_max_iter());
        QThread::msleep(interval_);
      }
    } else {
      algo->run();
    }
    qDebug() << "stopRequested_: " << stopRequested_;
    if (!stopRequested_) {
      auto routes = algo->get_route();
      QString result = algo->output();
      if (!routes.empty()) {
        emit routeUpdated(routes.back(), algo->get_cur_iter());
      }
      emit finished(result, algo->get_best_aim(), algo->get_avg_aim());
    }
  } catch (const std::exception &e) {
    qDebug() << "AlgorithmWorker caught exception:" << e.what();
    emit errorOccured("error occured in algorithm.");
  }
}
