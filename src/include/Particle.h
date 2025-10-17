#ifndef TSP_PARTICLE_H
#define TSP_PARTICLE_H

#include <QString>
#include <memory>
#include <vector>

inline constexpr double PARTINF = std::numeric_limits<double>::max();

class Particle {
public:
  Particle(int city, int particle, int max_iter, double max_w, double min_w,
           double c1, double c2);
  Particle();
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis);
  void run();
  QString output() const;
  const std::vector<double> &get_avg_aim() const noexcept;
  const std::vector<double> &get_best_aim() const noexcept;
  const std::vector<std::vector<int>> &get_route() const noexcept;
  ~Particle();

private:
  int cnt{}, city{}, particle_num{},
      max_iter{};                      // 城市数、粒子个数、最大迭代次数
  double max_w{}, min_w{}, c1{}, c2{}; // 惯性因子、个体学习因子、社会学习因子
  std::shared_ptr<const std::vector<std::vector<int>>> position;    // 位置矩阵
  std::shared_ptr<const std::vector<std::vector<double>>> distance; // 距离矩阵
  std::vector<std::vector<int>> particles_best;
  std::vector<std::vector<int>> particles;
  std::vector<std::vector<int>> best_route; // 每次迭代最佳路径
  std::vector<double> avg_aim;              // 每次迭代的平均行程距离
  std::vector<double> best_aim;             // 每次迭代最短行程距离
  std::vector<int> search(const std::vector<int> &paticle,
                          const std::vector<int> &temp) const;
  double get_length(const std::vector<int> &route) const;
  std::vector<double>
  get_length(const std::vector<std::vector<int>> &total) const;
  int find(const std::vector<int> &paticle, int city) const;
  bool isExist(int city, const std::vector<int> &paticle) const;
  void mutate();
  void cross();
  void reverse();
  void clock_opt();
};

#endif // TSP_PARTICLE_H
