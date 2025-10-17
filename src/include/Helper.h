#pragma once
#include <fstream>
#include <string>

using namespace std;
class Helper {
public:
  explicit Helper(int city);
  void Ant_run();
  void Gen_run();
  void Gen_run2();
  void Part_run();
  void Part_run2();

private:
  string ant_file, gen_file, part_file;
  int city;
  static ofstream get_stream(string &file);
  static void close_stream(ofstream &f);
};
