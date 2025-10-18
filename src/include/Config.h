#pragma once
#include <QSettings>
#include <QString>

struct AntParams {
  int ants;
  int max_iter;
  int Q;
  double alpha;
  double beta;
  double rho;
};

struct GeneticParams {
  int individual_num;
  int max_iter;
  double mutate_prob;
};

struct ParticleParams {
  int particle_num;
  int max_iter;
  double w1;
  double w2;
  double c1;
  double c2;
};

class Config {
public:
  explicit Config(const QString &path);
  AntParams getAntParams(int city) const;
  GeneticParams getGeneticParams(int city) const;
  ParticleParams getParticleParams(int city) const;

private:
  QSettings settings_;
};
