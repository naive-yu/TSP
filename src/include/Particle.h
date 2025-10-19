#pragma once
#include "Algorithm.h"
#include "Config.h"
#include <QString>
#include <vector>

class Particle : public Algorithm {
public:
  Particle() = default;
  Particle(int city, int max_iter, int particle, double max_w, double min_w,
           double c1, double c2);
  Particle(int city, ParticleParams &params);
  Particle(int city, ParticleParams &&params);
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis) override;
  void run() override;
  bool runStep() override;
  QString output() const override;
  ~Particle() override = default;

private:
  int particle_num_{};                    // 城市数、粒子个数
  double max_w_{}, min_w_{};              // 惯性因子限制
  double c1_{}, c2_{};                    // 个体学习因子、社会学习因子
  std::vector<std::vector<int>> velocity_; // 粒子速度
  std::vector<std::vector<int>> particles_best;
  std::vector<std::vector<int>> particles;
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
