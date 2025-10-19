#include "AlgoExecuter.h"
#include <QLoggingCategory>
#include <QThread>

Q_LOGGING_CATEGORY(AlgoExecuterLog, "AlgoExecuter")

AlgoExecuter::AlgoExecuter(std::shared_ptr<Config> config, QObject *parent)
    : QObject(parent), cfg_(std::move(config)) {}

AlgoExecuter::~AlgoExecuter() { stop(); }

void AlgoExecuter::execute(AlgoType type, int city,
                           const std::vector<std::vector<int>> &pos,
                           const std::vector<std::vector<double>> &dis,
                           bool stepByStep) {
  qCInfo(AlgoExecuterLog) << "Executing algorithm of type:" << type << "for city:" << city;

  if (worker_thread_ && worker_thread_->isRunning()) {
    stop();
  }

  // create thread and worker
  worker_thread_ = new QThread(this);
  worker_ = new AlgorithmWorker(cfg_);
  worker_->moveToThread(worker_thread_);

  // forward worker signals
  connect(worker_, &AlgorithmWorker::finished, this, &AlgoExecuter::finished);
  connect(worker_, &AlgorithmWorker::routeUpdated, this,
          &AlgoExecuter::routeUpdated);
  connect(worker_, &AlgorithmWorker::progressed, this,
          &AlgoExecuter::progressed);
  connect(worker_, &AlgorithmWorker::errorOccured, this,
          &AlgoExecuter::errorOccured);

  // 工作线程运行配置
  connect(worker_thread_, &QThread::started, worker_,
          [worker = worker_, type, city, &pos, &dis, stepByStep]() mutable {
            worker->startWork(type, city, pos, dis, stepByStep);
          });

  // Worker 完成后清理线程和 Worker
  connect(worker_, &AlgorithmWorker::finished, this, [this]() { stop(); });

  // 启动线程
  worker_thread_->start();

  qCInfo(AlgoExecuterLog) << "Execution started.";
}

void AlgoExecuter::stop() {
  if (worker_thread_ && worker_thread_->isRunning()) {
    worker_->requestStop();
    worker_thread_->quit();
    worker_thread_->wait(); // 等待线程完全停止
    worker_->deleteLater();
    worker_thread_->deleteLater();
    worker_ = nullptr;
    worker_thread_ = nullptr;
  }
}

void AlgoExecuter::pause() {
  if (worker_thread_ && worker_) {
    worker_->pause();
  }
}

void AlgoExecuter::resume() {
  if (worker_thread_ && worker_) {
    worker_->resume();
  }
}

void AlgoExecuter::setInterval(int interval) {
  if (worker_thread_ && worker_) {
    worker_->setInterval(interval);
  }
}