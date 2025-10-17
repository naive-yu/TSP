#include "Particle.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <numeric>
#include <random>

Particle::Particle() = default;
Particle::Particle(int city, int particle_num, int max_iter, double max_w,
                   double min_w, double c1, double c2)
    : city(city), particle_num(particle_num), max_iter(max_iter), max_w(max_w),
      min_w(min_w), c1(c1), c2(c2) {}

void Particle::init(const std::vector<std::vector<int>> &pos,
                    const std::vector<std::vector<double>> &dis) {
  position = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance = std::make_shared<const std::vector<std::vector<double>>>(dis);

  particles_best.assign(particle_num, std::vector<int>(city));
  particles.assign(particle_num, std::vector<int>(city));
  best_route.assign(max_iter, std::vector<int>(city));
  avg_aim.assign(max_iter, 0.0);
  best_aim.assign(max_iter, PARTINF);
}

void Particle::run() {
  // 线程局部随机数引擎
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<double> u1(0, 1);
  std::vector<int> route(city);
  for (int i = 0; i < city; i++)
    route[i] = i;
  for (int i = 0; i < particle_num; i++) {
    shuffle(route.begin(), route.end(), std::mt19937(std::random_device()()));
    particles[i] = route;
    particles_best[i] = route;
  }

  // 随机构建粒子初始速度
  std::vector<std::vector<int>> v(particle_num, std::vector<int>(city));
  for (int i = 0; i < city; i++)
    route[i] = i;
  for (int i = 0; i < particle_num; i++) {
    v[i] = route;
    shuffle(v[i].begin(), v[i].end(), std::mt19937(std::random_device()()));
  }

  // 计算群体极值
  std::vector<double> lens = get_length(particles);
  int index = 0;
  double mini = PARTINF;
  for (int i = 0; i < particle_num; i++) {
    if (lens[i] < mini) {
      index = i;
      mini = lens[i];
    }
  }
  best_route[0] = particles[index];
  best_aim[0] = mini;
  avg_aim[0] = std::accumulate(lens.begin(), lens.end(), 0.0) / particle_num;

  std::vector<std::vector<int>> change1(particle_num, std::vector<int>(city));
  std::vector<std::vector<int>> change2(particle_num, std::vector<int>(city));
  double w;
  for (cnt = 1; cnt < max_iter; cnt++) {
    // 更新惯性因子
    w = max_w - (max_w - min_w) * pow((cnt / (0.0 + max_iter)), 2);
    // 更新速度, 个体极值修正部分
    std::vector<int> p;
    std::vector<int> b;
    for (int i = 0; i < particle_num; i++) {
      p = particles[i];
      b = particles_best[i];
      for (int j = 0; j < city; j++) {
        change1[i][j] = find(p, b[j]); // 执行速度计算
        int temp = p[j];
        p[j] = p[change1[i][j]];
        p[change1[i][j]] = temp;
      }
    }
    // 群体极值修正部分
    std::vector<int> gb = best_route[cnt - 1];
    for (int i = 0; i < particle_num; i++) {
      p = particles[i];
      for (int j = 0; j < city; j++) {
        change2[i][j] = find(p, gb[j]);
        int temp = p[j];
        p[j] = p[change2[i][j]];
        p[change2[i][j]] = temp;
      }
    }
    // 原速度部分
    for (int i = 0; i < particle_num; i++) {
      for (int j = 0; j < city; j++) {
        if (u1(rng) > w)
          v[i][j] = -1; // 一定概率保留原序列
        if (u1(rng) < c1)
          v[i][j] = change1[i][j];
        if (u1(rng) < c2)
          v[i][j] = change2[i][j];
      }
    }
    // 更新位置
    for (int i = 0; i < particle_num; i++) {
      for (int j = 0; j < city; j++) {
        if (v[i][j] != -1) {
          int temp = particles[i][j];
          particles[i][j] = particles[i][v[i][j]];
          particles[i][v[i][j]] = temp;
        }
      }
    }
    // 变异
    mutate();
    // 进化逆转
    // reverse();
    // 时针优化
    // clock_opt();

    // 个体极值和群体极值更新
    lens = get_length(particles);
    for (int i = 0; i < particle_num; i++) {
      if (lens[i] < get_length(particles_best[i]))
        particles_best[i] = particles[i];
    }
    index = 0;
    mini = PARTINF;
    for (int i = 0; i < particle_num; i++) {
      if (lens[i] < mini) {
        index = i;
        mini = lens[i];
      }
    }
    best_route[cnt] = particles[index];
    best_aim[cnt] = mini;
    avg_aim[cnt] = accumulate(lens.begin(), lens.end(), 0.0) / particle_num;
  }
}

