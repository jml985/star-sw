/***************************************************************************
 *
 * $Id: StZdcSummary.h,v 1.5 1999/04/30 13:16:31 fisyak Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StZdcSummary.h,v $
 * Revision 1.5  1999/04/30 13:16:31  fisyak
 * add StArray for StRootEvent
 *
 * Revision 1.5  1999/04/30 13:16:31  fisyak
 * add StArray for StRootEvent
 *
 * Revision 1.4  1999/04/28 22:27:41  fisyak
 * New version with pointer instead referencies
 *
 * Revision 1.2  1999/01/15 22:54:30  wenaus
 * version with constructors for table-based loading
 *
 **************************************************************************/
#ifndef StZdcSummary_hh
#define StZdcSummary_hh
#include "StObject.h"
#include "StEnumerations.h"

class StZdcSummary : public StObject {
public:
    StZdcSummary();
    StZdcSummary(Float_t ae, Float_t aw, Float_t s);
    ~StZdcSummary();
    // StZdcSummary(const StZdcSummary &right);
    // const StZdcSummary & operator=(const StZdcSummary &right);

    Float_t adcSum() const;
    Float_t adcSum(StBeamDirection) const;

    void setAdcSum(StBeamDirection, Float_t); 
    void setAdcSum(Float_t);     
    
protected:
    Float_t mAdcSumEast;
    Float_t mAdcSumWest;
    Float_t mAdcSum;        
  ClassDef(StZdcSummary,1)  //StZdcSummary structure
};

inline Float_t StZdcSummary::adcSum() const { return mAdcSum; }

inline Float_t StZdcSummary::adcSum(StBeamDirection dir) const
{
    if (dir == east)
	return mAdcSumEast;
    else
	return mAdcSumWest;
}

#endif
