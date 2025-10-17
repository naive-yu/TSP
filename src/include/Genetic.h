#ifndef TSP_GENETIC_H
#define TSP_GENETIC_H

#include <QString>
#include <vector>

inline constexpr double GENINF = std::numeric_limits<double>::max();

class Genetic {
public:
  Genetic();
  explicit Genetic(int city, int individual_num, int max_iter,
                   double mutate_prob);
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis);
  void run();
  QString output() const;
  const std::vector<std::vector<int>> &get_route() const noexcept;
  const std::vector<double> &get_best_aim() const noexcept;
  const std::vector<double> &get_avg_aim() const noexcept;
  ~Genetic();

private:
  int city{}, individual_num{}, max_iter{}; // 城市数量、种群数量、最大迭代次数
  double mutate_prob{};                     // 变异概率
  std::shared_ptr<const std::vector<std::vector<int>>> position;          // 位置矩阵
  std::shared_ptr<const std::vector<std::vector<double>>> distance; // 距离矩阵
  std::vector<std::vector<int>> individuals;                        // 当前种群
  std::vector<std::vector<int>> individuals_t; // 变异/交叉后种群
  std::vector<std::vector<int>> best_route;    // 每次迭代最佳路径
  std::vector<double> avg_aim;                 // 每次迭代的平均行程距离
  std::vector<double> best_aim;                // 每次迭代最短行程距离
  std::vector<int> search(std::vector<int> &individual,
                          std::vector<int> &temp) const;

  double get_fitness(std::vector<int> &individual) const;
  std::vector<double>
  get_fitness(std::vector<std::vector<int>> &individuals) const;
  bool isError(std::vector<int> &individual) const;
  bool isExist(int city, std::vector<int> &individual) const;
  void cross();
  void mutate();
  void select();
  void reverse();
  void clock_opt();
};

#endif // TSP_GENETIC_H
