#pragma once
#include <vector>
#include <include/wheel_bot.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

struct ParticleLaserScanParams {
  float angle_min, angle_max, angle_increment, range_min, range_max;
};

struct MapParams {
  float resolution;
  int width, height;
  float origin_x, origin_y, origin_theta;
};

class ParticleFilter{

public:
  ParticleFilter();
  ParticleFilter(int nParticles);
  void setNParticles(int nParticles);
  void initializeLaserScanParameters(float angle_min, float angle_max,
                                     float angle_increment, float range_min,
                                     float range_max);
  void initializeMapParameters(float resolution,
                               int width, int height,
                               float origin_x, float origin_y, float origin_theta);
  void setMap(std::vector<int>& map);
  void spawnParticles();
  void show(int n);
  void extract_particle_local_scan(boost::shared_ptr<WheelBot>& particle, std::vector<float>& scanRanges);
  boost::shared_ptr<WheelBot> getParticle(int i);

private:

  void propagate();
  void perturb();
  void resample();

private:
  int nParticles_;
  std::vector<boost::shared_ptr<WheelBot>> particles_;
  ParticleLaserScanParams laserScanParams_;
  MapParams mapParams_;
  std::vector<int> map_data_;



};

struct ParticleVisualProperties{
  float length, width, height;
};

