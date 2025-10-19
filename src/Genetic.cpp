#include "Genetic.h"
#include "Algorithm.h"
#include <QLoggingCategory>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>

Q_LOGGING_CATEGORY(GeneticLog, "Genetic")

Genetic::Genetic(int city, int max_iter, int individual_num, double mutate_prob)
    : Algorithm(city, max_iter, "Genetic"), individual_num_(individual_num),
      mutate_prob_(mutate_prob) {}

Genetic::Genetic(int city, GeneticParams &params)
    : Algorithm(city, params.max_iter, "Genetic"),
      individual_num_(params.individual_num), mutate_prob_(params.mutate_prob) {
}

Genetic::Genetic(int city, GeneticParams &&params)
    : Algorithm(city, params.max_iter, "Genetic"),
      individual_num_(params.individual_num), mutate_prob_(params.mutate_prob) {
}

void Genetic::init(const std::vector<std::vector<int>> &pos,
                   const std::vector<std::vector<double>> &dis) {
  qCInfo(GeneticLog) << "Initializing Genetic algorithm with" << city_
                     << "cities," << max_iter_ << "max_iter," << individual_num_
                     << "individual_num," << mutate_prob_ << "mutate_prob.";

  Algorithm::init(pos, dis);
  // 种群初始化
  individuals_.assign(individual_num_, std::vector<int>(city_));
  individuals_t_.assign(individual_num_, std::vector<int>(city_));

  // 随机构建生成初始种群
  std::vector<int> base(city_);
  for (int i = 0; i < city_; i++) {
    base[i] = i;
  }
  for (int i = 0; i < individual_num_; i++) {
    shuffle(base.begin(), base.end(), *rng_);
    individuals_[i] = base;
    individuals_t_[i] = base;
  }

  qCInfo(GeneticLog) << "Genetic algorithm initialized.";
}

void Genetic::run() {
  while (runStep()) {
  };
}

bool Genetic::runStep() {
  // 开始遗传算法
  if (cur_iter_ >= max_iter_) {
    return false;
  }

  // 顺时针优化
  clock_opt();
  // 交叉
  cross();
  // 变异
  mutate();
  // 选择
  select();
  // 进化逆转
  reverse();

  // 找出此代最佳路径
  std::vector<double> fitness = get_fitness(individuals_);
  double fitness_sum = std::accumulate(fitness.begin(), fitness.end(), 0.0);
  avg_aim_[cur_iter_] = static_cast<double>(individual_num_) / fitness_sum;
  double max_fitness = 0;
  int max_idx = 0;
  for (int i = 0; i < individual_num_; i++) {
    if (fitness[i] > max_fitness) {
      max_fitness = fitness[i];
      max_idx = i;
    }
  }
  best_aim_[cur_iter_] = (max_fitness > 0.0) ? 1.0 / max_fitness : ALGO_INF;
  best_route_[cur_iter_] = individuals_[max_idx];
  ++cur_iter_;
  return true;
}

std::vector<int> Genetic::search(std::vector<int> &individual,
                                 std::vector<int> &temp) const {
  int cross_num = static_cast<int>(temp.size());
  std::vector<int> idx(cross_num);
  for (int i = 0; i < cross_num; i++) {
    for (int j = 0; j < city_; j++) {
      if (individual[j] == temp[i]) {
        idx[i] = j;
        break;
      }
    }
  }
  return idx;
}

double Genetic::get_fitness(std::vector<int> &route) const {
  if (!distance_ || route.empty())
    return 0.0;
  double dis = (*distance_)[route[city_ - 1]][route[0]];
  for (int i = 0; i < city_ - 1; ++i) {
    dis += (*distance_)[route[i]][route[i + 1]];
  }
  return (dis > 0.0) ? 1.0 / dis : 0.0;
}

std::vector<double>
Genetic::get_fitness(std::vector<std::vector<int>> &all_individuals) const {
  std::vector<double> fitness;
  fitness.reserve(all_individuals.size());
  for (std::vector<int> &route : all_individuals) {
    fitness.push_back(get_fitness(route));
  }
  return fitness;
}

void Genetic::cross() {
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_int_distribution<int> u(0, std::max(0, city_ - 1));
  for (int i = 0; i < individual_num_ / 2; i++) {
    int pos1 = u(rng); // 该位置后cross_num个城市
    int pos2 = u(rng);
    if (pos2 < pos1) {
      int temp = pos1;
      pos1 = pos2;
      pos2 = temp;
    }
    int cross_num = pos2 - pos1 + 1;
    std::vector<int> temp1(cross_num);
    std::vector<int> temp2(cross_num);
    std::vector<int> index;
    for (int j = 0; j < cross_num; j++) {
      temp1[j] = individuals_t_[i][pos1 + j];
      temp2[j] = individuals_t_[individual_num_ - i - 1][pos1 + j];
    }
    // 记录第二条染色体中对应的下标
    index = search(individuals_t_[individual_num_ - i - 1], temp1);
    if (static_cast<int>(index.size()) != cross_num) {
      std::cout << "交叉异常1! ";
    }
    // 第二个染色体相应位置并执行交换
    for (int k = 0; k < cross_num; k++) {
      individuals_t_[individual_num_ - i - 1][pos1 + k] = temp1[k];
    }
    // 第二个染色体规范化
    for (int k = 0, j = 0; j < cross_num && k < cross_num;) {
      while (k < cross_num && isExist(temp1[k], temp2))
        ++k;
      while (j < cross_num && isExist(temp2[j], temp1))
        ++j;
      if (k < cross_num && j < cross_num) {
        individuals_t_[individual_num_ - i - 1][index[k]] = temp2[j];
        ++k;
        ++j;
      }
    }
    if (isError(individuals_t_[individual_num_ - i - 1])) {
      std::cout << "error!" << std::endl;
    }
    // 对第一个染色体进行处理和规范化
    index = search(individuals_t_[i], temp2);
    if ((int)index.size() != cross_num) {
      std::cout << "交叉异常2! ";
    }
    for (int k = 0; k < cross_num; k++) {
      individuals_t_[i][pos1 + k] = temp2[k];
    }
    // 第一个染色体规范化
    for (int j = 0, k = 0; j < cross_num && k < cross_num;) {
      while (j < cross_num && isExist(temp1[j], temp2))
        ++j;
      while (k < cross_num && isExist(temp2[k], temp1))
        ++k;
      if (j < cross_num && k < cross_num) {
        individuals_t_[i][index[k]] = temp1[j];
        ++k;
        ++j;
      }
    }
    if (isError(individuals_t_[i])) {
      std::cout << "error!" << std::endl;
    }
  }
}

