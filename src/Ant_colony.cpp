#include "Ant_colony.h"
#include <QString>
#include <chrono>
#include <cmath>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

Ant_colony::Ant_colony() = default;
Ant_colony::Ant_colony(int city, int ants, int max_iter, int Q, double alpha,
                       double beta, double rho)
    : city(city), ants(ants), max_iter(max_iter), Q(Q), alpha(alpha),
      beta(beta), rho(rho) {}

void Ant_colony::init(const std::vector<std::vector<int>> &pos,
                      const std::vector<std::vector<double>> &dis) {
  // 距离矩阵
  position = std::make_shared<const std::vector<std::vector<int>>>(pos);
  distance = std::make_shared<const std::vector<std::vector<double>>>(dis);

  // 信息素矩阵
  message.assign(city, std::vector<double>(city, 1.0));
  best_route.assign(max_iter, std::vector<int>(city, 0));
  avg_aim.assign(max_iter, 0.0);
  best_aim.assign(max_iter, ANTINF);
}

void Ant_colony::run() {
  // 高质量种子，线程局部引擎
  static thread_local std::mt19937 rng(
      (unsigned)std::chrono::high_resolution_clock::now()
          .time_since_epoch()
          .count());
  std::uniform_int_distribution<int> u(0, city - 1);
  std::uniform_real_distribution<double> ur(0.0, 1.0);
  for (int iter = 0; iter < max_iter; iter++) {
    // 每次迭代为每只蚂蚁生成路径
    std::vector<std::vector<int>> routes(ants, std::vector<int>(city));
    // 访问记录矩阵
    std::vector<std::vector<bool>> visited(ants, std::vector<bool>(city));
    // 随机生成初始城市
    for (int i = 0; i < ants; i++) {
      routes[i][0] = u(rng);
      visited[i][routes[i][0]] = true;
    }

    // 构建路径：按概率选择下一个城市
    for (int pos = 1; pos < city; pos++) {
      for (int ant = 0; ant < ants; ant++) {
        std::vector<double> p(city);
        for (int c = 0; c < city; c++) {
          // 未被此蚂蚁访问的才可能被下一次访问
          if (!visited[ant][c]) {
            double tau = message[routes[ant][pos - 1]][c];
            double eta =
                1.0 / static_cast<double>((*distance)[routes[ant][pos - 1]][c]);
            p[c] = std::pow(tau, alpha) * std::pow(eta, beta);
          } else {
            p[c] = 0;
          }
        }
        double total = std::accumulate(p.begin(), p.end(), 0.0);
        int select = -1;
        // 轮盘赌决定下一个城市
        while (select == -1) {
          double ps = ur(rng);
          for (int k = 0; k < city; k++) {
            if (!visited[ant][k] && ps <= p[k] / total) {
              select = k;
              break;
            }
          }
        }
        routes[ant][pos] = select;
        visited[ant][select] = true;
      }
    }

    // 记录本次迭代最佳路线的蚂蚁
    double best = ANTINF, sum = 0.0;
    int best_idx;
    for (int ant = 0; ant < ants; ant++) {
      double len = 0.0;
      for (int i = 0; i < city - 1; i++) {
        len += (*distance)[routes[ant][i]][routes[ant][i + 1]];
      }
      len += (*distance)[routes[ant][city - 1]][routes[ant][0]];
      sum += len;
      if (len < best) {
        best = len;
        best_idx = ant;
      }
    }

    // 最佳目标和平均目标
    best_aim[iter] = best;
    avg_aim[iter] = sum / static_cast<double>(ants);
    best_route[iter] = routes[best_idx];

    // 信息素挥发
    for (int i = 0; i < city; i++) {
      for (int j = 0; j < city; j++) {
        message[i][j] *= (1 - rho);
      }
    }

    // 信息素更新（在最优路径上释放信息素）
    for (int i = 0; i < city - 1; i++) {
      message[best_route[iter][i]][best_route[iter][i + 1]] +=
          static_cast<double>(Q) / best;
    }
    message[best_route[iter][city - 1]][best_route[iter][0]] +=
        static_cast<double>(Q) / best;
  }
}

QString Ant_colony::output() const {
  int idx = 0;
  double best = best_aim[0];
  for (int i = 1; i < max_iter; i++) {
    if (best_aim[i] < best) {
      best = best_aim[i];
      idx = i;
    }
  }

  QString name = "";
  if (city == 29) {
    name = "bayg29";
  } else if (city == 48) {
    name = "att48";
  } else if (city == 70) {
    name = "st70";
  }
  QString res = QString("( 蚁群算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(name)
                    .arg(best_aim[idx]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[idx][i] + 1));
  }
  return res;
}

const std::vector<double> &Ant_colony::get_avg_aim() const { return avg_aim; }

const std::vector<double> &Ant_colony::get_best_aim() const { return best_aim; }

const std::vector<std::vector<int>> &Ant_colony::get_route() const {
  return best_route;
}

Ant_colony::~Ant_colony() = default;
