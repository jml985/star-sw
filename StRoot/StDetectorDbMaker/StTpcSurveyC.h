#ifndef StTpcSurveyC_h
#define StTpcSurveyC_h

#include "St_SurveyC.h"
#include "StEnumerations.h"
class StTpcInnerSectorPosition : public St_SurveyC {// Out part of sector to Super Sector
 public:
  static StTpcInnerSectorPosition* 	instance();
  StTpcInnerSectorPosition(St_Survey *table=0) : St_SurveyC(table) {}
  virtual ~StTpcInnerSectorPosition() {fgInstance = 0;}
 private:
  static StTpcInnerSectorPosition* fgInstance;
  ClassDef(StTpcInnerSectorPosition,1) //C++ TChair for TpcInnerSectorPosition
};
class StTpcOuterSectorPosition : public St_SurveyC {// Out part of sector to Super Sector
 public:
  static StTpcOuterSectorPosition* 	instance();
  StTpcOuterSectorPosition(St_Survey *table=0) : St_SurveyC(table) {}
  virtual ~StTpcOuterSectorPosition() {fgInstance = 0;}
 private:
  static StTpcOuterSectorPosition* fgInstance;
  ClassDef(StTpcOuterSectorPosition,1) //C++ TChair for TpcOuterSectorPosition
};

class StTpcSuperSectorPosition : public St_SurveyC {// Extra rotation for whole Super Sector to Tpc
 public:
  static StTpcSuperSectorPosition* 	instance();
  StTpcSuperSectorPosition(St_Survey *table=0) : St_SurveyC(table) {}
  virtual ~StTpcSuperSectorPosition() {fgInstance = 0;}
 private:
  static StTpcSuperSectorPosition* fgInstance;
  ClassDef(StTpcSuperSectorPosition,1) //C++ TChair for TpcSuperSectorPosition
};

class StTpcHalfPosition : public St_SurveyC {// Extra rotation for whole Super Sector to Tpc
 public:
  static StTpcHalfPosition* 	instance();
  StTpcHalfPosition(St_Survey *table=0) : St_SurveyC(table) {}
  virtual ~StTpcHalfPosition() {fgInstance = 0;}
  const TGeoHMatrix  &GetEastMatrix() {return  GetMatrix(east);}
  const TGeoHMatrix  &GetWestMatrix() {return  GetMatrix(west);}
 private:
  static StTpcHalfPosition* fgInstance;
  ClassDef(StTpcHalfPosition,1) //C++ TChair for TpcHalfPosition
};
#endif
