#include "Algorithm.h"
#include <QDebug>

Algorithm::Algorithm() = default;

Algorithm::Algorithm(int city, int max_iter, const std::string &name)
    : city_(city), max_iter_(max_iter), name_(name) {}

void Algorithm::init(const std::vector<std::vector<int>> &pos,
                     const std::vector<std::vector<double>> &dis) {
  // 距离矩阵
  position_ = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance_ = std::make_shared<const std::vector<std::vector<double>>>(dis);

  // 防护：检查 city_ / max_iter_ 是否合理，避免被未初始化或错位参数
  const int MAX_SAFE_ITER = 20000; // 根据内存预算调整
  if (city_ <= 0) {
    qWarning() << "Algorithm::init: invalid city_ =" << city_ << "，设置为 1";
    city_ = 29;
  }
  if (max_iter_ <= 0 || max_iter_ > MAX_SAFE_ITER) {
    qWarning() << "Algorithm::init: suspicious max_iter_ =" << max_iter_
               << "，限制为合理范围 (1," << MAX_SAFE_ITER << ")";
    max_iter_ = std::clamp(max_iter_, 1, MAX_SAFE_ITER);
  }

  best_route_.assign(max_iter_, std::vector<int>(city_, 0));
  avg_aim_.assign(max_iter_, 0.0);
  best_aim_.assign(max_iter_, ALGO_INF);
}

const std::vector<double> &Algorithm::get_avg_aim() const { return avg_aim_; }

const std::vector<double> &Algorithm::get_best_aim() const { return best_aim_; }

const std::vector<std::vector<int>> &Algorithm::get_route() const {
  return best_route_;
}
