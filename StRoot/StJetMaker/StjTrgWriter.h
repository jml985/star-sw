// -*- mode: c++;-*-
// $Id: StjTrgWriter.h,v 1.2 2008/08/02 19:22:32 tai Exp $
#ifndef STJETTRGWRITER_H
#define STJETTRGWRITER_H

#include "StjTrgPassCondition.h"

#include <Rtypes.h>

class TDirectory;
class TTree;

#include <string>

class StjTrg;

class StjTrgWriter {

public:

  StjTrgWriter(const char *treeName, const char* treeTitle,
		 TDirectory* file, StjTrg* trg,
		 StjTrgPassCondition* fillCondition,
		 StjTrgPassCondition* passCondition)
    : _treeName(treeName), _treeTitle(treeName)
    , _file(file)
    , _trg(trg)
    , _fillCondition(fillCondition)
    , _passCondition(passCondition)
  { }
  virtual ~StjTrgWriter() { }

  virtual void Init();
  virtual void Make();
  virtual void Finish();

private:

  virtual TTree* createTree();
  virtual void createBranch_general(TTree* tree);
  virtual void createBranch_trgSpecific(TTree* tree)  = 0;

  virtual void fillBranch_general();
  virtual void fillBranch_trgSpecific() = 0;

  std::string _treeName;
  std::string _treeTitle;

  TDirectory* _file;
  TTree*      _tree;

  StjTrg* _trg;

  Int_t    _runNumber;
  Int_t    _eventId;
  Double_t _vertexZ;
  Int_t    _trigID;
  Double_t _prescale;
  Int_t    _passed;
  Int_t    _hard;
  Int_t    _soft;

  StjTrgPassCondition* _fillCondition;
  StjTrgPassCondition* _passCondition;

};

#endif // STJETTRGWRITER_H
