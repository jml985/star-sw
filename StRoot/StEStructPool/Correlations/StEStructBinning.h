/**********************************************************************
 *
 * $Id: StEStructBinning.h,v 1.1 2003/10/15 18:20:46 porter Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  Binning defs as c-structs for 2pt-analysis
 *
 *
 ***********************************************************************/
#ifndef __STEBYEBINNING__H
#define __STEBYEBINNING__H

#include <math.h>
/*
 *  I made these c-structs as floats in order to able to have them contain
 *  more than just the counts. If wts are added I need another word
 *  and it seems to me to be safer to do it in the cstruct rather than
 *  in another separate array.
 *
 */


// 25 + 1 over&under
#define EBYE_PHI_BINS 26
#define EBYE_ETA_BINS 26
#define EBYE_MT_BINS 40
#define EBYE_DELTAMT_BINS 26
#define EBYE_YT_BINS 28

#define EBYE_DPHI_BINS 26
#define EBYE_DETA_BINS 26
#define EBYE_DMT_BINS 26

#define EBYE_SPHI_BINS 26
#define EBYE_SETA_BINS 26
#define EBYE_SMT_BINS 51

struct phiBins {
  float phi[EBYE_PHI_BINS]; 
};
struct dphiBins {
  float dphi[EBYE_DPHI_BINS]; 
};
struct sphiBins {
  float sphi[EBYE_SPHI_BINS]; 
};

struct etaBins {
  float eta[EBYE_ETA_BINS];
};
struct detaBins {
  float deta[EBYE_DETA_BINS];
};
struct setaBins {
  float seta[EBYE_SETA_BINS];
};


struct mtBins {
  float mt[EBYE_MT_BINS]; 
};
struct dmtBins {
  float dmt[EBYE_DMT_BINS]; 
};
struct smtBins {
  float smt[EBYE_SMT_BINS]; 
};

struct deltaMtBins {
  float dmt[EBYE_DELTAMT_BINS];
};


class StEStructBinning {

protected:

  float maxPhi, minPhi, dPhi;    //! phi bins
  float maxEta, minEta, dEta;    //! eta bins
  float maxMt, minMt, dMt;       //! mt (x) bins
  int   nPhi, nEta, nMt;         //! n-bins

  float maxDPhi, minDPhi, dDPhi; //! delta phi bins
  float maxDEta, minDEta, dDEta; //! delta eta bins
  float maxDMt, minDMt, dDMt;    //! delta mt (x) bins
  int   nDPhi, nDEta, nDMt;         //! n-bins

  float maxSPhi, minSPhi, dSPhi; //! sigma phi bins
  float maxSEta, minSEta, dSEta; //! sigma eta bins
  float maxSMt, minSMt, dSMt;    //! sigma mt (x) bins
  int   nSPhi, nSEta, nSMt;         //! n-bins
 
  float maxDeltaMt, minDeltaMt, dDeltaMt; //! really mt 
  int   nDeltaMt;                   //! n-bins

  int i,j;

  StEStructBinning();
  StEStructBinning(StEStructBinning& me){};
  static StEStructBinning* mInstance;

public:  

  ~StEStructBinning(){};
  static StEStructBinning* Instance();

  int iphi(float phi);
  int ieta(float eta);
  int imt(float mt);

  float phiVal(int iphi);
  float etaVal(int ieta);
  float mtVal(int imt);

  int idphi(float phi);
  int ideta(float eta);
  int idmt(float mt);
  
  float dphiVal(int idphi);
  float detaVal(int ideta);
  float dmtVal(int idmt);

  int isphi(float phi);
  int iseta(float eta);
  int ismt(float mt);

  float sphiVal(int isphi);
  float setaVal(int iseta);
  float smtVal(int ismt);
  
  int   iDeltaMt(float mt);
  float deltaMtMax()    { return maxDeltaMt; }
  float deltaMtMin()    { return minDeltaMt; }
  float getBinWidthDeltaMt()  { return dDeltaMt; }
  int   deltaMtBins() { return nDeltaMt; }
  float deltaMtVal(int ideltaMt);


  float phiMax()   { return maxPhi; }
  float phiMin()   { return minPhi; }
  float getBinWidthPhi() { return dPhi; }
  int   phiBins() { return nPhi; }


  float etaMax()   { return maxEta; }
  float etaMin()   { return minEta; }
  float getBinWidthEta() { return dEta; }
  int   etaBins() { return nEta; };

  float mtMax()    { return maxMt; }
  float mtMin()    { return minMt; }
  float getBinWidthMt()  { return dMt; }
  int   mtBins() { return nMt; }

  float dphiMax()   { return maxDPhi; }
  float dphiMin()   { return minDPhi; }
  float getBinWidthDPhi() { return dDPhi; }
  int   dphiBins() { return nDPhi; }

