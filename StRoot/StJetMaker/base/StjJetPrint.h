// -*- mode: c++;-*-
// $Id: StjJetPrint.h,v 1.2 2008/08/02 19:22:47 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJETJETPRINT_H
#define STJETJETPRINT_H

#include "StjJetList.h"

#include <fstream>
#include <string>

namespace StSpinJet {

class StjJetPrint {

public:

  StjJetPrint() { }
  virtual ~StjJetPrint() { }

  void operator()(const StjJetList& jetList);

private:

  void print(const StjJet& jet);

};

}

#endif // STJETJETPRINT_H
