/***************************************************************************
 *
 * $Id: StMcHit.hh,v 2.1 1999/11/19 19:06:33 calderon Exp $
 * $Log: StMcHit.hh,v $
 * Revision 2.1  1999/11/19 19:06:33  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.4  1999/09/24 01:23:16  fisyak
 * Reduced Include Path
 *
 * Revision 1.3  1999/09/23 21:25:51  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#ifndef StMcHit_hh
#define StMcHit_hh

#include <iostream.h>
#include "StDbUtilities/StGlobalCoordinate.hh"

class StMcTrack;
class g2t_hits_st;

class StMcHit : public StGlobalCoordinate {
public:
    StMcHit();
    StMcHit(const StThreeVectorF&,
	  float, float,
	  StMcTrack*);
    StMcHit(g2t_hits_st*);
    // StMcHit(const StSvtHit&);                  use default
    // const StMcHit & operator=(const StMcHit&);   use default
    virtual ~StMcHit();
    
    int operator==(const StMcHit&) const;
    int operator!=(const StMcHit&) const;
    

  // "Get" Methods

    virtual float                       dE() const;
    virtual float                       dS() const;
    virtual StMcTrack*                  parentTrack() const;	

  // "Set" Methods

    virtual void setdE(float);
    virtual void setdS(float);
    virtual void setParentTrack(StMcTrack*);
    
protected:
    float                mdE;
    float                mdS;
    StMcTrack*           mParentTrack;
};

ostream&  operator<<(ostream& os, const StMcHit&);

inline float StMcHit::dE() const { return mdE; }

inline float StMcHit::dS() const { return mdS; }

inline StMcTrack* StMcHit::parentTrack() const { return mParentTrack; }	

#endif

