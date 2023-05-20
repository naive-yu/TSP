#include "tspwindow.h"
#include "Helper.h"
#include "Particle.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include<fstream>
#include<ctime>
#include "resource.h"

void test(){
    ofstream f_out;
    f_out.open("./test.csv",ios::app);
    double data1,data2,data3;
    int time1,time2,time3;
    clock_t start,end;
    f_out<<"bayg29"<<endl;
    for(int j=0;j<50;j++){
        Ant_colony ant=Ant_colony(29,120,500,10,0.4,5,0.1);
        Particle part=Particle(29,600,2000,1,0.7,0.6,0.05);
        Genetic gen=Genetic(29,280,2000,0.2);

        start=clock();
        ant.run();
        end=clock();
        time1=(end-start)/CLOCKS_PER_SEC;
        data1=ant.get_best_aim()->back();

        start=clock();
        part.run();
        end=clock();
        time2=(end-start)/CLOCKS_PER_SEC;
        data2=part.get_best_aim()->back();

        start=clock();
        gen.run();
        end=clock();
        time3=(end-start)/CLOCKS_PER_SEC;
        data3=gen.get_best_aim()->back();

        f_out<<data1<<","<<data2<<","<<data3<<","<<time1<<","<<time2<<","<<time3<<endl;
    }

    f_out<<"att48"<<endl;
    for(int j=0;j<50;j++){
        Ant_colony ant=Ant_colony(48,150,500,1000,0.4,5,0.1);
        Particle part=Particle(48,1000,2000,1,0.5,0.5,0.05);
        Genetic gen=Genetic(48,380,2000,0.2);

        start=clock();
        ant.run();
        end=clock();
        time1=(end-start)/CLOCKS_PER_SEC;
        data1=ant.get_best_aim()->back();

        start=clock();
        part.run();
        end=clock();
        time2=(end-start)/CLOCKS_PER_SEC;
        data2=part.get_best_aim()->back();

        start=clock();
        gen.run();
        end=clock();
        time3=(end-start)/CLOCKS_PER_SEC;
        data3=gen.get_best_aim()->back();

        f_out<<data1<<","<<data2<<","<<data3<<","<<time1<<","<<time2<<","<<time3<<endl;
    }

    f_out<<"st70"<<endl;
    for(int j=0;j<50;j++){
        Ant_colony ant=Ant_colony(70,210,500,100,0.4,6,0.2);
        Particle part=Particle(70,1800,2000,1,0.4,0.5,0.05);
        Genetic gen=Genetic(70,480,2000,0.15);

        start=clock();
        ant.run();
        end=clock();
        time1=(end-start)/CLOCKS_PER_SEC;
        data1=ant.get_best_aim()->back();

        start=clock();
        part.run();
        end=clock();
        time2=(end-start)/CLOCKS_PER_SEC;
        data2=part.get_best_aim()->back();

        start=clock();
        gen.run();
        end=clock();
        time3=(end-start)/CLOCKS_PER_SEC;
        data3=gen.get_best_aim()->back();

        f_out<<data1<<","<<data2<<","<<data3<<","<<time1<<","<<time2<<","<<time3<<endl;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TSP_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //初始化距离矩阵
//    vector<vector<float>> distance(29,vector<float>(29));
//    cout.precision(8);
//    for(int i=0;i<29;i++){
//        for(int j=i+1;j<29;j++){
//            distance[i][j]=(float)sqrt(pow(bayg29_position[i][0]-bayg29_position[j][0],2)
//                    +pow(bayg29_position[i][1]-bayg29_position[j][1],2));
//            distance[j][i]=distance[i][j];
//        }
//    }
//    for(int i=0;i<29;i++){
//        cout<<"{";
//        for(int j=0;j<28;j++){
//            cout<<distance[i][j]<<",";
//        }
//        cout<<distance[i][28]<<"},"<<endl;
//    }
    //最佳：att48,33523.7085;st70,678.5975
    //最佳：
//    Helper helper1=Helper(48);
//    helper1.Gen_run();
    //Helper helper2=Helper(70);
    //helper2.Gen_run2();
    //Helper hepler3=Helper(48);
    //hepler3.Part_run();
    //Helper hepler4=Helper(70);
    //hepler4.Part_run2();
    //无时针优化
    //29	280	0.2	2000	9795.17	9074.15	9369.62
    //48	380	0.2	2000	36543.6	33784	35032
    //70	480	0.15	2000	817.56	687.753	727.441
    //时针优化相关
//    vector<int> cites={29,48,70};
//    vector<int> individual_num={280,380,480};
//    vector<double> mutate_probs={0.2,0.2,0.15};
//    ofstream f_out;
//    f_out.open("./clock_opt.csv",ios::app);
//    double result,mini,maxi,avg;
//    for(int j=0;j<3;j++){
//        f_out<<cites[j]<<","<<individual_num[j]<<","<<mutate_probs[j]<<","<<2000<<",";
//        mini=INF,maxi=0,avg=0;
//        for(int i=0;i<20;i++){
//            Genetic gen = Genetic(cites[j], individual_num[j], 2000, mutate_probs[j]);
//            gen.run();
//            result=gen.get_best_aim()->back();
//            mini=min(mini,result);
//            maxi=max(maxi,result);
//            avg+=result;
//        }
//        avg/=20.0;
//        f_out<<maxi<<","<<mini<<","<<avg<<endl;
//    }
//    f_out.close();
    //具体参数多次实验
//    double mini = INF, maxi = 0, avg = 0,result;
//    for (int i = 0; i < 10; i++) {
//        Particle part =Particle(70,1800,2000,1,0.5,0.5,0.05);
//        part.run();
//        result = part.get_best_aim()->back();
//        mini = min(mini, result);
//        maxi = max(maxi, result);
//        avg += result;
//    }
//    avg /= 10.0;
//    cout << maxi << "," << mini << "," << avg << endl;
//    mini = INF, maxi = 0, avg = 0;
//    for (int i = 0; i < 10; i++) {
//        Particle part =Particle(70,1800,2000,1,0.4,0.5,0.05);
//        part.run();
//        result = part.get_best_aim()->back();
//        mini = min(mini, result);
//        maxi = max(maxi, result);
//        avg += result;
//    }
//    avg /= 10.0;
//    cout << maxi << "," << mini << "," << avg << endl;
    //test();
    TSPWindow w;
    w.show();
//    Particle p=Particle(48,150,400,1,1,0.8,0.1,0.075);
//    p.run();
//    p.output();
    //Genetic gen=Genetic(48, 100, 250, 0.1);
    //gen.init();
    //gen.run();
    //gen.output();
    //Helper helper0=Helper(29);
    //helper0.Ant_run();
//    helper0.Gen_run();
//    helper0.Part_run();
    //return 0;
    return a.exec();
}

