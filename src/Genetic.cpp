#include "Genetic.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <numeric>
#include <qDebug>
#include <random>

Genetic::Genetic() = default;
Genetic::Genetic(int city, int individual_num, int max_iter, double mutate_prob)
    : city(city), individual_num(individual_num), max_iter(max_iter),
      mutate_prob(mutate_prob) {}

void Genetic::init(const std::vector<std::vector<int>> &pos,
                   const std::vector<std::vector<double>> &dis) {
  position = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance = std::make_shared<const std::vector<std::vector<double>>>(dis);

  // 种群初始化
  individuals.assign(individual_num, std::vector<int>(city));
  individuals_t.assign(individual_num, std::vector<int>(city));
  best_route.assign(max_iter, std::vector<int>(city));
  avg_aim.assign(max_iter, 0.0);
  best_aim.assign(max_iter, GENINF);
}

void Genetic::run() {
  // 线程局部随机数引擎
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));

  // 随机构建生成初始种群
  std::vector<int> base(city);
  for (int i = 0; i < city; i++) {
    base[i] = i;
  }
  for (int i = 0; i < individual_num; i++) {
    shuffle(base.begin(), base.end(), rng);
    individuals[i] = base;
    individuals_t[i] = base;
  }

  // 开始遗传算法
  for (int gen = 0; gen < max_iter; gen++) {
    // 时针优化，统一采用顺时针方案
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
    std::vector<double> fitness = get_fitness(individuals);
    double fitness_sum = std::accumulate(fitness.begin(), fitness.end(), 0.0);
    avg_aim[gen] = static_cast<double>(individual_num) / fitness_sum;
    double max_fitness = 0;
    int max_idx = 0;
    for (int i = 0; i < individual_num; i++) {
      if (fitness[i] > max_fitness) {
        max_fitness = fitness[i];
        max_idx = i;
      }
    }
    best_aim[gen] = (max_fitness > 0.0) ? 1.0 / max_fitness : GENINF;
    best_route[gen] = individuals[max_idx];
  }
}

std::vector<int> Genetic::search(std::vector<int> &individual,
                                 std::vector<int> &temp) const {
  int cross_num = static_cast<int>(temp.size());
  std::vector<int> idx(cross_num);
  for (int i = 0; i < cross_num; i++) {
    for (int j = 0; j < city; j++) {
      if (individual[j] == temp[i]) {
        idx[i] = j;
        break;
      }
    }
  }
  return idx;
}

double Genetic::get_fitness(std::vector<int> &route) const {
  if (!distance || route.empty())
    return 0.0;
  double dis = (*distance)[route[city - 1]][route[0]];
  for (int i = 0; i < city - 1; ++i) {
    dis += (*distance)[route[i]][route[i + 1]];
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
  std::uniform_int_distribution<int> u(0, std::max(0, city - 1));
  for (int i = 0; i < individual_num / 2; i++) {
    int pos1 = u(rng); // 该位置后cross_num个城市
    int pos2 = u(rng);
    // std::cout<<pos1<<","<<pos2<<endl;
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
      temp1[j] = individuals_t[i][pos1 + j];
      temp2[j] = individuals_t[individual_num - i - 1][pos1 + j];
    }
    // 记录第二条染色体中对应的下标
    index = search(individuals_t[individual_num - i - 1], temp1);
    if (static_cast<int>(index.size()) != cross_num) {
      std::cout << "交叉异常1! ";
    }
    // 第二个染色体相应位置并执行交换
    for (int k = 0; k < cross_num; k++) {
      individuals_t[individual_num - i - 1][pos1 + k] = temp1[k];
    }
    // 第二个染色体规范化
    for (int k = 0, j = 0; j < cross_num && k < cross_num;) {
      while (k < cross_num && isExist(temp1[k], temp2))
        ++k;
      while (j < cross_num && isExist(temp2[j], temp1))
        ++j;
      if (k < cross_num && j < cross_num) {
        individuals_t[individual_num - i - 1][index[k]] = temp2[j];
        ++k;
        ++j;
      }
    }
    if (isError(individuals_t[individual_num - i - 1])) {
      std::cout << "error!" << std::endl;
    }
    // 对第一个染色体进行处理和规范化
    index = search(individuals_t[i], temp2);
    if ((int)index.size() != cross_num) {
      std::cout << "交叉异常2! ";
    }
    for (int k = 0; k < cross_num; k++) {
      individuals_t[i][pos1 + k] = temp2[k];
    }
    // 第一个染色体规范化
    for (int j = 0, k = 0; j < cross_num && k < cross_num;) {
      while (j < cross_num && isExist(temp1[j], temp2))
        ++j;
      while (k < cross_num && isExist(temp2[k], temp1))
        ++k;
      if (j < cross_num && k < cross_num) {
        individuals_t[i][index[k]] = temp1[j];
        ++k;
        ++j;
      }
    }
    if (isError(individuals_t[i])) {
      std::cout << "error!" << std::endl;
    }
  }
}

