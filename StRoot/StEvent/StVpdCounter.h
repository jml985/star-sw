/***************************************************************************
 *
 * $Id: StVpdCounter.h,v 1.1 1999/01/30 03:58:10 fisyak Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVpdCounter.h,v $
 * Revision 1.1  1999/01/30 03:58:10  fisyak
 * Root Version of StEvent
 *
 * Revision 1.4  1999/04/28 22:27:40  fisyak
 * New version with pointer instead referencies
 *
 * Revision 1.2  1999/01/15 22:54:25  wenaus
 * version with constructors for table-based loading
 *
 **************************************************************************/
#ifdef __ROOT__
#include "TObject.h"
#endif
class StVpdCounter : public TObject {
#include "StObject.h"
#include "StArray.h"
class StVpdCounter : public StObject {
public:
    StVpdCounter();
    StVpdCounter(Short_t id, Float_t a, Float_t t);
    ~StVpdCounter();
    // StVpdCounter(const StVpdCounter &right);
    // const StVpdCounter & operator=(const StVpdCounter &right);

    Short_t id() const;
    Float_t adc() const;
    Float_t time() const;

    void setId(Short_t);
    void setAdc(Float_t);
    void setTime(Float_t);
    
protected:
#ifdef __ROOT__
	ClassDef(StVpdCounter,1)  //StVpdCounter structure
#endif
    Float_t mAdc;

};
StCollectionDef(VpdCounter)

inline Short_t StVpdCounter::id() const { return mId; }

inline Float_t StVpdCounter::adc() const { return mAdc; }

inline Float_t StVpdCounter::time() const { return mTime; }

#endif
