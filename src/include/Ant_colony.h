#ifndef TSP_ANT_COLONY_H
#define TSP_ANT_COLONY_H

#include <QString>
#include <limits>
#include <memory>
#include <vector>

inline constexpr double ANTINF = std::numeric_limits<double>::max();

class Ant_colony {
public:
  Ant_colony(int city, int ants, int max_iter, int Q, double alpha, double beta,
             double rho);
  Ant_colony();
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis);
  void run();
  QString output() const;
  const std::vector<double> &get_avg_aim() const;
  const std::vector<double> &get_best_aim() const;
  const std::vector<std::vector<int>> &get_route() const;
  ~Ant_colony();

private:
  // 城市数量、蚂蚁个数、最大迭代次数、信息素增加强度系数
  int city{}, ants{}, max_iter{}, Q{};
  // 信息启发因子、期望启发因子、信息素挥发因子
  double alpha{}, beta{}, rho{};
  std::shared_ptr<const std::vector<std::vector<int>>> position;    // 位置矩阵
  std::shared_ptr<const std::vector<std::vector<double>>> distance; // 距离矩阵
  std::vector<std::vector<double>> message; // 信息素矩阵
  std::vector<std::vector<int>> best_route; // 每次迭代最佳路径
  std::vector<double> avg_aim;              // 每次迭代的平均行程距离
  std::vector<double> best_aim;             // 每次迭代最短行程距离
};

#endif // TSP_ANT_COLONY_H