void Genetic::mutate() {
  // 采用基于位置的变异
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<double> u1(0.0, 1.0);
  std::uniform_int_distribution<signed> u2(0, std::max(0, city - 1));

  for (int i = 0; i < individual_num; ++i) {
    if (u1(rng) < mutate_prob) {
      int idx1 = u2(rng);
      int idx2 = u2(rng);
      if (idx1 != idx2) {
        std::swap(individuals_t[i][idx2], individuals_t[i][idx1]);
        // or larger fitness logits
      }
    }
  }
}

void Genetic::select() {
  // 使用锦标赛算法合并新旧种群
  std::vector<double> fit1 = get_fitness(individuals);
  std::vector<double> fit2 = get_fitness(individuals_t);
  int new_i = 0;
  std::vector<int> route;
  double fit;
  for (int i = 0; i < individual_num / 10; i++) {
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
      route = individuals[new_i];
      fit = fit1[new_i];
      individuals[new_i] = individuals[10 * i + index];
      individuals[10 * i + index] = route;
      fit1[10 * i + index] = fit;
      ++new_i;
    }
  }
  for (int i = 0; i < individual_num / 10; i++) {
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
      individuals[new_i] = individuals_t[10 * i + index];
      visit[index] = true;
      ++new_i;
    }
  }
  for (int i = 0; i < individual_num; i++) {
    individuals_t[i] = individuals[i];
  }
  // 或者使用轮盘赌算法
}

void Genetic::clock_opt() {
  for (int i = 0; i < individual_num; i++) {
    long long s = 0;
    for (int j = 0; j < city - 1; j++) {
      s += (*position)[individuals[i][j]][0] *
               (*position)[individuals[i][j + 1]][1] -
           (*position)[individuals[i][j]][1] *
               (*position)[individuals[i][j + 1]][0];
    }
    s += (*position)[individuals[i][city - 1]][0] *
             (*position)[individuals[i][0]][1] -
         (*position)[individuals[i][city - 1]][1] *
             (*position)[individuals[i][0]][0];
    // 顺时针
    if (s > 0) {
      std::reverse(individuals[i].begin(), individuals[i].end());
      std::reverse(individuals_t[i].begin(), individuals_t[i].end());
    }
  }
}

void Genetic::reverse() {
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_int_distribution<signed> u(0, std::max(0, city - 1));

  for (int i = 0; i < individual_num; ++i) {
    // 选取区间
    std::vector<int> route = individuals[i];
    int pos1 = u(rng);
    int pos2 = u(rng);
    if (pos2 < pos1) {
      std::swap(pos1, pos2);
    }
    int cnt = (pos2 - pos1 + 1) / 2;
    for (int j = 0; j < cnt; j++) {
      std::swap(route[pos1 + j], route[pos2 - j]);
    }
    if (get_fitness(route) > get_fitness(individuals[i])) {
      individuals[i] = route;
      individuals_t[i] = route;
    }
  }
}

QString Genetic::output() const {
  int idx = 0;
  double best = best_aim[0];
  for (int i = 1; i < max_iter; i++) {
    if (best_aim[i] < best) {
      best = best_aim[i];
      idx = i;
    }
  }

  QString str = "";
  if (city == 29) {
    str = "bayg29";
  } else if (city == 48) {
    str = "att48";
  } else if (city == 70) {
    str = "st70";
  }
  QString res = QString("( 遗传算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(str)
                    .arg(best_aim[idx]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[idx][i] + 1));
  }
  return res;
}

bool Genetic::isExist(int city, std::vector<int> &individual) const {
  return std::find(individual.begin(), individual.end(), city) !=
         individual.end();
}

bool Genetic::isError(std::vector<int> &individual) const {
  std::vector<bool> visit(city, false);
  for (int city : individual) {
    if (city < 0 || city >= this->city || visit[city]) {
      return true;
    }
    visit[city] = true;
  }
  return false;
}

const std::vector<std::vector<int>> &Genetic::get_route() const noexcept {
  return best_route;
}

const std::vector<double> &Genetic::get_best_aim() const noexcept {
  return best_aim;
}

const std::vector<double> &Genetic::get_avg_aim() const noexcept {
  return avg_aim;
}

Genetic::~Genetic() = default;
