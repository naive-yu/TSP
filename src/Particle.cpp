#include "Particle.h"
#include "resource.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

Particle::Particle() = default;
Particle::Particle(int city, int particle_num, int max_iter, double max_w,
                   double min_w, double c1, double c2)
    : city(city), particle_num(particle_num), max_iter(max_iter), max_w(max_w),
      min_w(min_w), c1(c1), c2(c2) {
  init();
}

void Particle::init() {
  if (city == 29) {
    position = bayg29_position;
    distance = &bayg29_distance;
  } else if (city == 48) {
    position = att48_position;
    distance = &att48_distance;
  } else if (city == 70) {
    position = st70_position;
    distance = &st70_distance;
  } else
    cout << "城市数量异常！";
  particles_best = vector<vector<int>>(particle_num, vector<int>(city));
  particles = vector<vector<int>>(particle_num, vector<int>(city));
  //    particles_best.resize(particle_num);
  //    for(int i=0;i<max_iter;i++)particles_best[i].resize(city);
  //    particles.resize(particle_num);
  //    for(int i=0;i<max_iter;i++)particles[i].resize(city);
  best_route.resize(max_iter);
  for (int i = 0; i < max_iter; i++)
    best_route[i].resize(city);
  avg_aim.resize(max_iter);
  best_aim.resize(max_iter);
}

void Particle::run() {
  // random_device rd;
  default_random_engine e(time(0));
  uniform_real_distribution<double> u1(0, 1);
  vector<int> route(city);
  for (int i = 0; i < city; i++)
    route[i] = i;
  for (int i = 0; i < particle_num; i++) {
    shuffle(route.begin(), route.end(), std::mt19937(std::random_device()()));
    particles[i] = route;
    particles_best[i] = route;
  }
  // 随机构建粒子初始速度
  vector<vector<int>> v(particle_num, vector<int>(city));
  for (int i = 0; i < city; i++)
    route[i] = i;
  for (int i = 0; i < particle_num; i++) {
    v[i] = route;
    shuffle(v[i].begin(), v[i].end(), std::mt19937(std::random_device()()));
  }
  // clock_opt();
  // 计算群体极值
  vector<double> lens = get_length(particles);
  int index = 0;
  double mini = INF;
  for (int i = 0; i < particle_num; i++) {
    if (lens[i] < mini) {
      index = i;
      mini = lens[i];
    }
  }
  best_route[0] = particles[index];
  best_aim[0] = mini;
  avg_aim[0] = accumulate(lens.begin(), lens.end(), 0.0) / particle_num;
  // vector<int> route(city);
  vector<vector<int>> change1(particle_num, vector<int>(city));
  vector<vector<int>> change2(particle_num, vector<int>(city));
  double w;
  for (cnt = 1; cnt < max_iter; cnt++) {
    // 更新惯性因子
    w = max_w - (max_w - min_w) * pow((cnt / (0.0 + max_iter)), 2);
    // 更新速度
    // 个体极值修正部分
    vector<int> p;
    vector<int> b;
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
    vector<int> gb = best_route[cnt - 1];
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
        if (u1(e) > w)
          v[i][j] = -1; // 一定概率保留原序列
        if (u1(e) < c1)
          v[i][j] = change1[i][j];
        if (u1(e) < c2)
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
    ////        //变异
    mutate();
    //        //进化逆转
    // reverse();

    // clock_opt();
    // 个体极值和群体极值更新
    lens = get_length(particles);
    for (int i = 0; i < particle_num; i++) {
      if (lens[i] < get_length(particles_best[i]))
        particles_best[i] = particles[i];
    }
    index = 0;
    mini = INF;
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
    int j;
    for (j = 0; j < city - 1; j++) {
      s += (position[particles[i][j]][0] * position[particles[i][j + 1]][1] -
            position[particles[i][j]][1] * position[particles[i][j + 1]][0]);
    }
    s += (position[particles[i][j]][0] * position[particles[i][0]][1] -
          position[particles[i][j]][1] * position[particles[i][0]][0]);
    // cout<<"s:"<<s<<endl;
    if (s > 0) { // 顺时针
      std::reverse(particles[i].begin(), particles[i].end());
    }
  }
}

void Particle::mutate() {
  // 采用基于位置的变异
  // random_device rd;
  default_random_engine e(time(0));
  uniform_real_distribution<double> u1(0, 1);
  uniform_int_distribution<signed> u2(0, city - 1);
  vector<int> route;
  for (int i = 0; i < particle_num; i++) {
    int index1 = u2(e);
    int index2 = u2(e);
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
  //    //random_device rd;
  default_random_engine e(time(0));
  uniform_int_distribution<signed> u(0, city - 1);
  int count; // 交换次数
  vector<int> route(city);
  for (int i = 0; i < particle_num; i++) {
    // 选取区间
    route = particles[i];
    int pos1 = u(e);
    int pos2 = u(e);
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

vector<int> Particle::search(vector<int> &individual, vector<int> &temp) const {
  int cross_num = (int)temp.size();
  vector<int> index(cross_num);
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

int Particle::find(vector<int> &t, int a) const {
  for (int i = 0; i < city; i++) {
    if (t[i] == a)
      return i;
  }
  return -1;
}

QString Particle::output() {
  // 找到最短路径
  int index = 0;
  double best = best_aim[0];
  for (int i = 1; i < max_iter; i++) {
    if (best_aim[i] < best) {
      best = best_aim[i];
      index = i;
    }
  }
  QString str = "";
  if (city == 48)
    str = "att48";
  else if (city == 70)
    str = "st70";
  QString res = QString("（粒子群优化算法，%1）最短环路距离：%2\n最短环路：")
                    .arg(str)
                    .arg(best_aim[index]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[index][i] + 1));
  }
  return res;
}

bool Particle::isExist(int c, vector<int> t) const {
  for (int i = 0; i < t.size(); i++) {
    if (t[i] == c)
      return true;
  }
  return false;
}

vector<double> *Particle::get_avg_aim() { return &avg_aim; }

vector<double> *Particle::get_best_aim() { return &best_aim; }

vector<vector<int>> *Particle::get_route() { return &best_route; }

double Particle::get_length(vector<int> &route) {
  double len = (*distance)[route[city - 1]][route[0]];
  for (int i = 0; i < city - 1; i++) {
    len += (*distance)[route[i]][route[i + 1]];
  }
  return len;
}

vector<double> Particle::get_length(vector<vector<int>> &total) {
  vector<double> res;
  for (vector<int> &route : total) {
    double len = (*distance)[route[city - 1]][route[0]];
    for (int i = 0; i < city - 1; i++) {
      len += (*distance)[route[i]][route[i + 1]];
    }
    res.push_back(len);
  }
  return res;
}

Particle::~Particle() = default;
