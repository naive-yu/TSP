#pragma once
#include <QString>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>

inline constexpr double ALGO_INF = std::numeric_limits<double>::max();

class Algorithm {
public:
  Algorithm() = default;
  Algorithm(const Algorithm &) = delete;
  Algorithm &operator=(const Algorithm &) = delete;
  Algorithm(Algorithm &&) = default;
  Algorithm &operator=(Algorithm &&) = default;
  Algorithm(int city, int max_iter, const std::string &name);
  virtual void init(const std::vector<std::vector<int>> &pos,
                    const std::vector<std::vector<double>> &dis);
  virtual void run() = 0;
  virtual bool runStep() = 0;
  virtual QString output() const = 0;
  int get_cur_iter() const;
  int get_max_iter() const;
  const std::vector<double> &get_avg_aim() const;
  const std::vector<double> &get_best_aim() const;
  const std::vector<std::vector<int>> &get_route() const;
  virtual ~Algorithm() = default;

protected:
  std::string name_;                      // 算法名称
  int city_{}, max_iter_{}, cur_iter_{0}; // 城市数量、最大迭代次数、当前迭代数
  std::unique_ptr<std::mt19937> rng_;
  std::unique_ptr<std::uniform_int_distribution<int>> u_;
  std::unique_ptr<std::uniform_real_distribution<double>> ur_;
  std::shared_ptr<const std::vector<std::vector<int>>> position_;    // 位置矩阵
  std::shared_ptr<const std::vector<std::vector<double>>> distance_; // 距离矩阵
  std::vector<std::vector<int>> best_route_; // 每次迭代最佳路径
  std::vector<double> avg_aim_;              // 每次迭代的平均行程距离
  std::vector<double> best_aim_;             // 每次迭代最短行程距离
};
