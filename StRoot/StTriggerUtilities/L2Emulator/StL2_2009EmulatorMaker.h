//////////////////////////////////////////////////////////////////////////
//
//
// StL2_2009EmulatorMaker Jan Balewski  (Fall, 2008)
//
// Goal: execute all L2-algos used in 2009 
//
//////////////////////////////////////////////////////////////////////////

#ifndef STAR_StL2_2009EmulatorMaker
#define STAR_StL2_2009EmulatorMaker


#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StGenericL2Emulator2009.h"


// algos used in 2009 ...
class  L2jetAlgo2009;

class StL2_2009EmulatorMaker : public StMaker, public StGenericL2Emulator2009 {
private:
  L2jetAlgo2009* mL2jetAlgo2009;

  void  addTriggerList();
  bool  getTriggerData();

public:
  StL2_2009EmulatorMaker(const char *name="L2Emul2009");
  virtual         ~StL2_2009EmulatorMaker();
  virtual Int_t   InitRun(int runumber);
  virtual Int_t   Init();
  virtual Int_t   Make();
  virtual Int_t   Finish();
  virtual void    Clear(const Option_t* = "");

  ClassDef(StL2_2009EmulatorMaker,0)
};

#endif

// $Id: StL2_2009EmulatorMaker.h,v 1.1 2010/04/17 05:32:27 pibero Exp $
//
