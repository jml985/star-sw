/*!
 * \class StFgtA2CMaker 
 * \author S. Gliske, Sept 2011
 */

/***************************************************************************
 *
 * $Id: StFgtA2CMaker.h,v 1.1 2011/10/28 14:58:49 sgliske Exp $
 * Author: S. Gliske, Oct 2011
 *
 ***************************************************************************
 *
 * Description: Converts the ADC value to charge.  Currently, this involves
 *
 * 1) pedistal subtraction
 * 2) applying minimum threshold (both fixed and multiples of the pedistal st. err.)
 * 3) applies gains
 *
 * These steps are only applied for time bins matching the given time bin mask.
 * The maker can also remove raw hits from the StFgtEvent for time bins
 * that will not be used.  Eventually, this maker may also include
 * common mode noise correction.
 *
 * The time bin mask is a bit field, with the lowest bit being the
 * lowest time bin, e.g. 0x01 is the 0th time bin, while 0x10 is the 4th
 * time bin.
 *
 ***************************************************************************
 *
 * $Log: StFgtA2CMaker.h,v $
 * Revision 1.1  2011/10/28 14:58:49  sgliske
 * replacement to StFgtCorAdcMaker
 *
 *
 **************************************************************************/

#ifndef _ST_FGT_A2C_MAKER_H
#define _ST_FGT_A2C_MAKER_H

#include <string>
#include "StMaker.h"

#include "StRoot/StEvent/StFgtEvent/StFgtEvent.h"
#include "StRoot/StFgtPedMaker/StFgtPedReader.h"

class StFgtA2CMaker : public StMaker {
 public:
   // constructors
   StFgtA2CMaker( const Char_t* name = "fgtCorAdcMaker" );

   // default OK
   // StFgtA2CMaker(const StFgtA2CMaker&);

   // deconstructor
   virtual ~StFgtA2CMaker();

   // equals operator -- default OK
   // StFgtA2CMaker& operator=(const StFgtA2CMaker&);

   virtual Int_t Init();
   virtual Int_t Make();

   // modifiers
   void setPedReaderFile( const Char_t* filename );
   void setTimeBinMask( Short_t timeBinMask );
   void setAbsThres( Float_t thres );  // set to below -4096 to skip cut
   void setRelThres( Float_t thres );  // set to zero to skip cut

   void doRemoveOtherTimeBins(  Bool_t doIt );

 protected:
   // for accessing the data
   StFgtEvent* mFgtEventPtr;

   // for the ped reader
   StFgtPedReader *mPedReader;
   std::string mPedFile;

   // other parameters
   Short_t mTimeBinMask;
   Bool_t mDoRemoveOtherTimeBins;
   Float_t mAbsThres, mRelThres;

   // member functions
   Int_t subtrPeds();
   Int_t cutHits();

 private:   
   ClassDef(StFgtA2CMaker,1);

}; 

// inline functions

// deconstructor
inline StFgtA2CMaker::~StFgtA2CMaker(){ /* */ };

// modifiers
inline void StFgtA2CMaker::setTimeBinMask( Short_t timeBinMask ){ mTimeBinMask = timeBinMask; };
inline void StFgtA2CMaker::setPedReaderFile( const Char_t* filename ){ mPedFile = filename; };
inline void StFgtA2CMaker::setAbsThres( Float_t thres ){ mAbsThres = thres; };
inline void StFgtA2CMaker::setRelThres( Float_t thres ){ mRelThres = thres; };
inline void StFgtA2CMaker::doRemoveOtherTimeBins(  Bool_t doIt ){ mDoRemoveOtherTimeBins = doIt; };

#endif
