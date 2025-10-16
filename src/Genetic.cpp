#include "Genetic.h"
#include "resource.h"
#include <iostream>
#include <qDebug>
#include <random>

using namespace std;
Genetic::Genetic() = default;
Genetic::Genetic(int city, int individual_num, int max_iter, double mutate_prob)
    : city(city), individual_num(individual_num), max_iter(max_iter),
      mutate_prob(mutate_prob) {
  init();
}

void Genetic::init() {
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
  // 开辟空间
  individuals.resize(individual_num);
  for (int i = 0; i < individual_num; i++)
    individuals[i].resize(city);
  individuals_t.resize(individual_num);
  for (int i = 0; i < individual_num; i++)
    individuals_t[i].resize(city);
  best_route.resize(max_iter);
  for (int i = 0; i < max_iter; i++)
    best_route[i].resize(city);
  avg_aim.resize(max_iter);
  best_aim.resize(max_iter);
}

void Genetic::run() {
  // 生成初始种群
  // 依据贪心思路构建
  // random_device rd;
  //    default_random_engine e(time(0));
  //    for(int j=0;j<individual_num;j++){
  //        vector<int> route(city);//路径矩阵
  //        vector<bool> visit(city);//访问矩阵
  //        uniform_int_distribution<signed> u(0,city-1);
  //        route[0]=u(e);
  //        visit[route[0]]=true;
  //        individuals[j][0]=route[0];
  //        individuals_t[j][0]=route[0];
  //        for(int i=1;i<city;i++){
  //            //找到距离最近的城市
  //            int index=-1;
  //            double best=INF;
  //            for(int k=0;k<city;k++){
  //                if(!visit[k]&&(*distance)[route[i-1]][k]<best){
  //                    index=k;
  //                    best=(*distance)[route[i-1]][k];
  //                }
  //            }
  //            route[i]=index;
  //            individuals[j][i]=index;
  //            individuals_t[j][i]=index;
  //            visit[index]=true;
  //        }
  //    }
  // 随机构建
  vector<int> route(city);
  for (int i = 0; i < city; i++)
    route[i] = i;
  for (int i = 0; i < individual_num; i++) {
    individuals[i] = route;
    individuals_t[i] = route;
    shuffle(route.begin(), route.end(), std::mt19937(std::random_device()()));
  }
  // 开始遗传算法
  for (int count = 0; count < max_iter; count++) {
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
    vector<double> fitness = get_fitness(individuals);
    avg_aim[count] =
        individual_num / accumulate(fitness.begin(), fitness.end(), 0.0);
    double max_fitness = 0;
    int max_index = -1;
    for (int i = 0; i < individual_num; i++) {
      if (fitness[i] > max_fitness) {
        max_fitness = fitness[i];
        max_index = i;
      }
    }
    best_aim[count] = 1.0 / max_fitness;
    // cout<<best_aim[count]<<endl;
    best_route[count] = individuals[max_index];
  }
}

