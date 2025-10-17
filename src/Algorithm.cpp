#include "Algorithm.h"

Algorithm::Algorithm() = default;

Algorithm::Algorithm(int city, int max_iter, const std::string &name)
    : city_(city), max_iter_(max_iter), name_(name) {}

void Algorithm::init(const std::vector<std::vector<int>> &pos,
                     const std::vector<std::vector<double>> &dis) {
  // 距离矩阵
  position_ = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance_ = std::make_shared<const std::vector<std::vector<double>>>(dis);
  best_route_.assign(max_iter_, std::vector<int>(city_, 0));
  avg_aim_.assign(max_iter_, 0.0);
  best_aim_.assign(max_iter_, ALGO_INF);
}

const std::vector<double> &Algorithm::get_avg_aim() const { return avg_aim_; }

const std::vector<double> &Algorithm::get_best_aim() const { return best_aim_; }

const std::vector<std::vector<int>> &Algorithm::get_route() const {
  return best_route_;
}
