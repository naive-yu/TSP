//
// Created by 余玮 on 2023/4/1.
//

#ifndef TSP_ANT_COLONY_H
#define TSP_ANT_COLONY_H
#define INF 1000000
#include<string>
#include<vector>
#include"resource.h"

using namespace std;
class ant_colony {
public:
    ant_colony(int city,int ants,int max_iter,int Q,double alpha,double beta,double rho);
    ant_colony();
    void init();
    void run();
    void output();
    vector<vector<int>> get_position();
    vector<vector<int>> get_best_route();
    vector<double> get_avg_aim();
    vector<double> get_best_aim();
private:
    int city{},ants{},max_iter{},Q{};//蚂蚁个数、最大迭代次数、信息素增加强度系数
    double alpha{},beta{},rho{};//信息启发因子、期望启发因子、信息素挥发因子
    vector<vector<int>> position;//位置矩阵
    vector<vector<double>> distance;//距离矩阵
    vector<vector<double>> message;//信息素矩阵
    vector<vector<int>> best_route;//每次迭代最佳路径
    vector<double> avg_aim;//每次迭代的平均行程距离
    vector<double> best_aim;//每次迭代最短行程距离
};


#endif //TSP_ANT_COLONY_H
