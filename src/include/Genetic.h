#pragma once
#include "Algorithm.h"
#include "Config.h"
#include <QString>
#include <vector>

class Genetic : public Algorithm {
public:
  Genetic() = default;
  Genetic(int city, int max_iter, int individual_num, double mutate_prob);
  Genetic(int city, GeneticParams &params);
  Genetic(int city, GeneticParams &&params);
  void init(const std::vector<std::vector<int>> &pos,
            const std::vector<std::vector<double>> &dis) override;
  void run() override;
  bool runStep() override;
  QString output() const override;
  ~Genetic() override = default;

private:
  int individual_num_{};                        // 种群数量
  double mutate_prob_{};                        // 变异概率
  std::vector<std::vector<int>> individuals_;   // 当前种群
  std::vector<std::vector<int>> individuals_t_; // 变异/交叉后种群
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
