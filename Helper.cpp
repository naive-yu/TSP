//
// Created by 余玮 on 2023/4/14.
//

#include "Helper.h"

Helper::Helper(int city):city(city) {
    if(city==48){
        ant_file="./ant_48.csv";
        gen_file="./genetic_48.csv";
    }else if(city==70){
        ant_file="./ant_70.csv";
        gen_file="./genetic_70.csv";
    }else cout<<"error"<<endl;
}

ofstream Helper::get_stream(string &file) {
    ofstream f_out;
    f_out.open(file,ios::app);
    return f_out;
}

void Helper::close_stream(ofstream &f){
    f.close();
}

void Helper::Ant_run() {
    //测试蚁群算法
    // att48大概4秒一次，共1260次，20:45开始,23:40结束
    // st70大概40秒一次，共1260次，0:0开始，15:00结束
    //新：att48大概00秒一次，共192*5=960次，
    double mini,maxi,avg,result;
    ofstream f_out=get_stream(ant_file);
    for(int ants=210;ants<=210;ants+=10){//140/210
        for(int Q=10;Q<=10000;Q*=10){//100~10000
            for(int alpha=4;alpha<=13;alpha+=3){//0.4~1.3
                for(int beta=3;beta<=6;beta++){//3~6
                    for(int rho=1;rho<=3;rho++){//0.1~0.3
                        f_out<<ants<<","<<Q<<","<<alpha*0.1<<","<<beta<<","<<rho*0.1<<",";
                        mini=INF,maxi=0,avg=0;
                         for(int i=0;i<5;i++){
                            Ant_colony antTsp=Ant_colony(city,ants,500,Q,alpha*0.1,beta,rho*0.1);
                            antTsp.run();
                            result=antTsp.get_best_aim()->back();
                            mini=min(mini,result);
                            maxi=max(maxi,result);
                            avg+=result;
                        }
                        avg/=5.0;
                        f_out<<maxi<<","<<mini<<","<<avg<<endl;
                    }
                }
            }
        }
    }
    close_stream(f_out);
}

void Helper::Gen_run() {
    //测试遗传算法
    double mini,maxi,avg,result;
    ofstream f_out= get_stream(gen_file);
    for(int individual_num=100;individual_num<=400;individual_num+=10){//90~180
        for (int mutate_prob = 1; mutate_prob <=4; mutate_prob++) {//0.05~0.2
            f_out<<individual_num<<","<<mutate_prob*0.05<<",";
            mini=INF,maxi=0,avg=0;
            for(int i=0;i<5;i++){
                Genetic gen = Genetic(48, individual_num, 2000, mutate_prob * 0.05);
                gen.run();
                result=gen.get_best_aim()->back();
                mini=min(mini,result);
                maxi=max(maxi,result);
                avg+=result;
            }
            avg/=5.0;
            f_out<<maxi<<","<<mini<<","<<avg<<endl;
        }
    }
    close_stream(f_out);
}

void Helper::Gen_run2() {
    //测试遗传算法
    double mini,maxi,avg,result;
    ofstream f_out= get_stream(gen_file);
    for(int individual_num=300;individual_num<=600;individual_num+=10){//90~180
        for (int mutate_prob = 1; mutate_prob <=4; mutate_prob++) {//0.05~0.2
            f_out<<individual_num<<","<<mutate_prob*0.05<<",";
            mini=INF,maxi=0,avg=0;
            for(int i=0;i<5;i++){
                Genetic gen = Genetic(70, individual_num, 2000, mutate_prob * 0.05);
                gen.run();
                result=gen.get_best_aim()->back();
                mini=min(mini,result);
                maxi=max(maxi,result);
                avg+=result;
            }
            avg/=5.0;
            f_out<<maxi<<","<<mini<<","<<avg<<endl;
        }
    }
    close_stream(f_out);
}

void Helper::Part_run(){
    //测试粒子群算法
    double mini,maxi,avg,result;
    ofstream f_out= get_stream(gen_file);
    for(int individual_num=300;individual_num<=600;individual_num+=10){
        for (int mutate_prob = 1; mutate_prob <=4; mutate_prob++) {
            f_out<<individual_num<<","<<mutate_prob*0.05<<",";
            mini=INF,maxi=0,avg=0;
            for(int i=0;i<5;i++){
                Particle pan= Particle(city,150,5000,1,0.8,0.4,0.3);
                pan.run();
                result=pan.get_best_aim()->back();
                mini=min(mini,result);
                maxi=max(maxi,result);
                avg+=result;
            }
            avg/=5.0;
            f_out<<maxi<<","<<mini<<","<<avg<<endl;
        }
    }
    close_stream(f_out);
}