vector<int> Genetic::search(vector<int> &individual, vector<int> &temp) const {
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

double Genetic::get_fitness(vector<int> &route) {
  double dis = (*distance)[route[city - 1]][route[0]];
  for (int i = 0; i < city - 1; i++) {
    dis += (*distance)[route[i]][route[i + 1]];
  }
  return 1.0 / dis;
}

vector<double> Genetic::get_fitness(vector<vector<int>> &all_individuals) {
  vector<double> fitness(all_individuals.size());
  int index = 0;
  double dis;
  for (vector<int> &route : all_individuals) {
    dis = (*distance)[route[city - 1]][route[0]];
    for (int i = 0; i < city - 1; i++) {
      dis += (*distance)[route[i]][route[i + 1]];
    }
    fitness[index++] = 1.0 / dis;
  }
  return fitness;
}

void Genetic::cross() {
  // random_device rd;
  default_random_engine e(time(0));
  uniform_int_distribution<signed> u(0, city - 1);
  for (int i = 0; i < individual_num / 2; i++) {
    int pos1 = u(e); // 该位置后cross_num个城市
    int pos2 = u(e);
    // cout<<pos1<<","<<pos2<<endl;
    if (pos2 < pos1) {
      int temp = pos1;
      pos1 = pos2;
      pos2 = temp;
    }
    int cross_num = pos2 - pos1 + 1;
    vector<int> temp1(cross_num);
    vector<int> temp2(cross_num);
    vector<int> index;
    for (int j = 0; j < cross_num; j++) {
      temp1[j] = individuals_t[i][pos1 + j];
      temp2[j] = individuals_t[individual_num - i - 1][pos1 + j];
    }
    // 记录第二条染色体中对应的下标
    index = search(individuals_t[individual_num - i - 1], temp1);
    if ((int)index.size() != cross_num) {
      cout << "交叉异常1！";
      // throw exception();
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
      cout << "error!" << endl;
    }
    // 对第一个染色体进行处理和规范化
    index = search(individuals_t[i], temp2);
    if ((int)index.size() != cross_num) {
      cout << "交叉异常2！";
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
      cout << "error!" << endl;
    }
  }
}

void Genetic::mutate() {
  // 采用基于位置的变异
  // random_device rd;
  default_random_engine e(time(0));
  uniform_real_distribution<double> u1(0, 1);
  uniform_int_distribution<signed> u2(0, city - 1);
  vector<int> route;
  for (int i = 0; i < individual_num; i++) {
    if (u1(e) < mutate_prob) {
      int index1 = u2(e);
      int index2 = u2(e);
      if (index1 != index2) {
        route = individuals_t[i];
        route[index1] = individuals_t[i][index2];
        route[index2] = individuals_t[i][index1];
        individuals_t[i] = route;
        // if(get_fitness(route)>get_fitness(individuals_t[i]))individuals_t[i]=route;
      }
    }
  }
}

void Genetic::select() {
  // 合并新旧种群
  // 35800，individuals优先,参数48,100,400,4,0.1
  // 使用锦标赛算法
  vector<double> fit1 = get_fitness(individuals);
  vector<double> fit2 = get_fitness(individuals_t);
  int new_i = 0;
  vector<int> route;
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
    vector<bool> visit(10, false);
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
  for (int i = 0; i < individual_num; i++)
    individuals_t[i] = individuals[i];
  // 计算种群适应度
  //    vector<vector<int>> all_individuals = individuals_t;
  //    all_individuals.insert(all_individuals.end(),individuals.begin(),individuals.end());
  //    vector<double> fitness=get_fitness(all_individuals);
  //    //使用轮盘赌算法
  //    random_device rd;
  //    default_random_engine e(rd());
  //    uniform_real_distribution<double> u(0,1);
  //    double sum=0;
  //    vector<bool> visit(individual_num*2);
  //    for(int i=0;i<individual_num*2;i++){
  //        sum+=fitness[i];
  //    }
  //    for(int i=0;i<individual_num;i++){
  //        int select=-1;
  //        while(select==-1){
  //            double p=u(e);
  //            for(int j=0;j<individual_num*2;j++){
  //                if(!visit[j]&&p<=fitness[j]/sum){
  //                    select=j;
  //                    break;
  //                }
  //            }
  //        }
  //        visit[select]=true;
  //        sum-=fitness[select];
  ////        if(isError(all_individuals[select])){
  ////            cout<<"error!"<<endl;
  ////        }
  //        individuals[i]=all_individuals[select];
  //        individuals_t[i]=all_individuals[select];
  //    }
}

void Genetic::clock_opt() {
  for (int i = 0; i < individual_num; i++) {
    long long s = 0;
    int j;
    for (j = 0; j < city - 1; j++) {
      s +=
          (position[individuals[i][j]][0] * position[individuals[i][j + 1]][1] -
           position[individuals[i][j]][1] * position[individuals[i][j + 1]][0]);
    }
    s += (position[individuals[i][j]][0] * position[individuals[i][0]][1] -
          position[individuals[i][j]][1] * position[individuals[i][0]][0]);
    // cout<<"s:"<<s<<endl;
    if (s > 0) { // 顺时针
      std::reverse(individuals[i].begin(), individuals[i].end());
      std::reverse(individuals_t[i].begin(), individuals_t[i].end());
    }
  }
}

void Genetic::reverse() {
  //    //random_device rd;
  default_random_engine e(time(0));
  uniform_int_distribution<signed> u(0, city - 1);
  int cnt; // 交换次数
  vector<int> route(city);
  for (int i = 0; i < individual_num; i++) {
    // 选取区间
    route = individuals[i];
    int pos1 = u(e);
    int pos2 = u(e);
    if (pos2 < pos1) {
      int temp = pos1;
      pos1 = pos2;
      pos2 = temp;
    }
    cnt = (pos2 - pos1 + 1) / 2;
    for (int j = 0; j < cnt; j++) {
      int temp = route[pos1 + j];
      route[pos1 + j] = route[pos2 - j];
      route[pos2 - j] = temp;
    }
    if (get_fitness(route) > get_fitness(individuals[i])) {
      individuals[i] = route; // 进化
      individuals_t[i] = route;
    }
  }
}

QString Genetic::output() {
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
  QString res = QString("（遗传算法，%1）最短环路距离：%2\n最短环路：")
                    .arg(str)
                    .arg(best_aim[index]);
  for (int i = 0; i < city; i++) {
    res.append(QString("%1, ").arg(best_route[index][i] + 1));
  }
  return res;
}

bool Genetic::isExist(int c, vector<int> t) const {
  for (int i = 0; i < t.size(); i++) {
    if (t[i] == c)
      return true;
  }
  return false;
}

bool Genetic::isError(vector<int> &individual) const {
  vector<bool> visit(city, false);
  for (int i = 0; i < city; i++) {
    if (visit[individual[i]])
      return true;
    else
      visit[individual[i]] = true;
  }
  return false;
}

vector<vector<int>> *Genetic::get_route() { return &best_route; }

vector<double> *Genetic::get_best_aim() { return &best_aim; }

vector<double> *Genetic::get_avg_aim() { return &avg_aim; }

Genetic::~Genetic() = default;
