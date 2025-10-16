#include "Ant_colony.h"
#include "resource.h"
#include <QString>
#include <cmath>
#include <iostream>
#include <random>

using namespace std;
Ant_colony::Ant_colony() = default;
Ant_colony::Ant_colony(int city, int ants, int max_iter, int Q, double alpha,
                       double beta, double rho)
    : city(city), ants(ants), max_iter(max_iter), Q(Q), alpha(alpha),
      beta(beta), rho(rho) {
  init();
}

void Ant_colony::init() {
  if (city == 29) {
    // position=&bayg29_position;
    distance = &bayg29_distance;
  } else if (city == 48) {
    // position=&att48_position;
    distance = &att48_distance;
  } else if (city == 70) {
    // position=&st70_position;
    distance = &st70_distance;
  } else
    cout << "城市数量异常！";
  // 城市距离初始化
  //     distance.resize(city);
  //     for(int i=0;i<city;i++)distance[i].resize(city);
  //     for(int i=0;i<city;i++){
  //         for(int j=i+1;j<city;j++){
  //             distance[i][j]=(float)sqrt(pow((*position)[i][0]-(*position)[j][0],2)
  //                     +pow((*position)[i][1]-(*position) [j][1],2));
  //             distance[j][i]=distance[i][j];
  //         }
  //     }
  //     cout.precision(8);
  // 信息素矩阵
  message.resize(city);
  for (int i = 0; i < city; i++)
    message[i].assign(city, 1);
  best_route.resize(max_iter);
  for (int i = 0; i < max_iter; i++)
    best_route[i].resize(city);
  avg_aim.resize(max_iter);
  best_aim.resize(max_iter);
}

void Ant_colony::run() {
  // 使用随机数引擎
  // random_device rd;
  // cout<<time(0)<<endl;
  default_random_engine e(time(0)); // time(0)
  uniform_int_distribution<signed> u(0, city - 1);
  uniform_real_distribution<double> u1(0, 1);
  for (int count = 0; count < max_iter; count++) {
    vector<vector<int>> route(ants, vector<int>(city)); // 存放每只蚂蚁的路径
    // 随机生成初始城市
    vector<vector<bool>> visit(ants, vector<bool>(city)); // 访问矩阵
    for (int i = 0; i < ants; i++) {
      route[i][0] = u(e);
      visit[i][route[i][0]] = true;
    }
    // 每只蚂蚁按概率函数选择下一座城市
    for (int j = 1; j < city; j++) {
      for (int i = 0; i < ants; i++) {
        vector<double> p(city); // 概率矩阵
        for (int k = 0; k < city; k++) {
          if (!visit[i][k]) { // 未访问过
            p[k] = pow(message[route[i][j - 1]][k], alpha) *
                   pow(1 / (*distance)[route[i][j - 1]][k], beta);
          } else
            p[k] = 0;
        }
        double total = 0;
        for (int k = 0; k < city; k++) {
          total += p[k];
        }
        int select = -1;
        while (select == -1) { // 轮盘赌决定下一个城市
          double ps = u1(e);
          for (int k = 0; k < city; k++) {
            if (!visit[i][k] && ps <= p[k] / total) {
              select = k;
              break;
            }
          }
        }
        route[i][j] = select;
        visit[i][select] = true;
      }
    }
    // 记录本次迭代最佳路线
    vector<double> len(ants);
    double best = INF, sum = 0;
    int index;
    for (int i = 0; i < ants; i++) {
      for (int j = 0; j < city - 1; j++) {
        len[i] += (*distance)[route[i][j]][route[i][j + 1]];
      }
      len[i] += (*distance)[route[i][city - 1]][route[i][0]];
      sum += len[i];
      if (len[i] < best) {
        best = len[i];
        index = i;
      }
    }
    // 最佳和平均目标
    best_aim[count] = best;
    avg_aim[count] = sum / ants;
    for (int i = 0; i < city; i++) {
      best_route[count][i] = route[index][i];
    }
    // best_route[count]=route[index];
    // 更新全局信息素
    for (int i = 0; i < city; i++) {
      for (int j = 0; j < city; j++) {
        message[i][j] *= (1 - rho);
      }
    }
    for (int i = 0; i < city - 1; i++) {
      message[best_route[count][i]][best_route[count][i + 1]] += Q / best;
    } // 在整个路径上的信息素增量
    message[best_route[count][city - 1]][best_route[count][0]] += Q / best;
  }
}
QString Ant_colony::output() {
  // 找到最短路径
  int index = 0;
  double best = best_aim[0];
  for (int i = 1; i < max_iter; i++) {
    if (best_aim[i] < best) {
      best = best_aim[i];
      index = i;
    }
  }
  // 输出结果
  //    cout << "蚁群算法：我的最短环路距离：" << best_aim[index] << endl;
  //    cout << "我的最短环路：";
  //    for (int i = 0; i < city; i++) {
  //        cout << best_route[index][i]+1 << "->";
  //    }
  //    cout << endl;
  QString str = "";
  if (city == 48)
    str = "att48";
  else if (city == 70)
    str = "st70";
  QString res = QString("（蚁群算法，%1）最短环路距离：%2\n最短环路：")
                    .arg(str)
                    .arg(best_aim[index]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[index][i] + 1));
  }
  return res;
}

vector<double> *Ant_colony::get_avg_aim() { return &avg_aim; }

vector<double> *Ant_colony::get_best_aim() { return &best_aim; }

vector<vector<int>> *Ant_colony::get_route() { return &best_route; }

Ant_colony::~Ant_colony() = default;
