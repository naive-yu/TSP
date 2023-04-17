//
// Created by 余玮 on 2023/4/13.
//

#include "Particle.h"
#include <random>

Particle::Particle() = default;
Particle::Particle(int city, int particle_num, int max_iter, double w, double max_w, double min_w, double c1, double c2)
:city(city),particle_num(particle_num),max_iter(max_iter),w(w),max_w(max_w),min_w(min_w),c1(c1),c2(c2){
}

void Particle::init() {
    if(city==48)position=&att48_position;
    else if(city==70)position=&st70_position;
    else cout<<"城市数量异常！";
    distance.resize(city);
    for(int i=0;i<city;i++)distance[i].resize(city);
    for(int i=0;i<city;i++){
        for(int j=i+1;j<city;j++){
            distance[i][j]=sqrt(pow((*position)[i][0]-(*position)[j][0],2)
                                +pow((*position)[i][1]-(*position) [j][1],2));
            distance[j][i]=distance[i][j];
        }
    }
    particles_best=vector<vector<int>>(particle_num,vector<int>(city));
    particles=vector<vector<int>>(particle_num,vector<int>(city));
//    particles_best.resize(max_iter);
//    for(int i=0;i<max_iter;i++)particles_best[i].resize(city);
//    particles.resize(max_iter);
//    for(int i=0;i<max_iter;i++)particles[i].resize(city);
    best_route.resize(max_iter);
    for(int i=0;i<max_iter;i++)best_route[i].resize(city);
    avg_aim.resize(max_iter);
    best_aim.resize(max_iter);
}

void Particle::run() {
    random_device rd;
    default_random_engine e(rd());
    uniform_real_distribution<double> u1(0,1);
//    for(int j=0;j<particle_num;j++){
//        //依据贪心思路构建
//        vector<int> route(city);//路径矩阵
//        vector<bool> visit(city);//访问矩阵
//        uniform_int_distribution<signed> u(0,city-1);
//        route[0]=u(e);
//        visit[route[0]]=true;
//        particles[j][0]=route[0];
//        for(int i=1;i<city;i++){
//            //找到距离最近的城市
//            int index=-1;
//            double best=INF;
//            for(int k=0;k<city;k++){
//                if(!visit[k]&&distance[route[i-1]][k]<best){
//                    index=k;
//                    best=distance[route[i-1]][k];
//                }
//            }
//            route[i]=index;
//            visit[index]=true;
//        }
//        particles[j]=route;
//        particles_best[j]=route;//计算个体极值
//    }
    vector<int> route(city);
    for(int i=0;i<city;i++)route[i]=i;
    for(int i=0;i<particle_num;i++){
        particles[i]=route;
        particles_best[i]=route;
        shuffle(route.begin(),route.end(), std::mt19937(std::random_device()()));
    }
    //计算群体极值
    vector<double> lens= get_length(particles);
    int index=0;
    double mini=INF;
    for(int i=0;i<particle_num;i++){
        if(lens[i]<mini){
            index=i;
            mini=lens[i];
        }
    }
    best_route[0]=particles[index];
    best_aim[0]=mini;
    avg_aim[0]= accumulate(lens.begin(),lens.end(),0.0)/particle_num;
    vector<vector<int>> v(particle_num,vector<int>(city));
    for(int i=0;i<particle_num;i++){
        v[i]=route;
        shuffle(route.begin(),route.end(), std::mt19937(std::random_device()()));
    }
    vector<vector<int>> change1(particle_num,vector<int>(city));
    vector<vector<int>> change2(particle_num,vector<int>(city));
    for(int cnt=1;cnt<max_iter;cnt++){
        //更新惯性因子
        w=max_w-(max_w-min_w)*pow((cnt/(0.0+max_iter)),2);
        //更新速度
        //个体极值修正部分
        vector<int> p;
        vector<int> b;
        for(int i=0;i<particle_num;i++){
            p=particles[i];
            b=particles_best[i];
            for(int j=0;j<city;j++){
                change1[i][j]=find(p,b[j]);
                int temp=b[j];
                b[j]=p[change1[i][j]];
                p[change1[i][j]]=temp;
                if(u1(e)>c1)change1[i][j]=0;//一定概率保留原交换序列

            }
        }
        //群体极值修正部分
        vector<int> gb=best_route[cnt-1];
        for(int i=0;i<particle_num;i++){
            p=particles[i];
            for(int j=0;j<city;j++){
                change2[i][j]=find(p,gb[j]);
                int temp=gb[j];
                gb[j]=p[change2[i][j]];
                p[change2[i][j]]=temp;
                if(u1(e)>c2)change2[i][j]=0;//一定概率保留原交换序列

            }
        }
        //原速度部分
        for(int i=0;i<particle_num;i++){
            for(int j=0;j<city;j++){
                if(u1(e)>w)v[i][j]=0;
                if(change1[i][j]!=0)v[i][j]=change1[i][j];
                if(change2[i][j]!=0)v[i][j]=change2[i][j];
            }
        }
        //更新位置
        for(int i=0;i<particle_num;i++){
            for(int j=0;j<city;j++){
                if(v[i][j]!=0){
                    int temp=particles[i][j];
                    particles[i][j]=particles[i][v[i][j]];
                    particles[i][v[i][j]]=temp;
                }
            }
        }
        //个体极值和群体极值更新
        lens= get_length(particles);
        for(int i=0;i<particle_num;i++){
            if(lens[i]<get_length(particles_best[i]))particles_best[i]=particles[i];
        }
        index=0;
        mini=INF;
        for(int i=0;i<particle_num;i++){
            if(lens[i]<mini){
                index=i;
                mini=lens[i];
            }
        }
        best_route[cnt]=particles[index];
        best_aim[cnt]=mini;
        avg_aim[cnt]= accumulate(lens.begin(),lens.end(),0.0)/particle_num;
    }
}


int Particle::find(vector<int> &t,int a) const{
    for(int i=0;i<city;i++){
        if(t[i]==a)return i;
    }
    return -1;
}

QString Particle::output() {
    //找到最短路径
    int index = 0;
    double best = best_aim[0];
    for (int i = 1; i < max_iter; i++) {
        if (best_aim[i] < best) {
            best = best_aim[i];
            index = i;
        }
    }
    //输出结果
    //cout << "粒子群优化算法：我的最短环路距离：" << best_aim[index] << endl;
    //cout << "我的最短环路：";
//    for (int i = 0; i < city; i++) {
//        cout << best_route[index][i]+1 << "->";
//    }
//    cout << endl;
    QString res=QString("粒子群优化算法：我的最短环路距离：%1\n我的最短环路：").arg(best_aim[index]);
    for (int i = 0; i < city; i++) {
        res.append(QString("%1->").arg(best_route[index][i]+1));
    }
    return res;
}

vector<double> *Particle::get_avg_aim() {
    return &avg_aim;
}

vector<double> *Particle::get_best_aim() {
    return &best_aim;
}

vector<vector<int>> *Particle::get_route() {
    return &best_route;
}

double Particle::get_length(vector<int> &route) {
    double len=distance[route[city-1]][route[0]];
    for(int i=0;i<city-1;i++){
        len+=distance[route[i]][route[i+1]];
    }
    return len;
}

vector<double> Particle::get_length(vector<vector<int>> &total) {
    vector<double> res;
    for(vector<int> &route:total){
        double len=distance[route[city-1]][route[0]];
        for(int i=0;i<city-1;i++){
            len+=distance[route[i]][route[i+1]];
        }
        res.push_back(len);
    }
    return res;
}

Particle::~Particle() = default;