void Genetic::mutate() {
  // 采用基于位置的变异
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<double> u1(0.0, 1.0);
  std::uniform_int_distribution<signed> u2(0, std::max(0, city_ - 1));

  for (int i = 0; i < individual_num_; ++i) {
    if (u1(rng) < mutate_prob_) {
      int idx1 = u2(rng);
      int idx2 = u2(rng);
      if (idx1 != idx2) {
        std::swap(individuals_t_[i][idx2], individuals_t_[i][idx1]);
        // or larger fitness logits
      }
    }
  }
}

void Genetic::select() {
  // 使用锦标赛算法合并新旧种群
  std::vector<double> fit1 = get_fitness(individuals_);
  std::vector<double> fit2 = get_fitness(individuals_t_);
  int new_i = 0;
  std::vector<int> route;
  double fit;
  for (int i = 0; i < individual_num_ / 10; i++) {
    // 10个一组，每组选5个
    for (int j = 0; j < 5; j++) {
      int index = -1;
      double maxi = 0;
      for (int k = j; k < 10; k++) {
        if (fit1[10 * i + k] > maxi) {
          index = k;
          maxi = fit1[10 * i + k];
        }
      }
      route = individuals_[new_i];
      fit = fit1[new_i];
      individuals_[new_i] = individuals_[10 * i + index];
      individuals_[10 * i + index] = route;
      fit1[10 * i + index] = fit;
      ++new_i;
    }
  }
  for (int i = 0; i < individual_num_ / 10; i++) {
    // 10个一组，每组选5个
    std::vector<bool> visit(10, false);
    for (int j = 0; j < 5; j++) {
      int index = -1;
      double maxi = 0;
      for (int k = 0; k < 10; k++) {
        if (!visit[k] && fit2[10 * i + k] > maxi) {
          index = k;
          maxi = fit2[10 * i + k];
        }
      }
      individuals_[new_i] = individuals_t_[10 * i + index];
      visit[index] = true;
      ++new_i;
    }
  }
  for (int i = 0; i < individual_num_; i++) {
    individuals_t_[i] = individuals_[i];
  }
  // 或者使用轮盘赌算法
}

void Genetic::clock_opt() {
  for (int i = 0; i < individual_num_; i++) {
    long long s = 0;
    for (int j = 0; j < city_ - 1; j++) {
      s += (*position_)[individuals_[i][j]][0] *
               (*position_)[individuals_[i][j + 1]][1] -
           (*position_)[individuals_[i][j]][1] *
               (*position_)[individuals_[i][j + 1]][0];
    }
    s += (*position_)[individuals_[i][city_ - 1]][0] *
             (*position_)[individuals_[i][0]][1] -
         (*position_)[individuals_[i][city_ - 1]][1] *
             (*position_)[individuals_[i][0]][0];
    // 顺时针
    if (s > 0) {
      std::reverse(individuals_[i].begin(), individuals_[i].end());
      std::reverse(individuals_t_[i].begin(), individuals_t_[i].end());
    }
  }
}

void Genetic::reverse() {
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_int_distribution<signed> u(0, std::max(0, city_ - 1));

  for (int i = 0; i < individual_num_; ++i) {
    // 选取区间
    std::vector<int> route = individuals_[i];
    int pos1 = u(rng);
    int pos2 = u(rng);
    if (pos2 < pos1) {
      std::swap(pos1, pos2);
    }
    int cnt = (pos2 - pos1 + 1) / 2;
    for (int j = 0; j < cnt; j++) {
      std::swap(route[pos1 + j], route[pos2 - j]);
    }
    if (get_fitness(route) > get_fitness(individuals_[i])) {
      individuals_[i] = route;
      individuals_t_[i] = route;
    }
  }
}

QString Genetic::output() const {
  int idx = 0;
  double best = best_aim_[0];
  for (int i = 1; i < max_iter_; i++) {
    if (best_aim_[i] < best) {
      best = best_aim_[i];
      idx = i;
    }
  }

  QString str = "";
  if (city_ == 29) {
    str = "bayg29";
  } else if (city_ == 48) {
    str = "att48";
  } else if (city_ == 70) {
    str = "st70";
  }
  QString res = QString("( 遗传算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(str)
                    .arg(best_aim_[idx]);
  for (int i = 0; i < city_; i++) {
    res.append(QString("%1, ").arg(best_route_[idx][i] + 1));
  }
  return res;
}

bool Genetic::isExist(int city, std::vector<int> &individual) const {
  return std::find(individual.begin(), individual.end(), city) !=
         individual.end();
}

bool Genetic::isError(std::vector<int> &individual) const {
  std::vector<bool> visit(city_, false);
  for (int city : individual) {
    if (city < 0 || city >= this->city_ || visit[city]) {
      return true;
    }
    visit[city] = true;
  }
  return false;
}
