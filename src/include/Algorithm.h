#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H

#include <memory>
#include <string>
#include <vector>
#include <limits>

inline constexpr double ALGO_INF = std::numeric_limits<double>::max();

class Algorithm {
public:
  Algorithm();
  Algorithm(int city, const std::string &name);
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis);
  virtual void run() = 0;
  const std::vector<double> &get_avg_aim() const;
  const std::vector<double> &get_best_aim() const;
  const std::vector<std::vector<int>> &get_route() const;
  virtual ~Algorithm() = default;

private:
  int city_{};                                                       // 城市数量
  std::string name_;                                                  // 算法名称
  std::shared_ptr<const std::vector<std::vector<int>>> position_;    // 位置矩阵
  std::shared_ptr<const std::vector<std::vector<double>>> distance_; // 距离矩阵
  std::vector<std::vector<int>> best_route_; // 每次迭代最佳路径
  std::vector<double> avg_aim_;              // 每次迭代的平均行程距离
  std::vector<double> best_aim_;             // 每次迭代最短行程距离
};

#endif // TSP_ALGORITHM_H
