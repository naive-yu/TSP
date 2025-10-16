#ifndef TSP_ANT_COLONY_H
#define TSP_ANT_COLONY_H
#define INF 1000000
#include <QString>
#include <vector>

using namespace std;
class Ant_colony {
public:
  Ant_colony(int city, int ants, int max_iter, int Q, double alpha, double beta,
             double rho);
  Ant_colony();
  void init();
  void run();
  QString output();
  vector<double> *get_avg_aim();
  vector<double> *get_best_aim();
  vector<vector<int>> *get_route();
  ~Ant_colony();

private:
  int city{}, ants{}, max_iter{},
      Q{};                       // 蚂蚁个数、最大迭代次数、信息素增加强度系数
  double alpha{}, beta{}, rho{}; // 信息启发因子、期望启发因子、信息素挥发因子
  // const vector<vector<int>> *position{};//位置矩阵
  const vector<vector<float>> *distance{}; // 距离矩阵
  vector<vector<double>> message;          // 信息素矩阵
  vector<vector<int>> best_route;          // 每次迭代最佳路径
  vector<double> avg_aim;                  // 每次迭代的平均行程距离
  vector<double> best_aim;                 // 每次迭代最短行程距离
};

#endif // TSP_ANT_COLONY_H
