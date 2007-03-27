// $Id: StSsdBarrel.hh,v 1.3 2007/03/27 23:11:48 bouchet Exp $
//
// $Log: StSsdBarrel.hh,v $
// Revision 1.3  2007/03/27 23:11:48  bouchet
// Add a method to use the gain calibration for the Charge Matching between pulse of p and n sides
//
// Revision 1.2  2007/03/21 17:20:41  fisyak
// use TGeoHMatrix for coordinate transformation
//
// Revision 1.1  2006/10/16 16:43:29  bouchet
// StSsdUtil regroups now methods for the classes StSsdStrip, StSsdCluster and StSsdPoint
//
// Revision 1.8  2006/09/15 21:03:14  bouchet
// id_mctrack is using for setIdTruth and propagated to the hit
//
// Revision 1.7  2005/04/23 08:56:20  lmartin
// physics and pedestal data processing separated
//
// Revision 1.6  2005/03/22 13:45:01  lmartin
// new member mActiveLadders added
//
// Revision 1.5  2005/03/18 14:04:06  lmartin
// missing CVS header added
//

#ifndef STSSDBARREL_HH
#define STSSDBARREL_HH

#include "StSsdUtil/StSsdWafer.hh"
#include "StSsdUtil/StSsdLadder.hh"

class TFile;
class ssdDimensions_st;
class ssdConfiguration_st;
class ssdWafersPosition_st;
class slsCtrl_st;
class St_sls_strip;
class St_spa_strip;
class St_ssdWafersPosition;
class St_ssdStripCalib;
class St_ssdPedStrip;
class St_spa_strip;
class St_sdm_calib_db;
class St_sdm_condition_db;
class St_scf_cluster;
class St_scm_spt;
class St_ssdStripCalib;
class StSsdClusterControl;
class StSsdDynamicControl;
class StSsdHitCollection;
class St_ssdGainCalibWafer;

class StSsdBarrel
{
 public:
  StSsdBarrel(ssdDimensions_st  *dimensions, ssdConfiguration_st *config=0);
  ~StSsdBarrel();

  StSsdBarrel(const StSsdBarrel & originalBarrel);
  StSsdBarrel& operator=(const StSsdBarrel  originalBarrel);

  void  initLadders(St_ssdWafersPosition *wafpos);
  //   Int_t   readDeadStripFromTable(table_head_st *condition_db_h, sdm_condition_db_st *condition_db); 
  void  addNoiseToStrip(slsCtrl_st* ctrl); //
  Int_t readStripFromTable(St_spa_strip *spa_strip);
  Int_t readStripFromTable(St_sls_strip *sls_strip); //
  Int_t readNoiseFromTable(St_sdm_calib_db  *spa_noise, StSsdDynamicControl *dynamicControl);
  Int_t readNoiseFromTable(St_ssdStripCalib *strip_noise, StSsdDynamicControl *dynamicControl);
  Int_t readNoiseFromTable(St_ssdStripCalib *noise); //
  Int_t readConditionDbFromTable(St_sdm_condition_db *condition);//
  Int_t writeNoiseToFile(St_spa_strip *spa_strip);
  Int_t writeNoiseToFile(St_ssdPedStrip *pedStrip, char myLabel[]);
  Int_t readClusterFromTable(St_scf_cluster *scf_cluster);
  Int_t writeClusterToTable(St_scf_cluster *cluster);
  Int_t writeClusterToTable(St_scf_cluster *scf_cluster,St_spa_strip *spa_strip);
  Int_t writePointToContainer(St_scm_spt *scm_spt,StSsdHitCollection *ssdHitColl);
  Int_t writePointToContainer(St_scm_spt *scm_spt, StSsdHitCollection* ssdHitColl,St_scf_cluster *scf_cluster);    
  Int_t writeStripToTable(St_spa_strip * spa_strip); //
  Int_t writeStripToTable(St_spa_strip * spa_strip,St_sls_strip *sls_strip); //
  void  doSideClusterisation(Int_t *numberOfCluster);   
  Int_t doClusterMatching(Float_t CalibArray[320]);
  void  doDaqSimulation(slsCtrl_st* ctrl); //
  
  void  convertDigitToAnalog(StSsdDynamicControl *dynamicControl);
  void  convertGlobalFrameToOther();
  void  convertUFrameToOther();
  void  convertToStrip(Double_t pairCreationEnergy,
		       Int_t nstripInACluster,
		       Double_t parDiffP,
		       Double_t parDiffN,
		       Double_t parIndRightP,
		       Double_t parIndRightN,
		       Double_t parIndLeftP,
		       Double_t parIndLeftN
		       );
  void  sortListStrip();
  void  sortListCluster();
  Int_t getNumberOfLadders() { return mNLadder;}
  Int_t getNWaferPerLadder() { return mNWaferPerLadder;}
  Int_t getSsdLayer() { return mSsdLayer;}
  StSsdLadder *getLadder(Int_t i=0) {return mLadders[i];}
  ssdDimensions_st *getDimensions() {return mDimensions;}
  StSsdClusterControl *getClusterControl() {return mClusterControl;}
  Int_t isActiveLadder(Int_t i);
  void  debugUnPeu(Int_t monLadder, Int_t monwafer);
  void  setSsdParameters(ssdDimensions_st *geom_par);
  void  setLorentzShift(ssdDimensions_st *geom_par);
  void  setClusterControl(StSsdClusterControl *clusterControl) {mClusterControl = clusterControl;}
  void  initWafers(St_ssdWafersPosition *geom_class) {initLadders(geom_class);}
  void  renumHitAfterRemove();
  
  Int_t idWaferToWaferNumb(Int_t idWafer); //  idwafer = layer*1000+waf*100+ladder => waferNumb = mNWaferPerLadder*(ladder-1) + waf - 1
  Int_t idWaferToLadderNumb(Int_t idWafer);//  idwafer => ladder-1
  Int_t waferNumbToIdWafer(Int_t waferNumb);// waferNumb = mNWaferPerLadder*(ladder-1) + waf - 1 => idwafer
  Int_t idWaferToWafer(Int_t idWafer) {return (idWafer-7000)/100-1;}
  StSsdPointList *getInactiveHitList();
  void  Reset();
  void  SetDebug(Int_t k = 0) {mDebug = k;}
  Int_t Debug() {return mDebug;}
 private:
  Char_t   first[1];
 public:
  StSsdLadder** mLadders;
  static StSsdBarrel* Instance() {return fSsdBarrel;}
 private:
  Int_t    mSsdLayer;
  Int_t    mNLadder;
  Int_t    mNWaferPerLadder;
  Int_t    mNStripPerSide;
  Int_t    mActiveLadders[20];
  Float_t  mDetectorLargeEdge;
  Float_t  mDetectorSmallEdge;
  Float_t  mStripPitch;
  Float_t  mTheta;
  Float_t  mShift_hole;
  Float_t  mShift_elec;
  ssdDimensions_st *mDimensions;
  StSsdClusterControl *mClusterControl;

  Int_t    mDebug;
  Char_t   last[1];
  static   StSsdBarrel* fSsdBarrel;
};
#endif
