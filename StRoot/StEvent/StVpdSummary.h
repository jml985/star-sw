/***************************************************************************
 *
 * $Id: StVpdSummary.h,v 1.1 1999/01/30 03:58:10 fisyak Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVpdSummary.h,v $
 * Revision 1.1  1999/01/30 03:58:10  fisyak
 * Root Version of StEvent
 *
 * Revision 1.3  1999/03/17 22:22:19  ullrich
 * Some cosmetics
 *
 * Revision 1.2  1999/01/15 22:54:26  wenaus
 * version with constructors for table-based loading
 *
#ifdef __ROOT__
#include "TObject.h"
#endif
#ifndef StVpdSummary_hh
#define StVpdSummary_hh
class StVpdSummary : public TObject {
#include "StEnumerations.h"

    StVpdSummary(Float_t ae, Float_t aw, Float_t s);
public:
    StVpdSummary();
    StVpdSummary(Float_t te, Float_t tw, Float_t z);
    ~StVpdSummary();
    // StVpdSummary(const StVpdSummary &right);
    // const StVpdSummary & operator=(const StVpdSummary &right);

    Float_t vertexZ() const;
    Float_t minimumTime(StBeamDirection) const;

    void setMinimumTime(StBeamDirection, Float_t); 
    void setVertexZ(Float_t);     
    
protected:
#ifdef __ROOT__
	ClassDef(StVpdSummary,1)  //StVpdSummary structure
#endif
    Float_t mMinimumTimeWest;
    Float_t mVertexZ;        
  ClassDef(StVpdSummary,1)  //StVpdSummary structure
};

inline Float_t StVpdSummary::vertexZ() const { return mVertexZ; }

inline Float_t StVpdSummary::minimumTime(StBeamDirection dir) const
{
    if (dir == east)
	return mMinimumTimeEast;
    else
	return mMinimumTimeWest;
}

#endif
