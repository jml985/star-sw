// $Id: TrackToJetIndex.cxx,v 1.2 2008/06/01 18:01:37 tai Exp $
#include "TrackToJetIndex.h"

ClassImp(TrackToJetIndex)

TrackToJetIndex::TrackToJetIndex(int ji, int ti, StDetectorId id) 
  : mJetIndex(ji)
  , mTrackIndex(ti)
  , mDetId(id)
  , mCharge(0)
  , mNhits(0)
  , mNhitsPoss(0)
  , mNhitsDedx(0)
  , mNhitsFit(0)
  , mNsigmaPion(0.0)
  , mTdca(0.0)
  , mTdcaz(0.0)
  , mTdcaxy(0.0)
  , metaext(0.0)
  , mphiext(0.0)
  , mdEdx(0.0)
{

}

