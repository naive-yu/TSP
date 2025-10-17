#include "Algorithm.h"

Algorithm::Algorithm() = default;

Algorithm::Algorithm(int city, const std::string &name) : city_(city), name_(name) {}

void Algorithm::init(const std::vector<std::vector<int>> &pos,
                     const std::vector<std::vector<double>> &dis) {
  // 距离矩阵
  position_ = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance_ = std::make_shared<const std::vector<std::vector<double>>>(dis);
}

const std::vector<double> &Algorithm::get_avg_aim() const { return avg_aim_; }

const std::vector<double> &Algorithm::get_best_aim() const { return best_aim_; }

const std::vector<std::vector<int>> &Algorithm::get_route() const {
  return best_route_;
}
