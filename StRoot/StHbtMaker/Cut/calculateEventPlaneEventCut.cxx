/***************************************************************************
 *
 * $Id: calculateEventPlaneEventCut.cxx,v 1.2 2001/07/21 12:04:28 rcwells Exp $
 *
 * Author: Randall Wells, Ohio State, rcwells@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description:  Passes HbtEvent to FlowMaker to calculate the event
 *     plane.  Warning ... this will change event charateristics!
 *
 ***************************************************************************
 *
 * $Log: calculateEventPlaneEventCut.cxx,v $
 * Revision 1.2  2001/07/21 12:04:28  rcwells
 * Only calls FlowAnalysis if using HbtEvent
 *
 * Revision 1.1  2001/07/20 20:03:50  rcwells
 * Added pT weighting and moved event angle cal. to event cut
 *
 *
 **************************************************************************/

#include "StHbtMaker/Cut/calculateEventPlaneEventCut.h"
#include <cstdio>
#include "StFlowMaker/StFlowMaker.h"
#include "StFlowMaker/StFlowEvent.h"
#include "StFlowAnalysisMaker/StFlowAnalysisMaker.h"
#include "StFlowMaker/StFlowSelection.h"

#ifdef __ROOT__
ClassImp(calculateEventPlaneEventCut)
#endif

calculateEventPlaneEventCut::calculateEventPlaneEventCut(){
  mFlowMaker = 0;
  mFlowAnalysisMaker = 0;
  mFromHBT = 0;
  mNEventsPassed =  mNEventsFailed = 0;
} 
//------------------------------
//calculateEventPlaneEventCut::~calculateEventPlaneEventCut(){
//  /* noop */
//}
//------------------------------
bool calculateEventPlaneEventCut::Pass(const StHbtEvent* event){
  cout << "calculateEventPlaneEventCut::Pass()" << endl;
  bool goodEvent = true;
  if (event) {
    if (mFromHBT) mFlowMaker->FillFlowEvent(event);
    if (mFlowMaker->FlowEventPointer()) {
      mFlowMaker->FlowEventPointer()->SetPtWgt(false);
      // First get RP for whole event
      mFlowMaker->FlowSelection()->SetSubevent(-1);
      double reactionPlane = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      cout << "Reaction Plane        " << reactionPlane << endl;
      event->SetReactionPlane(reactionPlane,0);
      // Sub event RPs
      mFlowMaker->FlowSelection()->SetSubevent(0);
      double RP1 = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      mFlowMaker->FlowSelection()->SetSubevent(1);
      double RP2 = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      event->SetReactionPlaneSubEventDifference(RP1-RP2,0);
      // Now with Pt Weighting
      mFlowMaker->FlowEventPointer()->SetPtWgt(true);
      // First get RP for whole event
      mFlowMaker->FlowSelection()->SetSubevent(-1);
      reactionPlane = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      cout << "Reaction Plane ptWgt  " << reactionPlane << endl;
      event->SetReactionPlane(reactionPlane,1);
      // Sub event RPs
      mFlowMaker->FlowSelection()->SetSubevent(0);
      RP1 = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      mFlowMaker->FlowSelection()->SetSubevent(1);
      RP2 = mFlowMaker->FlowEventPointer()->Psi(mFlowMaker->FlowSelection());
      event->SetReactionPlaneSubEventDifference(RP1-RP2,1);
      // if Flow Analysis is switched on ... make correction histogram
      if (mFromHBT && mFlowAnalysisMaker) mFlowAnalysisMaker->Make();
    }
    else {
      cout << "No flow event found" << endl;
      event->SetReactionPlane(-999,0);
      event->SetReactionPlane(-999,1);
      event->SetReactionPlaneSubEventDifference(-999,0);
      event->SetReactionPlaneSubEventDifference(-999,1);
    }
  }
  else {
    cout << "Something wrong with HbtEvent" << endl;
    event->SetReactionPlane(-999,0);
    event->SetReactionPlane(-999,1);
    event->SetReactionPlaneSubEventDifference(-999,0);
    event->SetReactionPlaneSubEventDifference(-999,1);
  }
  goodEvent ? mNEventsPassed++ : mNEventsFailed++ ;
  cout << "calculateEventPlaneEventCut:: return : " << goodEvent << endl;
  return (goodEvent);
}
//------------------------------
StHbtString calculateEventPlaneEventCut::Report(){
  string Stemp;
  char Ctemp[100];
  sprintf(Ctemp,"\nNumber of events which passed:\t%ld  Number which failed:\t%ld",mNEventsPassed,mNEventsFailed);
  Stemp += Ctemp;
  StHbtString returnThis = Stemp;
  return returnThis;
}

