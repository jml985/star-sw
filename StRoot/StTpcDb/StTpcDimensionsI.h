#ifndef __STTPCDIMENSIONSI__
#define __STTPCDIMENSIONSI__
#include <TObject.h>

class StTpcDimensionsI : public TObject {

  //Abstract base class defining accessors
public:

  virtual int   numberOfSectors()     const = 0;

  //TPC field cage parameters:
  virtual float ifcRadius()           const = 0;
  virtual float ofcRadius()           const = 0;
  virtual float tpcTotalLength()      const = 0;

  //TPC wheel parameters:
  virtual float wheelInnerRadius()    const = 0;
  virtual float wheelOuterRadius()    const = 0;
  virtual float wheelThickness()      const = 0;

  virtual float senseGasOuterRadius() const = 0;
  virtual float tpeaThickness()       const = 0;

  //TPC cathode parameters:
  virtual float cathodeInnerRadius()  const = 0;
  virtual float cathodeOuterRadius()  const = 0;
  virtual float cathodeThickness()    const = 0; 

ClassDef(StTpcDimensionsI,0)

};
#endif















