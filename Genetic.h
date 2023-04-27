//
// Created by 余玮 on 2023/4/5.
//

#ifndef TSP_GENETIC_H
#define TSP_GENETIC_H
#define INF 1000000

#include<vector>
#include<random>
#include "resource.h"

using namespace std;
class Genetic {
public:
    Genetic();
    explicit Genetic(int city, int individual_num, int max_iter, int cross_num, double mutate_prob);
    void init();
    void run();
    QString output();
    vector<vector<int>> *get_route();
    vector<double> *get_best_aim();
    vector<double> *get_avg_aim();
    ~Genetic();
private:
    int city{},individual_num{},max_iter{},cross_num{};//城市数量、种群数量、最大迭代次数
    double mutate_prob{};//变异概率
    //vector<vector<int>> position;//位置矩阵
    const vector<vector<float>> *distance{};//距离矩阵
    vector<vector<int>> individuals;//种群
    vector<vector<int>> individuals_t;//种群
    vector<vector<int>> best_route;//每次迭代最佳路径
    vector<double> avg_aim;//每次迭代的平均行程距离
    vector<double> best_aim;//每次迭代最短行程距离
    vector<int> search(vector<int> &individual, vector<int> &temp) const;
    double get_fitness(vector<int> &individual);
    vector<double> get_fitness(vector<vector<int>> &all_individuals);
    bool isError(vector<int> &individual) const;
    bool isExist(int c,vector<int> t) const;
    void cross();
    void mutate();
    void select();
};


#endif //TSP_GENETIC_H
