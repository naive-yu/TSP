#include "Config.h"

Config::Config(const QString &path) : settings_(path, QSettings::IniFormat) {}

AntParams Config::getAntParams(int city) const {
  AntParams p;
  QString base = QStringLiteral("Ant_colony/");
  QString sect = base + "default";
  p.ants = settings_.value(sect + "/ants", p.ants).toInt();
  p.max_iter = settings_.value(sect + "/max_iter", p.max_iter).toInt();
  p.Q = settings_.value(sect + "/Q", p.Q).toInt();
  p.alpha = settings_.value(sect + "/alpha", p.alpha).toDouble();
  p.beta = settings_.value(sect + "/beta", p.beta).toDouble();
  p.rho = settings_.value(sect + "/rho", p.rho).toDouble();

  QString citySect = base + QString::number(city);
  if (settings_.contains(citySect + "/ants"))
    p.ants = settings_.value(citySect + "/ants").toInt();
  if (settings_.contains(citySect + "/max_iter"))
    p.max_iter = settings_.value(citySect + "/max_iter").toInt();
  if (settings_.contains(citySect + "/Q"))
    p.Q = settings_.value(citySect + "/Q").toInt();
  if (settings_.contains(citySect + "/alpha"))
    p.alpha = settings_.value(citySect + "/alpha").toDouble();
  if (settings_.contains(citySect + "/beta"))
    p.beta = settings_.value(citySect + "/beta").toDouble();
  if (settings_.contains(citySect + "/rho"))
    p.rho = settings_.value(citySect + "/rho").toDouble();
  return p;
}

GeneticParams Config::getGeneticParams(int city) const {
  GeneticParams p;
  QString base = QStringLiteral("Genetic/");
  QString sect = base + "default";
  p.individual_num =
      settings_.value(sect + "/individual_num", p.individual_num).toInt();
  p.max_iter = settings_.value(sect + "/max_iter", p.max_iter).toInt();
  p.mutate_prob =
      settings_.value(sect + "/mutate_prob", p.mutate_prob).toDouble();

  QString citySect = base + QString::number(city);
  if (settings_.contains(citySect + "/individual_num"))
    p.individual_num = settings_.value(citySect + "/individual_num").toInt();
  if (settings_.contains(citySect + "/max_iter"))
    p.max_iter = settings_.value(citySect + "/max_iter").toInt();
  if (settings_.contains(citySect + "/mutate_prob"))
    p.mutate_prob = settings_.value(citySect + "/mutate_prob").toDouble();
  return p;
}

ParticleParams Config::getParticleParams(int city) const {
  ParticleParams p;
  QString base = QStringLiteral("Particle/");
  QString sect = base + "default";
  p.particle_num =
      settings_.value(sect + "/particle_num", p.particle_num).toInt();
  p.max_iter = settings_.value(sect + "/max_iter", p.max_iter).toInt();
  p.w1 = settings_.value(sect + "/w1", p.w1).toDouble();
  p.w2 = settings_.value(sect + "/w2", p.w2).toDouble();
  p.c1 = settings_.value(sect + "/c1", p.c1).toDouble();
  p.c2 = settings_.value(sect + "/c2", p.c2).toDouble();

  QString citySect = base + QString::number(city);
  if (settings_.contains(citySect + "/particle_num"))
    p.particle_num = settings_.value(citySect + "/particle_num").toInt();
  if (settings_.contains(citySect + "/max_iter"))
    p.max_iter = settings_.value(citySect + "/max_iter").toInt();
  if (settings_.contains(citySect + "/w1"))
    p.w1 = settings_.value(citySect + "/w1").toDouble();
  if (settings_.contains(citySect + "/w2"))
    p.w2 = settings_.value(citySect + "/w2").toDouble();
  if (settings_.contains(citySect + "/c1"))
    p.c1 = settings_.value(citySect + "/c1").toDouble();
  if (settings_.contains(citySect + "/c2"))
    p.c2 = settings_.value(citySect + "/c2").toDouble();
  return p;
}
