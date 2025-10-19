#include "Particle.h"
#include "Algorithm.h"
#include <QLoggingCategory>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>

Q_LOGGING_CATEGORY(ParticleLog, "Particle")

Particle::Particle(int city, int max_iter, int particle_num, double max_w,
                   double min_w, double c1, double c2)
    : Algorithm(city, max_iter, "Particle"), particle_num_(particle_num),
      max_w_(max_w), min_w_(min_w), c1_(c1), c2_(c2) {}

Particle::Particle(int city, ParticleParams &params)
    : Algorithm(city, params.max_iter, "Particle"),
      particle_num_(params.particle_num), max_w_(params.w1), min_w_(params.w2),
      c1_(params.c1), c2_(params.c2) {}

Particle::Particle(int city, ParticleParams &&params)
    : Algorithm(city, params.max_iter, "Particle"),
      particle_num_(params.particle_num), max_w_(params.w1), min_w_(params.w2),
      c1_(params.c1), c2_(params.c2) {}

void Particle::init(const std::vector<std::vector<int>> &pos,
                    const std::vector<std::vector<double>> &dis) {
  qCInfo(ParticleLog) << "Initializing Particle algorithm with" << city_
                      << "cities," << max_iter_ << "max_iter," << particle_num_
                      << "particle_num," << max_w_ << "max_w," << min_w_
                      << "min_w," << c1_ << "c1," << c2_ << "c2.";

  Algorithm::init(pos, dis);
  // 粒子群初始化
  particles_best.assign(particle_num_, std::vector<int>(city_));
  particles.assign(particle_num_, std::vector<int>(city_));

  std::vector<int> route(city_);
  for (int i = 0; i < city_; i++) {
    route[i] = i;
  }
  for (int i = 0; i < particle_num_; i++) {
    shuffle(route.begin(), route.end(), *rng_);
    particles[i] = route;
    particles_best[i] = route;
  }

  // 随机构建粒子初始速度
  velocity_.assign(particle_num_, std::vector<int>(city_));
  for (int i = 0; i < city_; i++) {
    route[i] = i;
  }
  for (int i = 0; i < particle_num_; i++) {
    velocity_[i] = route;
    shuffle(velocity_[i].begin(), velocity_[i].end(), *rng_);
  }

  // 计算群体极值
  std::vector<double> lens = get_length(particles);
  int index = 0;
  double mini = ALGO_INF;
  for (int i = 0; i < particle_num_; i++) {
    if (lens[i] < mini) {
      index = i;
      mini = lens[i];
    }
  }
  best_route_[0] = particles[index];
  best_aim_[0] = mini;
  avg_aim_[0] = std::accumulate(lens.begin(), lens.end(), 0.0) / particle_num_;
  cur_iter_ = 1;

  qCInfo(ParticleLog) << "Particle algorithm initialized.";
}

void Particle::run() {
  while (runStep()) {
  };
}