void Particle::clock_opt() {
  for (int i = 0; i < particle_num; i++) {
    long long s = 0;
    for (int j = 0; j < city - 1; j++) {
      s +=
          (*position)[particles[i][j]][0] *
              (*position)[particles[i][j + 1]][1] -
          (*position)[particles[i][j]][1] * (*position)[particles[i][j + 1]][0];
    }
    s += (*position)[particles[i][city - 1]][0] *
             (*position)[particles[i][0]][1] -
         (*position)[particles[i][city - 1]][1] *
             (*position)[particles[i][0]][0];

    // 顺时针
    if (s > 0) {
      std::reverse(particles[i].begin(), particles[i].end());
    }
  }
}

void Particle::mutate() {
  // 线程局部随机数引擎
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<double> u1(0, 1);
  std::uniform_int_distribution<signed> u2(0, std::max(0, city - 1));
  std::vector<int> route;

  // 采用基于位置的变异
  for (int i = 0; i < particle_num; i++) {
    int index1 = u2(rng);
    int index2 = u2(rng);
    if (index1 != index2) {
      route = particles[i];
      route[index1] = particles[i][index2];
      route[index2] = particles[i][index1];
      if (get_length(route) < get_length(particles[i])) {
        particles[i] = route;
        if (get_length(route) < get_length(particles_best[i]))
          particles_best[i] = route;
      }
    }
  }
}

void Particle::reverse() {
  // 线程局部随机数引擎
  static thread_local std::mt19937 rng(static_cast<unsigned>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::uniform_int_distribution<signed> u(0, std::max(0, city - 1));
  int count; // 交换次数
  std::vector<int> route(city);
  for (int i = 0; i < particle_num; i++) {
    // 选取区间
    route = particles[i];
    int pos1 = u(rng);
    int pos2 = u(rng);
    if (pos2 < pos1) {
      int temp = pos1;
      pos1 = pos2;
      pos2 = temp;
    }
    count = (pos2 - pos1 + 1) / 2;
    for (int j = 0; j < count; j++) {
      int temp = route[pos1 + j];
      route[pos1 + j] = route[pos2 - j];
      route[pos2 - j] = temp;
    }
    if (get_length(route) < get_length(particles[i]))
      particles[i] = route; // 进化
  }
}

std::vector<int> Particle::search(const std::vector<int> &individual,
                                  const std::vector<int> &temp) const {
  int cross_num = (int)temp.size();
  std::vector<int> index(cross_num);
  for (int i = 0; i < cross_num; i++) {
    for (int j = 0; j < city; j++) {
      if (individual[j] == temp[i]) {
        index[i] = j;
        break;
      }
    }
  }
  return index;
}

int Particle::find(const std::vector<int> &paticle, int city) const {
  return std::find(paticle.begin(), paticle.end(), city) - paticle.begin();
}

QString Particle::output() const {
  int index = 0;
  double best = best_aim[0];
  for (int i = 1; i < max_iter; i++) {
    if (best_aim[i] < best) {
      best = best_aim[i];
      index = i;
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
  QString res = QString("( 粒子群优化算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(str)
                    .arg(best_aim[index]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[index][i] + 1));
  }
  return res;
}

double Particle::get_length(const std::vector<int> &route) const {
  double len = (*distance)[route[city - 1]][route[0]];
  for (int i = 0; i < city - 1; i++) {
    len += (*distance)[route[i]][route[i + 1]];
  }
  return len;
}

std::vector<double>
Particle::get_length(const std::vector<std::vector<int>> &total) const {
  std::vector<double> res;
  res.reserve(total.size());
  for (const std::vector<int> &route : total) {
    double len = (*distance)[route[city - 1]][route[0]];
    for (int i = 0; i < city - 1; i++) {
      len += (*distance)[route[i]][route[i + 1]];
    }
    res.push_back(len);
  }
  return res;
}

bool Particle::isExist(int city, const std::vector<int> &paticle) const {
  return std::find(paticle.begin(), paticle.end(), city) != paticle.end();
}

const std::vector<double> &Particle::get_avg_aim() const noexcept {
  return avg_aim;
}

const std::vector<double> &Particle::get_best_aim() const noexcept {
  return best_aim;
}

const std::vector<std::vector<int>> &Particle::get_route() const noexcept {
  return best_route;
}

Particle::~Particle() = default;