  float detaMax()   { return maxDEta; }
  float detaMin()   { return minDEta; }
  float getBinWidthDEta() { return dDEta; }
  int   detaBins() { return nDEta; };

  float dmtMax()    { return maxDMt; }
  float dmtMin()    { return minDMt; }
  float getBinWidthDMt()  { return dDMt; }
  int   dmtBins() { return nDMt; }

  float sphiMax()   { return maxSPhi; }
  float sphiMin()   { return minSPhi; }
  float getBinWidthSPhi() { return dSPhi; }
  int   sphiBins() { return nSPhi; }

  float setaMax()   { return maxSEta; }
  float setaMin()   { return minSEta; }
  float getBinWidthSEta() { return dSEta; }
  int   setaBins() { return nSEta; };

  float smtMax()    { return maxSMt; }
  float smtMin()    { return minSMt; }
  float getBinWidthSMt()  { return dSMt; }
  int   smtBins() { return nSMt; }

};

inline StEStructBinning* StEStructBinning::Instance(){
  if(!mInstance) mInstance=new StEStructBinning;
  return mInstance;
}

inline int StEStructBinning::iphi(float phi){
  return ( (j=(int)((phi-minPhi)/dPhi))<0 || j>EBYE_PHI_BINS-2) ? (int)EBYE_PHI_BINS-1 : j;
}

inline int StEStructBinning::idphi(float phi){
  if(phi<minDPhi)phi+=2*M_PI;
  return ((j=(int)((phi-minDPhi)/dDPhi))<0 || j>EBYE_DPHI_BINS-2) ? (int)EBYE_DPHI_BINS-1 : j;
}

inline int StEStructBinning::isphi(float phi){
  return ((j=(int)((phi-minSPhi)/dSPhi))<0 || j>EBYE_SPHI_BINS-2) ? (int)EBYE_SPHI_BINS-1  : j;
}

inline float StEStructBinning::phiVal(int iphi){
  return minPhi+iphi*dPhi+dPhi/2;
};

inline float StEStructBinning::sphiVal(int isphi){
  return minSPhi+isphi*dSPhi+dSPhi/2;
};

inline float StEStructBinning::dphiVal(int idphi){
  return minDPhi+idphi*dDPhi+dDPhi/2;
};


inline int StEStructBinning::ieta(float eta){
  return ((j=(int)((eta-minEta)/dEta))<0 || j>EBYE_ETA_BINS-2) ? (int)EBYE_ETA_BINS-1 : j;  
}

inline int StEStructBinning::ideta(float eta){
  return ((j=(int)((eta-minDEta)/dDEta))<0 || j>EBYE_DETA_BINS-2) ? (int)EBYE_DETA_BINS-1 : j;
}

inline int StEStructBinning::iseta(float eta){
  return ((j=(int)((eta-minSEta)/dSEta))<0 || j>EBYE_SETA_BINS-2) ? (int)EBYE_SETA_BINS-1 : j;
  
}

inline float StEStructBinning::etaVal(int ieta){
  return minEta+ieta*dEta+dEta/2;
};

inline float StEStructBinning::setaVal(int iseta){
  return minSEta+iseta*dSEta+dSEta/2;
};

inline float StEStructBinning::detaVal(int ideta){
  return minDEta+ideta*dDEta+dDEta/2;
};



inline int StEStructBinning::imt(float mt){
  return ((j=(int)((mt-minMt)/dMt))<0 || j>EBYE_MT_BINS-2) ? (int)EBYE_MT_BINS-1 : j;  

}

inline int StEStructBinning::idmt(float mt){
  return ((j=(int)((mt-minDMt)/dDMt))<0 || j>EBYE_DMT_BINS-2) ? (int)EBYE_DMT_BINS-1 : j;
  
}
inline int StEStructBinning::ismt(float mt){
  return ((j=(int)((mt-minSMt)/dSMt))<0 || j>EBYE_SMT_BINS-2) ? (int)EBYE_SMT_BINS-1 : j;
  
}

inline float StEStructBinning::mtVal(int imt){
  return minMt+imt*dMt+dMt/2;
};

inline float StEStructBinning::smtVal(int ismt){
  return minSMt+ismt*dSMt+dSMt/2;
};

inline float StEStructBinning::dmtVal(int idmt){
  return minDMt+idmt*dDMt+dDMt/2;
};


inline int StEStructBinning::iDeltaMt(float mt){
  return ((j=(int)((mt-minDeltaMt)/dDeltaMt))<0 || j>EBYE_DELTAMT_BINS-2) ? (int)EBYE_DELTAMT_BINS-1 : j;
  
}

inline float StEStructBinning::deltaMtVal(int ideltaMt){
  return minDeltaMt+ideltaMt*dDeltaMt+dDeltaMt/2;
};

#endif

/***********************************************************************
 *
 * $Log: StEStructBinning.h,v $
 * Revision 1.1  2003/10/15 18:20:46  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/





