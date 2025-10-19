#include "Algorithm.h"
#include <QLoggingCategory>
#include <chrono>
#include <memory>

Q_LOGGING_CATEGORY(AlgorithmLog, "Algorithm")

Algorithm::Algorithm(int city, int max_iter, const std::string &name)
    : city_(city), max_iter_(max_iter), name_(name) {}

void Algorithm::init(const std::vector<std::vector<int>> &pos,
                     const std::vector<std::vector<double>> &dis) {
  qCInfo(AlgorithmLog) << "Initializing algorithm with" << city_ << "cities and" << max_iter_ << "iterations.";

  // 随机化引擎
  rng_ = std::make_unique<std::mt19937>(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  u_ = std::make_unique<std::uniform_int_distribution<int>>(0, std::max(0, city_ - 1));
  ur_ = std::make_unique<std::uniform_real_distribution<double>>(0.0, 1.0);

  // 距离矩阵
  position_ = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance_ = std::make_shared<const std::vector<std::vector<double>>>(dis);

  // 检查 city_和max_iter_
  const int MAX_SAFE_ITER = 20000; // 内存预算
  if (city_ <= 0) {
    qCWarning(AlgorithmLog) << "Algorithm::init: invalid city_ =" << city_ << ", 设置为 1";
    city_ = 29;
  }
  if (max_iter_ <= 0 || max_iter_ > MAX_SAFE_ITER) {
    qCWarning(AlgorithmLog) << "Algorithm::init: suspicious max_iter_ =" << max_iter_
               << "，限制为合理范围 (1," << MAX_SAFE_ITER << ")";
    max_iter_ = std::clamp(max_iter_, 1, MAX_SAFE_ITER);
  }

  qCInfo(AlgorithmLog) << "Algorithm initialized with adjusted parameters: city_ =" << city_ << ", max_iter_ =" << max_iter_;

  best_route_.assign(max_iter_, std::vector<int>(city_, 0));
  avg_aim_.assign(max_iter_, 0.0);
  best_aim_.assign(max_iter_, ALGO_INF);
}

int Algorithm::get_cur_iter() const { return cur_iter_; };

int Algorithm::get_max_iter() const { return max_iter_; };

const std::vector<double> &Algorithm::get_avg_aim() const { return avg_aim_; }

const std::vector<double> &Algorithm::get_best_aim() const { return best_aim_; }

const std::vector<std::vector<int>> &Algorithm::get_route() const {
  return best_route_;
}
