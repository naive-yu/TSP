#include "tspwindow.h"
#include "Ant_colony.h"
#include "Genetic.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

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
    //最佳：att48,33523.7085;st70,678.5975
    TSPWindow w;
//    double best=40000;
//    int b_ants=0,b_Q=0,b_alpha=0,b_beta=0,b_rho=0;
//    for(int ants=60;ants<=120;ants+=10){
//        for(int Q=100;Q<=10000;Q*=10){
//            for(int alpha=5;alpha<=20;alpha+=5){
//                for(int beta=1;beta<=8;beta++){
//                    for(int rho=1;rho<=4;rho++){
//                        Ant_colony antTsp=Ant_colony(48,ants,250,Q,alpha*0.1,beta,rho*0.1);
//                        antTsp.init();
//                        antTsp.run();
//                        if(best>antTsp.get_best_aim().back()){
//                            best=antTsp.get_best_aim().back();
//                            b_ants=ants;
//                            b_Q=Q;
//                            b_alpha=alpha;
//                            b_beta=beta;
//                            b_rho=rho;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    cout<<b_ants<<" "<<b_Q<<" "<<b_alpha<<" "<<b_beta<<" "<<b_rho<<endl;
//    cout<<best<<endl;
//    Ant_colony antTsp=Ant_colony(48,150,250,1000,0.5,4,0.1);
//    antTsp.init();
//    antTsp.run();
//    cout<<antTsp.get_best_aim().back()<<endl;
//    double best2=40000;
//    int b_individual_num,b_cross_num,b_mutate_prob;
//    for(int individual_num=150;individual_num<=200;individual_num+=50){
//        for(int cross_num=3;cross_num<=5;cross_num++){
//            for(int mutate_prob=2;mutate_prob<=5;mutate_prob++){
//                Genetic gen=Genetic(48,individual_num,250,cross_num,mutate_prob*0.1);
//                gen.init();
//                gen.run();
//                if(best2>gen.get_best_aim().back()){
//                    best2=gen.get_best_aim().back();
//                    b_individual_num=individual_num;
//                    b_cross_num=cross_num;
//                    b_mutate_prob=mutate_prob;
//                }
//            }
//        }
//    }
//    cout<<b_individual_num<<" "<<b_cross_num<<" "<<b_mutate_prob<<" "<<endl;
//    cout<<best2<<endl;
    //90 10000 10 4 0
    //100 1000 10 6 2:33600.56
    //80 1000 10 6 4:33710.99
    //110 1000 5 4 1:33523.7
    w.show();
    return a.exec();
}
