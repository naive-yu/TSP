#pragma once
#include "Algorithm.h"
#include "Config.h"
#include <QString>
#include <vector>

class Ant_colony : public Algorithm {
public:
  Ant_colony() = default;
  Ant_colony(int city, int ants, int max_iter, int Q, double alpha, double beta,
             double rho);
  Ant_colony(int city, AntParams &params);
  Ant_colony(int city, AntParams &&params);
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis) override;
  void run() override;
  bool runStep() override;
  QString output() const override;
  ~Ant_colony() override = default;

private:
  // 蚂蚁个数、信息素增加强度系数
  int ants_{}, Q_{};
  // 信息启发因子、期望启发因子、信息素挥发因子
  double alpha_{}, beta_{}, rho_{};
  // 信息素矩阵
  std::vector<std::vector<double>> message;
};
