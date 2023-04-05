//
// Created by 余玮 on 2023/4/1.
//

#include "ant_colony.h"
#include<iostream>
#include<cmath>
#include<QDebug>
#include<random>
#include<QString>

using namespace std;

ant_colony::ant_colony(int city, int ants, int max_iter, int Q, float alpha, float beta, float rho)
:city(city),ants(ants),max_iter(max_iter),Q(Q),alpha(alpha),beta(beta),rho(rho){
}

void ant_colony::init() {
    if(city==48)position=att48_position;
    else if(city==70)position=st70_position;
    else cout<<"城市数量异常！";
    //城市距离初始化
    //cout<<position[47][0];
    //qDebug()<<position[47][0];
    distance.resize(city);
    for(int i=0;i<city;i++)distance[i].resize(city);
    for(int i=0;i<city;i++){
        for(int j=i+1;j<city;j++){
            distance[i][j]=sqrt(pow(position[i][0]-position[j][0],2)
                    +pow(position[i][1]-position[j][1],2));
            distance[j][i]=distance[i][j];
        }
    }
    //信息素矩阵
    message.resize(city);
    for(int i=0;i<city;i++)message[i].assign(city,1);
    best_route.resize(max_iter);
    for(int i=0;i<max_iter;i++)best_route[i].resize(city);
    avg_aim.resize(max_iter);
    best_aim.resize(max_iter);
}
void ant_colony::run(){
    //使用随机数引擎
    static default_random_engine e(0);
    static uniform_int_distribution<signed> u(0,city-1);
    for(int count=0;count<max_iter;count++){
        vector<vector<int>> route(ants,vector<int>(city));//存放每只蚂蚁的路径
        //随机生成初始城市
        vector<vector<bool>> visit(ants,vector<bool>(city));//访问矩阵
        for(int i=0;i<ants;i++){
            route[i][0]=u(e);
            visit[i][route[i][0]]=true;
        }
        //每只蚂蚁按概率函数选择下一座城市
        for(int j=1;j<city;j++){
            for(int i=0;i<ants;i++){
                vector<double> p(city);//概率矩阵
                for(int k=0;k<city;k++){
                    if(!visit[i][k]){//未访问过
                        p[k]=pow(message[route[i][j-1]][k],alpha)
                                *pow(1/distance[route[i][j-1]][k],beta);
                    }else p[k]=0;
                }
                double total=0;
                for(int k=0;k<city;k++){
                    total+=p[k];
                }
                int select=-1;
                static uniform_real_distribution<double> u1(0,1);
                while(select==-1){//轮盘赌决定下一个城市
                    double ps=u1(e);
                    for(int k=0;k<city;k++){
                        if(!visit[i][k]&&ps<=p[k]/total){
                            select=k;
                            break;
                        }
                    }
                }
                route[i][j]=select;
                visit[i][select]=true;
            }
        }
        //记录本次迭代最佳路线
        vector<double> len(ants);
        double best=INF,sum=0;
        int index;
        for(int i=0;i<ants;i++) {
            for (int j=0;j<city-1;j++) {
                len[i]+=distance[route[i][j]][route[i][j+1]];
            }
            len[i]+=distance[route[i][city-1]][route[i][0]];
            sum+=len[i];
            if(len[i]<best){
                best=len[i];index=i;
            }
        }
        //最佳和平均目标
        best_aim[count]=best;
        avg_aim[count]=sum/ants;
        for(int i=0;i<city;i++){
            best_route[count][i]=route[index][i];
        }
        //best_route[count]=route[index];
        //更新全局信息素
        for(int i=0;i<city;i++){
            for(int j=0;j<city;j++){
                message[i][j]*=(1-rho);
            }
        }
        for(int i=0;i<city-1;i++){
            message[best_route[count][i]][best_route[count][i+1]]+=rho*(double)Q/best;
        }//在整个路径上的信息素增量
        message[best_route[count][city-1]][best_route[count][0]]+=rho*(double)Q/best;
    }
    output();
}
void ant_colony::output(){
    //找到最短路径
    int index=0;
    double best=best_aim[0];
    for(int i=1;i<max_iter;i++){
        if(best_aim[i]<best){
            best=best_aim[i];
            index=i;
        }
    }
    //输出结果
    cout<<"我的最短环路距离："<<best_aim[index]<<endl;
    cout<<"我的最短环路：";
    for(int i=0;i<city;i++){
        cout<<best_route[index][i]<<"->";
    }
}