bool Particle::runStep() {
  if (cur_iter_ >= max_iter_) {
    return false;
  }

  // change
  std::vector<std::vector<int>> change1(particle_num_, std::vector<int>(city_));
  std::vector<std::vector<int>> change2(particle_num_, std::vector<int>(city_));

  // 更新惯性因子
  double w =
      max_w_ - (max_w_ - min_w_) * pow((cur_iter_ / (0.0 + max_iter_)), 2);
  // 更新速度, 个体极值修正部分
  std::vector<int> p;
  std::vector<int> b;
  for (int i = 0; i < particle_num_; i++) {
    p = particles[i];
    b = particles_best[i];
    for (int j = 0; j < city_; j++) {
      change1[i][j] = find(p, b[j]); // 执行速度计算
      int temp = p[j];
      p[j] = p[change1[i][j]];
      p[change1[i][j]] = temp;
    }
  }
  // 群体极值修正部分
  std::vector<int> gb = best_route_[cur_iter_ - 1];
  for (int i = 0; i < particle_num_; i++) {
    p = particles[i];
    for (int j = 0; j < city_; j++) {
      change2[i][j] = find(p, gb[j]);
      int temp = p[j];
      p[j] = p[change2[i][j]];
      p[change2[i][j]] = temp;
    }
  }
  // 原速度部分
  for (int i = 0; i < particle_num_; i++) {
    for (int j = 0; j < city_; j++) {
      if ((*ur_)(*rng_) > w)
        velocity_[i][j] = -1; // 一定概率保留原序列
      if ((*ur_)(*rng_) < c1_)
        velocity_[i][j] = change1[i][j];
      if ((*ur_)(*rng_) < c2_)
        velocity_[i][j] = change2[i][j];
    }
  }
  // 更新位置
  for (int i = 0; i < particle_num_; i++) {
    for (int j = 0; j < city_; j++) {
      if (velocity_[i][j] != -1) {
        int temp = particles[i][j];
        particles[i][j] = particles[i][velocity_[i][j]];
        particles[i][velocity_[i][j]] = temp;
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
  auto lens = get_length(particles);
  for (int i = 0; i < particle_num_; i++) {
    if (lens[i] < get_length(particles_best[i]))
      particles_best[i] = particles[i];
  }
  int index = 0;
  double mini = ALGO_INF;
  for (int i = 0; i < particle_num_; i++) {
    if (lens[i] < mini) {
      index = i;
      mini = lens[i];
    }
  }
  best_route_[cur_iter_] = particles[index];
  best_aim_[cur_iter_] = mini;
  avg_aim_[cur_iter_] =
      accumulate(lens.begin(), lens.end(), 0.0) / particle_num_;

  ++cur_iter_;
  return true;
}

void Particle::clock_opt() {
  for (int i = 0; i < particle_num_; i++) {
    long long s = 0;
    for (int j = 0; j < city_ - 1; j++) {
      s += (*position_)[particles[i][j]][0] *
               (*position_)[particles[i][j + 1]][1] -
           (*position_)[particles[i][j]][1] *
               (*position_)[particles[i][j + 1]][0];
    }
    s += (*position_)[particles[i][city_ - 1]][0] *
             (*position_)[particles[i][0]][1] -
         (*position_)[particles[i][city_ - 1]][1] *
             (*position_)[particles[i][0]][0];

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
  std::uniform_int_distribution<signed> u2(0, std::max(0, city_ - 1));
  std::vector<int> route;

  // 采用基于位置的变异
  for (int i = 0; i < particle_num_; i++) {
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
  std::uniform_int_distribution<signed> u(0, std::max(0, city_ - 1));
  int count; // 交换次数
  std::vector<int> route(city_);
  for (int i = 0; i < particle_num_; i++) {
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
    for (int j = 0; j < city_; j++) {
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
  double best = best_aim_[0];
  for (int i = 1; i < max_iter_; i++) {
    if (best_aim_[i] < best) {
      best = best_aim_[i];
      index = i;
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
  QString res = QString("( 粒子群优化算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(str)
                    .arg(best_aim_[index]);
  for (int i = 0; i < city_; i++) {
    res.append(QString("%1, ").arg(best_route_[index][i] + 1));
  }
  return res;
}

double Particle::get_length(const std::vector<int> &route) const {
  double len = (*distance_)[route[city_ - 1]][route[0]];
  for (int i = 0; i < city_ - 1; i++) {
    len += (*distance_)[route[i]][route[i + 1]];
  }
  return len;
}

std::vector<double>
Particle::get_length(const std::vector<std::vector<int>> &total) const {
  std::vector<double> res;
  res.reserve(total.size());
  for (const std::vector<int> &route : total) {
    double len = (*distance_)[route[city_ - 1]][route[0]];
    for (int i = 0; i < city_ - 1; i++) {
      len += (*distance_)[route[i]][route[i + 1]];
    }
    res.push_back(len);
  }
  return res;
}

bool Particle::isExist(int city, const std::vector<int> &paticle) const {
  return std::find(paticle.begin(), paticle.end(), city) != paticle.end();
}
