#include "Ant_colony.h"
#include "Algorithm.h"
#include <QLoggingCategory>
#include <QString>
#include <cmath>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

Q_LOGGING_CATEGORY(AntColonyLog, "AntColony")

Ant_colony::Ant_colony(int city, int ants, int max_iter, int Q, double alpha,
                       double beta, double rho)
    : Algorithm(city, max_iter, "Ant Colony"), ants_(ants), Q_(Q),
      alpha_(alpha), beta_(beta), rho_(rho) {}

Ant_colony::Ant_colony(int city, AntParams &params)
    : Algorithm(city, params.max_iter, "Ant Colony"), ants_(params.ants),
      Q_(params.Q), alpha_(params.alpha), beta_(params.beta), rho_(params.rho) {
}

Ant_colony::Ant_colony(int city, AntParams &&params)
    : Algorithm(city, params.max_iter, "Ant Colony"), ants_(params.ants),
      Q_(params.Q), alpha_(params.alpha), beta_(params.beta), rho_(params.rho) {
}

void Ant_colony::init(const std::vector<std::vector<int>> &pos,
                      const std::vector<std::vector<double>> &dis) {
  qCInfo(AntColonyLog) << "Initializing Ant Colony algorithm with" << city_
                       << "cities," << ants_ << "ants," << max_iter_
                       << "max_iter," << Q_ << "Q," << alpha_ << "alpha,"
                       << beta_ << "beta," << rho_ << "rho.";

  // 距离矩阵
  Algorithm::init(pos, dis);
  // 信息素矩阵
  message.assign(city_, std::vector<double>(city_, 1.0));

  qCInfo(AntColonyLog) << "Ant Colony algorithm initialized.";
}

void Ant_colony::run() {
  while (runStep()) {
  };
}

bool Ant_colony::runStep() {
  if (cur_iter_ >= max_iter_) {
    return false;
  }
  // 每次迭代为每只蚂蚁生成路径
  std::vector<std::vector<int>> routes(ants_, std::vector<int>(city_));
  // 访问记录矩阵
  std::vector<std::vector<bool>> visited(ants_, std::vector<bool>(city_));
  // 随机生成初始城市
  for (int i = 0; i < ants_; i++) {
    routes[i][0] = (*u_)(*rng_);
    visited[i][routes[i][0]] = true;
  }

  // 构建路径：按概率选择下一个城市
  for (int pos = 1; pos < city_; pos++) {
    for (int ant = 0; ant < ants_; ant++) {
      std::vector<double> p(city_);
      for (int c = 0; c < city_; c++) {
        // 未被此蚂蚁访问的才可能被下一次访问
        if (!visited[ant][c]) {
          double tau = message[routes[ant][pos - 1]][c];
          double eta =
              1.0 / static_cast<double>((*distance_)[routes[ant][pos - 1]][c]);
          p[c] = std::pow(tau, alpha_) * std::pow(eta, beta_);
        } else {
          p[c] = 0;
        }
      }
      double total = std::accumulate(p.begin(), p.end(), 0.0);
      int select = -1;
      // 轮盘赌决定下一个城市
      while (select == -1) {
        double ps = (*ur_)(*rng_);
        for (int k = 0; k < city_; k++) {
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
  double best = ALGO_INF, sum = 0.0;
  int best_idx;
  for (int ant = 0; ant < ants_; ant++) {
    double len = 0.0;
    for (int i = 0; i < city_ - 1; i++) {
      len += (*distance_)[routes[ant][i]][routes[ant][i + 1]];
    }
    len += (*distance_)[routes[ant][city_ - 1]][routes[ant][0]];
    sum += len;
    if (len < best) {
      best = len;
      best_idx = ant;
    }
  }

  // 最佳目标和平均目标
  best_aim_[cur_iter_] = best;
  avg_aim_[cur_iter_] = sum / static_cast<double>(ants_);
  best_route_[cur_iter_] = routes[best_idx];

  // 信息素挥发
  for (int i = 0; i < city_; i++) {
    for (int j = 0; j < city_; j++) {
      message[i][j] *= (1 - rho_);
    }
  }

  // 信息素更新（在最优路径上释放信息素）
  for (int i = 0; i < city_ - 1; i++) {
    message[best_route_[cur_iter_][i]][best_route_[cur_iter_][i + 1]] +=
        static_cast<double>(Q_) / best;
  }
  message[best_route_[cur_iter_][city_ - 1]][best_route_[cur_iter_][0]] +=
      static_cast<double>(Q_) / best;
  ++cur_iter_;
  return true;
}

QString Ant_colony::output() const {
  int idx = 0;
  double best = best_aim_[0];
  for (int i = 1; i < max_iter_; i++) {
    if (best_aim_[i] < best) {
      best = best_aim_[i];
      idx = i;
    }
  }

  QString name = "";
  if (city_ == 29) {
    name = "bayg29";
  } else if (city_ == 48) {
    name = "att48";
  } else if (city_ == 70) {
    name = "st70";
  }
  QString res = QString("( 蚁群算法 %1 ) 最短环路距离: %2\n最短环路: ")
                    .arg(name)
                    .arg(best_aim_[idx]);
  for (int i = 0; i < city_; i++) {
    res.append(QString("%1, ").arg(best_route_[idx][i] + 1));
  }
  return res;
}
