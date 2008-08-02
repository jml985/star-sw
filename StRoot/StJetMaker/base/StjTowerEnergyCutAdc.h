// -*- mode: c++;-*-
// $Id: StjTowerEnergyCutAdc.h,v 1.2 2008/08/02 19:22:50 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef TOWERENERGYCUTADC_H
#define TOWERENERGYCUTADC_H

#include "StjTowerEnergyCut.h"

namespace StJetTowerEnergyCut {

class StjTowerEnergyCutAdc : public StjTowerEnergyCut {

public:
  StjTowerEnergyCutAdc(int min = 0, double factor = 2.0)
    : _min(min), _factor(factor) { }
  virtual ~StjTowerEnergyCutAdc() { }

  bool operator()(const StSpinJet::StjTowerEnergy& tower)
  {
    if(tower.adc - tower.pedestal <= _min) return true;

    if(tower.adc - tower.pedestal <= _factor*tower.rms) return true;

    return false;
  }

private:

  int _min;
  double _factor;
};

}

#endif // TOWERENERGYCUTADC_H
