//
// Created by 余玮 on 2023/4/13.
//

#ifndef TSP_PARTICLE_H
#define TSP_PARTICLE_H
#define INF 1000000
#include<string>
#include<vector>
#include<iostream>
#include <QString>
#include"resource.h"

using namespace  std;
class Particle {
public:
    Particle(int city, int particle, int max_iter, double w, double max_w, double min_w, double c1, double c2);
    Particle();
    void init();

    void run();
    QString output();
    vector<double> *get_avg_aim();
    vector<double> *get_best_aim();
    vector <vector<int>> *get_route();
    ~Particle();

private:
    int city{}, particle_num{}, max_iter{};//城市数、粒子个数、最大迭代次数
    double w{}, max_w{}, min_w{}, c1{}, c2{};//惯性因子、个体学习因子、社会学习因子
    const vector <vector<int>> *position{};//位置矩阵
    vector <vector<double>> distance;//距离矩阵
    vector<vector<int>> particles_best;
    vector<vector<int>> particles;
    double get_length(vector<int> &route);
    int find(vector<int> &t,int a) const;
    vector<double> get_length(vector<vector<int>> &total);
    vector <vector<int>> best_route;//每次迭代最佳路径
    vector<double> avg_aim;//每次迭代的平均行程距离
    vector<double> best_aim;//每次迭代最短行程距离
};

#endif //TSP_PARTICLE_H
//random_device rd;
//default_random_engine gen(rd());
//uniform_int_distribution<int> dis(0,city-1);