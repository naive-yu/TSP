//
// Created by 余玮 on 2023/4/14.
//

#ifndef TSP_HELPER_H
#define TSP_HELPER_H

#include "Ant_colony.h"
#include "Genetic.h"
#include<iostream>
#include<string>
#include<fstream>

using namespace std;
class Helper {
public:
    explicit Helper(int city);
    void Ant_run();
    void Gen_run();

private:
    string ant_file,gen_file;
    int city;
    static ofstream get_stream(string &file);
    static void close_stream(ofstream &f);
};


#endif //TSP_HELPER_H
