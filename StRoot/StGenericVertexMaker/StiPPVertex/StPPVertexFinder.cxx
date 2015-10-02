/************************************************************
 *
 * $Id: StPPVertexFinder.cxx,v 1.46 2015/08/31 20:28:02 genevb Exp $
 *
 * Author: Jan Balewski
 ************************************************************
 *
 * Description:  does not fear any pileup
 *
 ************************************************************/
   
#include <StMessMgr.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TH2.h>
#include <TFile.h>
#include <TLine.h>
#include <TCanvas.h> //tmp

#include <math_constants.h>
#include <tables/St_g2t_vertex_Table.h> // tmp for Dz(vertex)

#include "StPPVertexFinder.h"
#include "TrackData.h"
#include "VertexData.h" 
#include "Vertex3D.h"
#include "StGenericVertexMaker.h"
#include "St_VertexCutsC.h"

#include <Sti/StiToolkit.h>
#include <Sti/StiKalmanTrack.h>
#include <Sti/StiKalmanTrackNode.h>
#include <Sti/StiTrackContainer.h>

#include <St_db_Maker/St_db_Maker.h>
#include <StIOMaker/StIOMaker.h> // to save  local histos 
#include <StBFChain/StBFChain.h>

#define xL(t)   (t->getX())
#define yL(t)   (t->getY())
#define eyL(t)  sqrt(t->getCyy())
#define zL(t)   (t->getZ())
#define ezL(t)  sqrt(t->getCzz())
#define rxyL(t) sqrt(xL(t)*xL(t) + yL(t)*yL(t)) 
#define xG(t)   (t->x_g())
#define yG(t)   (t->y_g())
#define zG(t)   (t->z_g())
#define rxyG(t) sqrt(xG(t)*xG(t) + yG(t)*yG(t)) 

#include <StEEmcUtil/database/StEEmcDb.h>
#include <StEEmcUtil/database/EEmcDbItem.h>
#include <StEEmcUtil/database/cstructs/eemcConstDB.hh>
#include <StEEmcUtil/EEmcGeom/EEmcGeomSimple.h>

#include "BtofHitList.h" // dongx
#include "CtbHitList.h"
#include "BemcHitList.h"
#include "EemcHitList.h"

#include "StEmcCollection.h"
#include "StBTofCollection.h" // dongx
#include "StBTofUtil/StBTofGeometry.h"
#include "TObjectSet.h"

//==========================================================
//==========================================================

StPPVertexFinder::StPPVertexFinder() {
  LOG_INFO << "StPPVertexFinder::StPPVertexFinder is in use" << endm;

  mdxdz=mdydz=mX0=mY0  = 0; // beam line params
  mTotEve              = 0;
  HList=0;
  mToolkit =0;
  memset(hA,0,sizeof(hA));

  setMC(false);                      // default = real Data
  UseCTB(true);                      // default CTB is in the data stream
  setDropPostCrossingTrack(true);    // default PCT rejection on
  mVertexOrderMethod = orderByRanking; // change ordering by ranking

  mAlgoSwitches=0; // default, as for 2008 pp data production

  //........... tune for W-boson reco
  mAlgoSwitches|=kSwitchOneHighPT;
  mCut_oneTrackPT=10; // GeV, used only if coresponding algoSwitch switch is ON.
  mBeamLineTracks=0; // expert only, activation via BFC 

  // special histogram for finding the vertex, not to be saved
  int nb=5000;
  float zRange=250;// (cm)
  hL=new TH1D("ppvL","Vertex likelyhood; Z /cm",nb,-zRange,zRange);
  // needed only for  better errZ calculation
  hM=new TH1D("ppvM","cumulative track multiplicity; Z /cm",nb,-zRange,zRange);
  hW=new TH1D("ppvW","cumulative track weight; Z /cm",nb,-zRange,zRange);

} 


//==========================================================
//==========================================================
void 
StPPVertexFinder::Init() {
  assert(mTotEve==0); // can't be called twice
  LOG_INFO << Form("PPV-algo  switches=0x%0x,  following cuts have been activated:",mAlgoSwitches)<<endm;
  //.. set various params 
  mStoreUnqualifiedVertex=5; // extension requested by Akio, October 2008, set to 0 do disable it
  mFitPossWeighting=false;          // default prior to 2012 

  //get pointer to Sti toolkit
  mToolkit = StiToolkit::instance();
  assert(mToolkit);          // internal error of Sti
  
  ctbList  = new CtbHitList;
  bemcList = new BemcHitList;
  btofList = new BtofHitList;
  vertex3D = 0; // default
  

  // access EEMC-DB
  eeDb = (StEEmcDb*)StMaker::GetChain()->GetDataSet("StEEmcDb"); 
  assert(eeDb); // eemcDB must be in the chain, fix it,JB
  LOG_INFO << "eeDb done" <<endm;
  geomE= new EEmcGeomSimple();
  // choose which 'stat' bits are fatal for mip detection
  uint  killStatEEmc=EEMCSTAT_ONLPED | EEMCSTAT_STKBT|  EEMCSTAT_HOTHT |  EEMCSTAT_HOTJP | EEMCSTAT_JUMPED ;
  eemcList =new EemcHitList(eeDb, killStatEEmc,geomE);
   
  HList=new TObjArray(0);   
  initHisto();
  LOG_INFO << "initiated histos" << endm;
  if (mUseBtof)
    btofList->initHisto( HList); // dongx
  ctbList->initHisto( HList);
  bemcList->initHisto( HList);
  eemcList->initHisto( HList);
  LOG_INFO << "Finished Init" << endm;
}

//==========================================================
//==========================================================
void 
StPPVertexFinder::InitRun(int runnumber){
  LOG_INFO << "PPV InitRun() runNo="<<runnumber<<endm;
  St_db_Maker* mydb = (St_db_Maker*) StMaker::GetChain()->GetMaker("db");
  int dateY=mydb->GetDateTime().GetYear();
  
  if(mIsMC) assert(runnumber <1000000); // probably embeding job ,crash it, JB
 // Initialize BTOF geometry - dongx
  if (mUseBtof){ // only add btof if it is required
    btofGeom = 0;
    TObjectSet *geom = (TObjectSet *) mydb->GetDataSet("btofGeometry");
    if (geom)   btofGeom = (StBTofGeometry *) geom->GetObject();
    if (btofGeom) {
      LOG_INFO << " Found btofGeometry ... " << endm;
    } else {
      btofGeom = new StBTofGeometry("btofGeometry","btofGeometry in VertexFinder");
      geom = new TObjectSet("btofGeometry",btofGeom);
      LOG_INFO << " Create a new btofGeometry ... " << endm;
      mydb->AddConst(geom);
    } 
    if(btofGeom && !btofGeom->IsInitDone()) {
      LOG_INFO << " BTofGeometry initialization ... " << endm;
      TVolume *starHall = (TVolume *)mydb->GetDataSet("HALL");
      btofGeom->Init(mydb, starHall);
    }
  }

  //.. set various params 
  if (!mIsMC && runnumber < 13000000) {
    // old defaults, pre-Run12
    // (important if we want to reprocess old data with different cuts!)
    LOG_INFO << "PPV InitRun() using old, hardwired cuts" << endm;
    mMaxTrkDcaRxy = 3.0;  // cm 
    mMinTrkPt     = 0.20; // GeV/c  //was 0.2 in 2005 prod
    mMinFitPfrac  = 0.7;  // nFit /nPossible points on the track
    mMaxZradius   = 3.0;  //+sigTrack, to match tracks to Zvertex
    mMinMatchTr   = 2;    // required to accept vertex
  } else {
    St_VertexCutsC* vtxCuts = St_VertexCutsC::instance();
    mMaxTrkDcaRxy = vtxCuts->RImpactMax();
    mMinTrkPt     = vtxCuts->MinTrackPt();
    mMinFitPfrac  = vtxCuts->MinFracOfPossFitPointsOnTrack();
    mMaxZradius   = vtxCuts->DcaZMax();  //+sigTrack, to match tracks to Zvertex
    mMinMatchTr   = vtxCuts->MinTrack();    // required to accept vertex
    mFitPossWeighting = true;
  }
  mMaxZrange    = 200;  // to accept Z_DCA of a track           
  mDyBtof       = 1.5;  // |dy|<1.5 cm for local position - not used now
  mMinZBtof     = -3.0; //
  mMaxZBtof     = 3.0;  // -3.0<zLocal<3.0 - dongx              
  mMinAdcEemc   = 5;    // chan, MIP @ 6-18 ADC depending on eta

  //assert(dateY<2008); // who knows what 2007 setup will be,  crash it just in case
  if(mIsMC) {
    LOG_INFO << "PPV InitRun() M-C, Db_date="<<mydb->GetDateTime().AsString()<<endm;
    // simu prior to 2008 
    mMinAdcBemc =7; //ideal BTOW gain 60 GeV ET @ 3500 ADC 
    // in late 2008 Matt uploaded ideal (aka sim) gians matching endcap
    if(dateY>=2008)  mMinAdcBemc =8; //ideal BTOW gain 60 GeV ET @ 4076 ADC

    if(dateY>2006)  LOG_WARN <<
	  "PPV InitRun() , M-C time stamp differs from 2005,\n BTOW status tables questionable,\n PPV results qauestionable, \n\n  F I X    B T O W    S T A T U S     T A B L E S     B E F O R E     U S E  !!!  \n \n chain will continue taking whatever is loaded in to DB\n  Jan Balewski, January 2006\n"<<endm; 
  }

  if(dateY<2006) {
    mMinAdcBemc   = 15;   // BTOW used calibration of maxt Et @ ~27Gev 
  } else {
    mMinAdcBemc   = 8;    // BTOW used calibration of maxt Et @ ~60Gev 
  }
  if (mUseBtof)
    btofList->initRun(); // dongx
  ctbList->initRun(); 
  bemcList->initRun();
  eemcList->initRun();
  
  if(mBeamLineTracks){
    assert(vertex3D==0); // crash means initRun was called twice - not foreseen,Jan B.
    vertex3D=new Vertex3D;
    vertex3D->setCuts(0.8,8.0, 3.3,5); // pT1(GeV), pT2, sigY(cm), nTr
    vertex3D->initHisto( HList);
    vertex3D->initRun();
  }

  //gMessMgr->Message("","I") 
  LOG_INFO 
    << "PPV::cuts "
    <<"\n MinNumberOfFitPointsOnTrack = unused"
    <<"\n MinFitPfrac=nFit/nPos  = " << mMinFitPfrac 
    <<"\n MaxTrkDcaRxy/cm= " << mMaxTrkDcaRxy
    <<"\n MinTrkPt GeV/c = " << mMinTrkPt
    <<"\n MinMatchTr of prim tracks = " << mMinMatchTr
    <<"\n MaxZrange (cm)for glob tracks = " << mMaxZrange
    <<"\n MaxZradius (cm) for prim tracks &Likelihood  = " << mMaxZradius
    <<"\n DeltaY (cm) for BTOF local posision = "<< mDyBtof
    <<"\n Min/Max Z position for BTOF hit = " << mMinZBtof<<" "<<mMaxZBtof   
    <<"\n MinAdcBemc for MIP = " << mMinAdcBemc
    <<"\n MinAdcEemc for MIP = " << mMinAdcEemc
    <<"\n bool    mIsMC = " << mIsMC
    <<"\n bool  useCtb = " << mUseCtb
    <<"\n bool useBtof = " << mUseBtof
    <<"\n bool nFit/nPoss weighting = " << mFitPossWeighting
    <<"\n bool DropPostCrossingTrack = " << mDropPostCrossingTrack
    <<"\n Store # of UnqualifiedVertex = " << mStoreUnqualifiedVertex
    <<"\n Store="<<(mAlgoSwitches & kSwitchOneHighPT) <<
               " oneTrack-vertex if track PT/GeV>"<< mCut_oneTrackPT 
    <<"\n dump tracks for beamLine study = " << mBeamLineTracks
    <<"\n"
    <<endm; 

}


//==========================================================
//==========================================================
void 
StPPVertexFinder::initHisto() {
  assert(HList);
  hA[0]=new TH1F("ppvStat","event types; 1=inp, 2=trg, 3=-, 4=w/trk, 5=anyMch, 6=Bmch 7=Emch 8=anyV, 9=mulV",10,0.5,10.5);
  hA[1]=new TH1F("ch1","chi2/Dof, ppv pool tracks",100,0,10);
  hA[2]=new TH1F("nP","No. of fit points, ppv pool tracks",30,-.5,59.5);
  hA[3]=new TH1F("zV","reconstructed vertices ; Z (cm)",100,-200,200);
  hA[4]=new TH1F("nV","No. of vertices per eve",20,-0.5,19.5);
  
  hA[5]=new TH1F("rxyDca","Rxy to beam @ DCA ; (cm)",40,-0.1,3.9);
  hA[6]=new TH1F("nTpcM","No. tracks: tpcMatch /eve ",60,-.5,59.5);
  hA[7]=new TH1F("nTpcV","No. tracks: tpcVeto /eve ",60,-.5,59.5);

  hA[8]=0; // (TH1F*) new TH2F ("xyE","Y vs. X  of match  tracks in EEMC; X (cm); Y(cm)",200,-250.,250,200,-250,250);

  hA[9]=new TH1F("zDca","Z DCA for all accepted tracks; Z (cm)",100,-200,200);
  
  hA[10]=new TH1F("zCtb","Z @CTB for all accepted tracks; Z (cm)",50,-250,250);  
  hA[11]=new TH1F("zBemc","Z @Bemc for all accepted tracks; Z (cm)",100,-400,400);
  hA[12]=new TH1F("dzVerTr","zVerGeant - zDca of tracks used by any vertex ; (cm)",100,-5,5);
  hA[13]=new TH1F("dzVerVer","zVerGeant - best reco vertex ; (cm)",100,-5,5);

  hA[14]=new TH1F("EzDca","Error of Z DCA for all accepted tracks; Z (cm)",100,-0.,4);
  hA[15]=new TH1F("nTpcT","No. tracks: accepted Dca /eve ",201,-.5,200.5);
  hA[16]=new TH1F("ptTr","pT, ppv pool tracks; pT (GeV/c) ",50,0.,10.);
  hA[17]=new TH1F("vRL","PPV Vertex rank, 'funny' X-axis; X=Log10(rank-1e6)+offset", 150, -11,25);

  hACorr=new TH2F("BTOFvsBEMC","BTOF vs BEMC", 5,-2.5,2.5,5,-2.5,2.5);

  int i;
  for(i=0;i<mxH; i++) if(hA[i]) HList->Add(hA[i]);
  HList->Add(hACorr);
}

//==========================================================
//==========================================================
void 
StPPVertexFinder::Clear(){
  LOG_DEBUG << "PPVertex::Clear nEve="<<mTotEve<<  endm;
  StGenericVertexFinder::Clear();
  btofList->clear();  // dongx
  ctbList->clear();
  bemcList->clear();
  eemcList->clear();
  mTrackData.clear();
  mVertexData.clear();
  eveID=-1;

  // the clear below is not needed but cleans up stale result
  hL->Reset();
  hM->Reset();
  hW->Reset();


}


//==========================================================
//==========================================================
StPPVertexFinder::~StPPVertexFinder() {
  //x delete mTrackData;
  //x delete mVertexData;
  delete geomE;
  //yf  if(btofGeom) delete btofGeom; // dongx 
}

//======================================================
//======================================================
void
StPPVertexFinder::printInfo(ostream& os) const
{
  os << "StPPVertexFinder ver=1 - Fit Statistics:" << endl;
  
  os << "StPPVertexFinder::result "<<mVertexData.size()<<" vertices found\n" << endl;

  int nTpcM=0, nTpcV=0;
  uint i;
  int k=0;
  for(i=0;i<mTrackData.size();i++) {
    const TrackData *t=&mTrackData[i];
    if(  t->mTpc>0)   nTpcM++;
    else if (  t->mTpc<0) nTpcV++;
    hA[9]->Fill(t->zDca);
    hA[14]->Fill(t->ezDca);
    if(t->vertexID<=0) continue; // skip not used or pileup vertex 
    k++;
    LOG_DEBUG 
      <<
      Form("%d track@z0=%.2f +/- %.2f gPt=%.3f vertID=%d match:  bin,Fired,Track:\n",
	   k,t->zDca,t->ezDca,t->gPt,t->vertexID) 
      << Form("    Btof %3d,%d,%d",t->btofBin,btofList->getFired(t->btofBin),btofList->getTrack(t->btofBin))   // dongx
      << Form("    CTB  %3d,%d,%d",t->ctbBin,ctbList->getFired(t->ctbBin),ctbList->getTrack(t->ctbBin))
      << Form("    Bemc %3d,%d,%d",t->bemcBin,bemcList->getFired(t->bemcBin),bemcList->getTrack(t->bemcBin))
      << Form("    Eemc %3d,%d,%d",t->eemcBin,eemcList->getFired(t->eemcBin),eemcList->getTrack(t->eemcBin))
      << Form("    TPC %d",t->mTpc)
      <<endm;
  }
  hA[6]->Fill(nTpcM);
  hA[7]->Fill(nTpcV);
  hA[15]->Fill(mTrackData.size());
  
  LOG_INFO<< Form("PPVend  eveID=%d,  list of found %d vertices from pool of %d tracks\n",eveID,mVertexData.size(),mTrackData.size())<<endm;
  for(i=0;i<mVertexData.size();i++) {
    const VertexData *V=& mVertexData[i];
    V->print(os);
  }
  float zGeant=999;
  
  if(mIsMC) {
    // get geant vertex
    St_DataSet *gds=StMaker::GetChain()->GetDataSet("geant");
    assert(gds);
    St_g2t_vertex  *g2t_ver=( St_g2t_vertex *)gds->Find("g2t_vertex");
    if(g2t_ver) {
      // --------------  A C C E S S    G E A N T   V E R T E X  (for histo)
      g2t_vertex_st *GVER= g2t_ver->GetTable();
      zGeant=GVER->ge_x[2];
      printf("#GVER z=%.1f x=%.1f y=%.1f  nEve=%d nV=%d ",zGeant,GVER->ge_x[0],GVER->ge_x[1],mTotEve,mVertexData.size());  
    }
  }
  
  if(mIsMC) {
    for(i=0;i<mTrackData.size();i++) {
      const TrackData *t=&mTrackData[i];
      if(t->vertexID<=0) continue; // skip not used or pileup vertex 
      hA[12]->Fill(zGeant-t->zDca);
    }
  }
   
  LOG_DEBUG<< Form("---- end of PPVertex Info\n")<<endm;

}

//======================================================
//======================================================
void 
StPPVertexFinder::CalibBeamLine(){
  LOG_INFO << "StPPVertexFinder::CalibBeamLine: activated saving high quality prim tracks for 3D fit of the beamLine"<<endm;
  mBeamLineTracks=1; 
}

//======================================================
//======================================================
void 
StPPVertexFinder::UseVertexConstraint(double x0, double y0, double dxdz, double dydz, double weight) {
  mVertexConstrain = true;
  mX0 = x0;
  mY0 = y0;
  mdxdz = dxdz;
  mdydz = dydz;

  // weight - not used ;
  LOG_INFO << "StPPVertexFinder::Using Constrained Vertex" << endm;
  LOG_INFO << "x origin = " << mX0 << endm;
  LOG_INFO << "y origin = " << mY0 << endm;
  LOG_INFO << "slope dxdz = " << mdxdz << endm;
  LOG_INFO << "slope dydz = " << mdydz << endm;

}


//==========================================================
//==========================================================
int 
StPPVertexFinder::fit(StEvent* event) {
  LOG_INFO << "***** START FIT" << endm;
  if(mBeamLineTracks) vertex3D->clearEvent();

  hA[0]->Fill(1);

  StEvent *mEvent = (StEvent *)  StMaker::GetChain()->GetInputDS("StEvent");
  assert(mEvent); 

#if 0 // do you want trigger filter?
  //-------------------tmp trigger filter
  StTriggerIdCollection *ticA=mEvent->triggerIdCollection();
  assert(ticA);     
  const StTriggerId *L1=L1=ticA->nominal(); 
  // vector<unsigned int> trgL=tic->nominal().triggerIds();
  vector<unsigned int> trgL=L1->triggerIds();
  printf("PPVertex::Fit START trigL len=%d \n",trgL.size());
  uint ii;
  for(ii=0;ii<trgL.size();ii++){
    printf("ii=%d trigID=%d\n",ii,trgL[ii]);
  }
  
  if(L1->isTrigger(127221)==0) { 
    printf("PPV is off for this trigger(s)\n"); 
    return false;
  }
#endif
  //--------------------end of trigger filter

  mTotEve++;
  eveID=event->id();
  LOG_INFO << "\n   @@@@@@   PPVertex::Fit START nEve="<<mTotEve<<"  eveID="<<eveID<<  endm;

  TString tt="Vertex likelyhood, eveID=";
  tt+=eveID;
  hL->SetTitle(tt);

  hA[0]->Fill(2);

  if(mToolkit==0) {    
   LOG_WARN <<"no Sti tool kit,  PPV is OFF"<<endm;
   return 0;
  }

 // get BTOF info - dongx

  if(mUseBtof) {
    StBTofCollection *btofColl = (StBTofCollection*)mEvent->btofCollection();
    if(btofColl==0) {
      LOG_WARN << "no btofCollection , continue THE SAME eve"<<endm;
    } else {
      btofList->build(btofColl);
    }
  }

  // get CTB info, does not  work for embeding 
  if(mUseCtb) {// CTB could be off since 2006 
    if(mIsMC){
      St_DataSet *gds=StMaker::GetChain()->GetDataSet("geant");
      ctbList->buildFromMC(gds);  // use M-C
    }  else {
      StTriggerData *trgD=event->triggerData ();
      ctbList->buildFromData(trgD); // use real data
    }
  }

  
  StEmcCollection* emcC =(StEmcCollection*)mEvent->emcCollection(); 
  if(emcC==0) {
    LOG_WARN <<"no emcCollection , continue THE SAME eve"<<endm;
  } else {
    assert(emcC);
    StEmcDetector* btow = emcC->detector( kBarrelEmcTowerId); 
    if(btow==0) {
      LOG_WARN <<"no BEMC in emcColl , continue THE SAME eve"<<endm;
    } else {
      assert(btow);
      bemcList->build(btow, mMinAdcBemc);
    }
    
    StEmcDetector* etow = emcC->detector(kEndcapEmcTowerId); 
    if(etow==0) {
      LOG_WARN <<"no EEMC in emcColl , continue THE SAME eve"<<endm;
    } else {
      assert(etow);
      eemcList->build(etow, mMinAdcEemc);
    }
  }

  //get the Sti track container...
  StiTrackContainer* tracks = mToolkit->getTrackContainer();
   if(tracks==0) {
     LOG_WARN <<"no STi tracks , skip eve"<<endm;
     printInfo();  
     return 0 ;				       
   }

  hA[0]->Fill(4);
  
  //select reasonable tracks and add them to my list
  int k=0;
  int kBtof=0,kCtb=0,kBemc=0, kEemc=0,kTpc=0;
  int nmAny=0;

  int ntrk[7]; for(int i=0; i<7; i++) ntrk[i]=0;

  for (StiTrackContainer::const_iterator it=(*tracks).begin();  it!=(*tracks).end(); ++it) {
    k++;
    const StiKalmanTrack* track = static_cast<StiKalmanTrack*>(*it);
    TrackData t;

    ntrk[0]++;
    if(track->getFlag()!=true)        {ntrk[1]++; continue;}
    if(track->getPt()<mMinTrkPt)      {ntrk[2]++; continue;}
    if(mDropPostCrossingTrack){
      if(isPostCrossingTrack(track))  {ntrk[3]++; continue;}  // kill if it has hits in wrong z
    }
    if(!examinTrackDca(track,t))      {ntrk[4]++; continue;}  // drop from DCA		   
    if(!matchTrack2Membrane(track,t)) {ntrk[5]++; continue;}  // kill if nFitP too small	   
    ntrk[6]++;

    //cout <<"\n#e itr="<<k<<" gPt="<<track->getPt()<<" gEta="<<track->getPseudoRapidity()<<" nFitP="<<track->getFitPointCount()<<" of "<<track->getMaxPointCount()<<" poolSize="<< mTrackData->size()<<"  myW="<<t.weight<<endl;
    //printf(" t.weight AA=%f\n", t.weight);

    hA[1]->Fill(track->getChi2());
    hA[2]->Fill(track->getFitPointCount());
    hA[16]->Fill(track->getPt());
    //  dumpKalmanNodes(track);
    
    // ......... matcho various detectors ....................
    if(mUseBtof) matchTrack2BTOF(track,t,btofGeom);  // matching track to btofGeometry
    if(mUseCtb)  matchTrack2CTB(track,t);
    matchTrack2BEMC(track,t,242); // middle of tower in Rxy
    matchTrack2EEMC(track,t,288); // middle of tower in Z
    //.... all test done on this track .........
    t.mother=track;
    mTrackData.push_back(t); 

    hA[5]->Fill(t.rxyDca);

    if( t.mBtof>0 ) kBtof++;   // dongx
    if( t.mCtb>0 )  kCtb++;   
    if( t.mBemc>0)  kBemc++;   
    if( t.mEemc>0)  kEemc++;
    if( t.mTpc>0 )  kTpc++;
 
    if(t.mBtof>0 || t.mCtb>0 || t.mBemc>0 || t.mEemc>0 || t.mTpc>0 ) nmAny++ ; // dongx

    hACorr->Fill(t.mBtof, t.mBemc);
    //  t.print();
  }

  LOG_INFO<< Form("PPV:: # of input track          = %d",ntrk[0])<<endm;
  LOG_INFO<< Form("PPV:: dropped due to flag       = %d",ntrk[1])<<endm;
  LOG_INFO<< Form("PPV:: dropped due to pt         = %d",ntrk[2])<<endm;
  LOG_INFO<< Form("PPV:: dropped due to PCT check  = %d",ntrk[3])<<endm;
  LOG_INFO<< Form("PPV:: dropped due to DCA check  = %d",ntrk[4])<<endm;
  LOG_INFO<< Form("PPV:: dropped due to NHit check = %d",ntrk[5])<<endm;
  LOG_INFO<< Form("PPV:: # of track after all cuts = %d",ntrk[6])<<endm;

  if(mUseCtb) {
    ctbList ->print();
    ctbList ->doHisto();
  }

 if(mUseBtof) {
    btofList->print(); // dongx
    btofList->doHisto(); // dongx
  }

  bemcList->print();
  eemcList->print();
  LOG_INFO<< Form("PPV::TpcList size=%d nMatched=%d\n\n",mTrackData.size(),kTpc)<<endm;

  bemcList->doHisto();
  eemcList->doHisto();

  LOG_INFO << "PPV::fit() nEve="<<mTotEve<<" , "<<nmAny<<" traks with good DCA, matching: BTOF="<<kBtof<<" CTB="<<kCtb<<" BEMC="<<kBemc<<" EEMC="<<kEemc<<endm;


  if(nmAny<mMinMatchTr &&  mStoreUnqualifiedVertex<=0){
    LOG_INFO << "StPPVertexFinder::fit() nEve="<<mTotEve<<" Quit, to few matched tracks"<<endm;
    printInfo();
    return 0;
  }
  hA[0]->Fill(5);

  if(kBemc)  hA[0]->Fill(6);
  if(kEemc)  hA[0]->Fill(7);

  //............................................................
  // ...................... search for multiple vertices 
  //............................................................

  const float par_rankOffset=1e6; // to separate class of vertices (approximately)

  int nBadVertex=0;
  int vertexID=0;
  while(1) {
    if(! buildLikelihoodZ() ) break;
    VertexData V;
    V.id=++vertexID;
    if(! findVertexZ(V)) break;
  
    bool trigV=evalVertexZ(V);   // V.print();
    //bump up rank of 2+ track all vertices 
    if(V.nAnyMatch>=mMinMatchTr) V.Lmax+=par_rankOffset;
    if(!trigV) {
      if( nBadVertex>=mStoreUnqualifiedVertex)  continue; // drop this vertex
      /*  preserve this unqalified vertex for Akio 
	  and deposit 1 cent on Jan's bank account (optional) 
      */
      nBadVertex++;
      //bump down rank of sub-prime vertices 
      V.Lmax-=par_rankOffset; 
    } 
    
    {// ... more rank QA ...
      float rank=V.Lmax;
      if(rank>1e6)  hA[17]->Fill(log(rank-1e6)+10);
      else if(rank>0)   hA[17]->Fill(log(rank));
      else   hA[17]->Fill(log(rank+1e6)-10);
    }
    
    mVertexData.push_back(V);
    if(trigV && mBeamLineTracks) vertex3D->study(V.r,eveID);
  }

  LOG_INFO << "StPPVertexFinder::fit(totEve="<<mTotEve<<") "<<mVertexData.size()<<" vertices found, nBadVertex=" <<nBadVertex<< endm;
  
  if(mVertexData.size()>0)  hA[0]->Fill(8);
  if(mVertexData.size()>1)  hA[0]->Fill(9);

  exportVertices();
  printInfo();
  
  hA[4]->Fill(mVertexData.size());
  uint i;
  for(i=0;i<mVertexData.size();i++) {
    VertexData *V=&mVertexData[i];
    hA[3]->Fill(V->r.z());
  }
  
  if(mVertexData.size()<=0) {
    return 0; // no vertex
  }
  
  return size();
} 


//==========================================================
//==========================================================
bool  
StPPVertexFinder::buildLikelihoodZ(){
  hL->Reset();
  hM->Reset();
  hW->Reset();

  float dzMax2=mMaxZradius*mMaxZradius;

  int nt=mTrackData.size();
  LOG_DEBUG<< Form("PPV::buildLikelihood() pool of nTracks=%d",nt)<<endm;
  if(nt<=0) return false;

  int n1=0;
  int i;

  double *La=hL->GetArray(); // PPV main likelyhood histogram 
  double *Ma=hM->GetArray(); // track multiplicity  histogram 
  double *Wa=hW->GetArray(); // track weight histogram 
  
  for(i=0;i<nt;i++) {
    const TrackData *t=&mTrackData[i];
    if(t->vertexID!=0) continue; // track already used
    if(t->anyMatch) n1++;
    //  t->print();
    float z0=t->zDca;
    float ez=t->ezDca;
    float ez2=ez*ez;
    int j1=hL->FindBin(z0-mMaxZradius-.1);
    int j2=hL->FindBin(z0+mMaxZradius+.1);
    float base=dzMax2/2/ez2;
    float totW=t->weight;
    //  printf("i=%d Z0=%f ez=%f j1=%d j2=%d base=%f gPt/GeV=%.3f ctbW=%.3f\n",i,z0,ez,j1,j2,base,t->gPt,ctbW);

    int j;
    for(j=j1;j<=j2;j++) {
      float z=hL->GetBinCenter(j);
      float dz=z-z0;
      float xx=base-dz*dz/2/ez2;
      if(xx<=0) continue;
      La[j]+=xx*totW;
      Ma[j]+=1.;
      Wa[j]+=totW;
      // printf("z=%f dz=%f  xx=%f\n",z,dz,xx);
    }
    // break; // tmp , to get only one track
  }

 LOG_DEBUG<< Form("PPV::buildLikelihood() %d tracks w/ matched @ Lmax=%f",n1,hL->GetMaximum())<<endm;

#if 0 // save .ps for every vertex found
  //tmp
  LOG_WARN << "PPV saves .ps for every vertex, jan"<<endm;
  TCanvas c;
  hL->Draw();
  TString tt="eve";
  tt+=eveID; tt+="Lmx"; tt+=hL->GetMaximum();
  c.Print(tt+".ps");
#endif

  return (n1>=mMinMatchTr) ||  (mStoreUnqualifiedVertex>0 );
}

//==========================================================
//==========================================================
bool  
StPPVertexFinder::findVertexZ(VertexData &V) {

  if(hL->GetMaximum()<=0) return false; // no more tracks left

  int iMax=hL-> GetMaximumBin();
  float z0=hL-> GetBinCenter(iMax);
  float Lmax=hL-> GetBinContent(iMax);
  float accM=hM-> GetBinContent(iMax);
  float accW=hW-> GetBinContent(iMax);
  assert(accM>0);
  float avrW=accW/accM;

  // search for sigma of the vertex
  float Llow=0.9* Lmax;
  if((Lmax-Llow)<8*avrW )  Llow=Lmax-8*avrW;  // to be at least 4 sigma
  int i;
  double *L=hL->GetArray(); // likelyhood 

  int iLow=-1, iHigh=-1;
  for(i=iMax;i<=hL->GetNbinsX();i++) {
    if(L[i] >Llow) continue;
    iHigh=i;
    break;
  }
  for(i=iMax;i>=1;i--) {
    if(L[i] >Llow) continue;
    iLow=i;
    break;
  }
  
  float zLow=hL-> GetBinCenter(iLow);
  float zHigh=hL-> GetBinCenter(iHigh);

  float kSig= sqrt(2*(Lmax-Llow)/avrW);
  float sigZ= (zHigh-zLow)/2/kSig;
  LOG_DEBUG<< Form("PPV:: iLow/iMax/iHigh=%d/%d/%d\n",iLow,iMax,iHigh)
	  <<Form(" accM=%f  accW=%f  avrW=%f\n",accM,accW,avrW)   
	  <<Form("  Z low/max/high=%f %f %f, kSig=%f, sig=%f\n",zLow,z0,zHigh,kSig,sigZ)
	  <<Form(" found  PPVertex(ID=%d,neve=%d) z0 =%.2f +/- %.2f\n",V.id,mTotEve,z0,sigZ)<<endm;
  if(sigZ<0.1) sigZ=0.1; // tmp, make it not smaller than the bin size

  // take x,y from beam line equation, TMP
  float x=mX0+z0*mdxdz;
  float y=mY0+z0*mdydz;

  V.r=TVector3(x,y,z0);
  V.er=TVector3(0.1,0.1,sigZ); //tmp
  V.Lmax=Lmax;

  return true;
}

//==========================================================
//==========================================================
bool  
StPPVertexFinder::evalVertexZ(VertexData &V) { // and tag used tracks
  // returns true if vertex is accepted accepted
  if(mBeamLineTracks) vertex3D->clearTracks();
  int nt=mTrackData.size();
  LOG_DEBUG << "StPPVertexFinder::evalVertex Vid="<<V.id<<" start ..."<<endm;
  int n1=0, nHiPt=0;
  int i;
  
  for(i=0;i<nt;i++) {
    TrackData *t=&mTrackData[i];
    if(t->vertexID!=0) continue;
    if(! t->matchVertex(V,mMaxZradius)) continue; // track to far
    // this track belongs to this vertex
    n1++;
    t->vertexID=V.id;
    V.gPtSum+=t->gPt;
    if(mBeamLineTracks) vertex3D->addTrack(t);
    if( t->gPt>mCut_oneTrackPT && ( t->mBemc>0|| t->mEemc>0) ) nHiPt++;

    if(  t->mTpc>0)       V.nTpc++;
    else if (  t->mTpc<0) V.nTpcV++;

    if(  t->mBtof>0)       V.nBtof++;  // dongx
    else if (  t->mBtof<0) V.nBtofV++;

    if(  t->mCtb>0)       V.nCtb++;
    else if (  t->mCtb<0) V.nCtbV++;

    if(  t->mBemc>0)       V.nBemc++;
    else if (  t->mBemc<0) V.nBemcV++;

    if(  t->mEemc>0)       V.nEemc++;
    else if (  t->mEemc<0) V.nEemcV++;

    if( t->anyMatch)     V.nAnyMatch++;
    else if (t->anyVeto) V.nAnyVeto++;
  } 
  V.nUsedTrack=n1;  

  bool validVertex = V.nAnyMatch>=mMinMatchTr;
  if((mAlgoSwitches & kSwitchOneHighPT) && ( nHiPt>0)) {
    validVertex|=1;
  }

  if(!validVertex) { // discrad vertex
    //no match tracks in this vertex, tag vertex ID in tracks differently
    //V.print(cout);
    LOG_DEBUG << "StPPVertexFinder::evalVertex Vid="<<V.id<<" rejected"<<endm;
    for(i=0;i<nt;i++) {
      TrackData *t=&mTrackData[i];
      if(t->vertexID!=V.id) continue;
      t->vertexID=-V.id;
    }
    return false;
  }
  
  LOG_INFO << "StPPVertexFinder::evalVertex Vid="<<V.id<<" accepted, nAnyMatch="<<V.nAnyMatch<<" nAnyVeto="<<V.nAnyVeto<<endm;
  return true;
}

 
//-------------------------------------------------
//-------------------------------------------------
void 
StPPVertexFinder::exportVertices(){
  if ( ! mVertexConstrain ){
    // code is not ready for reco w/o beamLine
    LOG_FATAL << "StPPVertexFinder code is not ready for reco w/o beamLine" << endm;
    assert(mVertexConstrain); 
  }
  uint i;
  for(i=0;i<mVertexData.size();i++) {
    VertexData *V=&mVertexData[i];
    StThreeVectorD r(V->r.x(),V->r.y(),V->r.z());

    Float_t cov[6];
    memset(cov,0,sizeof(cov)); 
    cov[0]=V->er.x()*V->er.x(); 
    cov[2]=V->er.y()*V->er.y(); 
    cov[5]=V->er.z()*V->er.z();  // [5] is correct,JB 

    StPrimaryVertex primV;
    primV.setPosition(r);
    primV.setCovariantMatrix(cov); 

    if(mUseCtb)  primV.setVertexFinderId(ppvVertexFinder);
    else         primV.setVertexFinderId(ppvNoCtbVertexFinder); 

    primV.setNumTracksUsedInFinder(V->nUsedTrack);
    primV.setNumMatchesWithBTOF(V->nBtof);  // dongx
    primV.setNumMatchesWithCTB(V->nCtb);
    primV.setNumMatchesWithBEMC(V->nBemc);
    primV.setNumMatchesWithEEMC(V->nEemc);
    primV.setNumTracksCrossingCentralMembrane(V->nTpc);
    primV.setSumOfTrackPt(V->gPtSum);
    primV.setRanking(V->Lmax);
    primV.setFlag(1); //??? is it a right value?
  
    //..... add vertex to the list
    addVertex(&primV);
  }
  LOG_DEBUG << "StPPVertexFinder::exportVertices(), size="<<size()<<endm;
}

//-------------------------------------------------
//-------------------------------------------------
void 
StPPVertexFinder::Finish() {

  if(mBeamLineTracks) { // save local histo w/ PPV monitoring
    StIOMaker *ioMk=(StIOMaker*)StMaker::GetChain()->GetMaker("inputStream");
    
    TString tt="ppv";
    if(ioMk) {
      assert(ioMk);
      const char *fname=ioMk->GetFileName();
      tt=strstr(fname,"st_");
      tt.ReplaceAll(".daq",".ppv");
    } else {
      tt= ((StBFChain*)StMaker::GetChain())->GetFileOut() ;
      tt.ReplaceAll(".root",".ppv");
    }
    LOG_INFO << "PPV save local histo="<<tt<<endm;
    saveHisto(tt.Data());
  }

  LOG_INFO << "StPPVertexFinder::Finish() done, seen eve=" <<mTotEve<< endm;
  //  TString fileIn    = ((StBFChain*)StMaker::GetChain())->GetFileIn() ;
  // LOG_INFO << "in="<<fileIn<< endm;
}

//-------------------------------------------------
//-------------------------------------------------
void 
StPPVertexFinder::saveHisto(TString fname){
  TString outName=fname+".hist.root";
  TFile f( outName,"recreate");
  assert(f.IsOpen());
  printf("%d histos are written  to '%s' ...\n",HList->GetEntries(),outName.Data());
  HList->ls();
  HList->Write();
  f.Close();
}

//==========================================================
//==========================================================
void  
StPPVertexFinder::dumpKalmanNodes(const StiKalmanTrack*track){
   
  //.................... print all nodes ...........
  StiKTNBidirectionalIterator it;
  int in=0,nh=0,nTpc=0;
  float zL=999, zH=-999;
  for (it=track->begin();it!=track->end();it++,in++) {
    StiKalmanTrackNode& ktn = (*it);
    if(!ktn.isValid()) continue; // from Victor
    if(ktn.getHit() && ktn.getChi2() >1000) continue; // from Victor
    const StiDetector * det=ktn.getDetector();
    assert(!(ktn.x()) || det);
    float rxy=ktn.getX();
    bool actv= !det || det->isActive(ktn.getY(), ktn.getZ());
    if(!det || (rxy>58 && rxy < 190)){
      float z=ktn.z_g();
      if(zL>z) zL=z;
      if(zH<z) zH=z;
      if(actv) {
	nTpc++;
	if(ktn.getHit()) nh++;
      }
    }
  }
  int nn=in;
  TString tagPlp=" "; if((nTpc-nh)>10) tagPlp=" plp";
  TString tagMemb=" "; if(zH*zL<0) tagMemb=" memb";

  LOG_INFO
    <<"\n#e dumpKalmanNodes nNodes="<<nn<<" actv: nTPC="<<nTpc<<" nHit="<<nh
    <<" zL="<<zL<<" zH="<<zH <<tagPlp<<tagMemb
    <<endm;
 
  // ........................print both ends  ....................
  LOG_INFO << "#e  |P|="<<track->getP()<<" pT="<<track->getPt()<<" eta="<<track->getPseudoRapidity()<<" nFitP="<<track->getFitPointCount() << endm; 
  StiKalmanTrackNode* inNode=track->getInnerMostNode();
  LOG_INFO << "#e @InnerMostNode x:"<< inNode->x_g()<<" y:"<< inNode->y_g()<<" z:"<< inNode->z_g()<<" Eta="<<inNode->getEta()<<" |P|="<<inNode->getP() << endm;
  StiKalmanTrackNode* ouNode=track->getOuterMostNode();
  LOG_INFO << "#e @OuterMostNode g x:"<< ouNode->x_g()<<" y:"<< ouNode->y_g()<<" z:"<< ouNode->z_g()<<" Eta="<<ouNode->getEta()<<" |P|="<<ouNode->getP() << endm;

 in=0;
  for (it=track->begin();it!=track->end();it++,in++) {
    // if(in>=2 && in<nn-5) continue; // print only ends of the track
    StiKalmanTrackNode& ktn = (*it);
    if(!ktn.isValid()) continue; // from Victor
    if(ktn.getHit() && ktn.getChi2() >1000) continue; // from Victor
    float sy=sqrt(ktn.getCyy());
    float sz=sqrt(ktn.getCzz());
    const StiDetector * det=ktn.getDetector();
    assert(!(ktn.x()) || det);

    LOG_INFO << "#e in="<<in<<" |P|="<<ktn.getP()<<" Local: x="<<ktn.getX()<<" y="<<ktn.getY()<<" +/- "<<sy<<" z="<<ktn.getZ()<<" +/- "<<sz;

    if(ktn.getHit()) {LOG_INFO <<" hit=1";}
    else             {LOG_INFO <<" hit=0";}

    if(det==0)       {LOG_INFO <<" noDet ";}
    else             {LOG_INFO <<" detActv="<<(!det || det->isActive(ktn.getY(), ktn.getZ()));}
    LOG_INFO << endm;
    //    break; // tmp
  }
}

//==========================================================
//==========================================================
bool  
StPPVertexFinder::examinTrackDca(const StiKalmanTrack*track,TrackData &t){

  //1 StiKalmanTrackNode* inNode=track->getInnerMostNode();
  //1 cout <<"#e  track->getPseudoRapidity()="<<track->getPseudoRapidity()<<" track->getFitPointCount()="<<track->getFitPointCount()<<endl;
  
  // .......... test DCA to beam .............
  StiKalmanTrack track1=*track; // clone track
  StiKalmanTrackNode* bmNode=track1.extrapolateToBeam();
  if(bmNode==0)  { 
    //1 cout<<"#a @beam  DCA NULL"<<endl; 
    return false ; 
  }

  float rxy=rxyG(bmNode);

  //1 cout<<"#e @beam global DCA x:"<< bmNode->x_g()<<" y:"<< bmNode->y_g()<<" z:"<< bmNode->z_g()<<" Rxy="<< rxy <<endl;
  if(rxy>mMaxTrkDcaRxy) return false;
  if( fabs(bmNode->z_g())> mMaxZrange )   return false ; 
 
  //1 cout<<"#e inBeam |P|="<<bmNode->getP()<<" pT="<<bmNode->getPt()<<" local x="<<xL(bmNode)<<" y="<<yL(bmNode)<<" +/- "<<eyL(bmNode)<<" z="<<zL(bmNode)<<" +/- "<<ezL(bmNode)<<endl;

  t.zDca=zL(bmNode);
  t.ezDca=ezL(bmNode);
  t.rxyDca=rxy;
  t.gPt=bmNode->getPt();

  //...... record more detals for 3D vertex reco
  t.dcaTrack.R.SetXYZ(xG(bmNode),yG(bmNode),zG(bmNode));
  // approximation below: use sigX=sigY, I do not want to deal wih rotations in X-Y plane, Jan B.
  t.dcaTrack.sigYloc=eyL(bmNode); 
  t.dcaTrack.sigZ=ezL(bmNode); 
  StThreeVectorF const globP3=bmNode->getGlobalMomentumF();
  t.dcaTrack.gP.SetXYZ(globP3.x(),globP3.y(),globP3.z());
  t.dcaTrack.fitErr=bmNode->fitErrs();
  t.dcaTrack.gChi2=track1.getChi2();
  t.dcaTrack.nFitPoint=track1.getFitPointCount();
  //  t.dcaTrack.print();

  return true;
}


//==========================================================
//==========================================================
void  
StPPVertexFinder::matchTrack2BTOF(const StiKalmanTrack* track,TrackData &t,StBTofGeometry* geom){

  StiKalmanTrackNode* ouNode=track->getOuterMostNode();

  StThreeVectorD posTOF;
  // helix extrapolation:
  StThreeVectorD ou(ouNode->getX(),ouNode->getY(),ouNode->getZ());
  ou.rotateZ(ouNode->getAlpha());
  StPhysicalHelixD hlx(fabs(ouNode->getCurvature()),
                       ouNode->getDipAngle(),
                       ouNode->getPhase(),
                       ou,
                       ouNode->getHelicity());
  IntVec idVec;
  DoubleVec pathVec;
  PointVec crossVec;

  IntVec iBinVec;
  if(geom->HelixCrossCellIds(hlx,idVec,pathVec,crossVec)) {
    for(size_t i=0;i<idVec.size();i++) {
      int itray, imodule, icell;
      geom->DecodeCellId(idVec[i], icell, imodule, itray);

      Double_t local[3], global[3];
      for(int j=0;j<3;j++) local[j] = 0;
      global[0] = crossVec[i].x();
      global[1] = crossVec[i].y();
      global[2] = crossVec[i].z();
      StBTofGeomSensor *sensor = geom->GetGeomSensor(imodule,itray);
      if(!sensor) {
        LOG_WARN << "no sensitive module in this projection??? - weird" << endm;
        continue;
      }
      sensor->Master2Local(&global[0],&local[0]);
//      LOG_INFO << "   Hit the TOF cell " << itray << " " << imodule << " " << icell << endm;
//      LOG_INFO << "   position " << local[0] << " " << local[1] << " " << local[2] << endm;
//      float yCenter = (icell-1-2.5)*3.45;  // cell center position;
//      if(fabs(local[1]-yCenter)>mDyBtof) continue;
//      if(icell==1||icell==6) continue;
      if(local[2]<mMinZBtof||local[2]>mMaxZBtof) continue;
      int iBin = btofList->cell2bin(itray, imodule, icell);
      iBinVec.push_back(iBin);
      btofList->addBtofTrack(itray, imodule, icell);
      LOG_DEBUG << "   !!! Push to the list ...tray/module/cell " << itray << "/" << imodule << "/" << icell << endm;
    }
  }

  bool  btofMatch=btofList->isMatched(iBinVec);
  bool  btofVeto =btofList->isVetoed(iBinVec);
  float btofW    =btofList->getWeight(iBinVec);
  btofList->addBtofMatch(iBinVec);  // update the nMatch statistics

  LOG_DEBUG << " ** BTOF ** match/veto/weight = " << btofMatch << " " << btofVeto << " " << btofW << endm;

  t.updateAnyMatch(btofMatch,btofVeto,t.mBtof);
  t.weight*=btofW;
  t.btofBin= iBinVec.size() ? iBinVec[0] : -1;
}

//==========================================================
//==========================================================
void  
StPPVertexFinder::matchTrack2CTB(const StiKalmanTrack* track,TrackData &t){
  const double Rctb=213.6; // (cm) radius of the CTB 

  StiKalmanTrackNode* ouNode=track->getOuterMostNode();

  StThreeVectorD posCTB;
  float path=-1;
  //alternative helix extrapolation:
  if(1){
    StiKalmanTrackNode * inNode = ouNode;
    StThreeVectorD in(inNode->getX(),inNode->getY(),inNode->getZ());
    in.rotateZ(inNode->getAlpha());
    StPhysicalHelixD hlx(fabs(inNode->getCurvature()),
			 inNode->getDipAngle(),
			 inNode->getPhase(),
			 in,
			 inNode->getHelicity());
    pairD  d2;
    d2 = hlx.pathLength(Rctb);
    path=d2.second;
    if(d2.first>=0 || d2.second<=0) {
      LOG_DEBUG <<Form("WARN MatchTrk , unexpected solution for track crossing CTB\n")<<
	Form(" d2.firts=%f, second=%f, try first", d2.first, d2.second)<<endm;
      path=d2.first;
    }
    posCTB = hlx.at(path);
    // printf(" punch Cylinder x,y,z=%.1f, %.1f, %.1f path.second=%.1f\n",posCTB.x(),posCTB.y(),posCTB.z(),path);
  }

  // official Sti node extrapolation
  if(0){
    StiKalmanTrack track2=*track;
    StiKalmanTrackNode* ctbNode=track2.extrapolateToRadius(Rctb);

    if(ctbNode==0)  { 
      LOG_INFO <<"#e @ctbNode NULL"<<endm;
      LOG_INFO <<"#e @track dump"<< *track << endm;
      LOG_INFO <<"#e @OuterMostNode dump"<< *ouNode <<endm;
      return; 
    }
    //1 cout<<"#e inCTB |P|="<<ctbNode->getP()<<" local x="<<xL(ctbNode)<<" y="<<yL(ctbNode)<<" +/- "<<eyL(ctbNode)<<" z="<<zL(ctbNode)<<" +/- "<<ezL(ctbNode)<<endl;
    
    //    cout<<"#e @ctbNode g x:"<< ctbNode->x_g()<<" y:"<< ctbNode->y_g()<<" z:"<< ctbNode->z_g()<<" phi/deg="<<phi/3.1416*180<<endl;
    posCTB=StThreeVectorD( ctbNode->x_g(),ctbNode->y_g(),ctbNode->z_g());
  }

  float phi=atan2(posCTB.y(),posCTB.x());
  if(phi<0) phi+=2*C_PI;// now phi is [0,2Pi] as for CTB slats
  float eta=posCTB.pseudoRapidity();
  //1 cout<<"#e @ctbNode xyz="<<posCTB<<" eta="<<eta<<" phi/deg="<<phi/3.1416*180<<" path/cm="<<path<<endl;
  if(fabs(eta)<1) hA[10]->Fill(posCTB.z());

  int iBin=ctbList->addTrack(eta,phi);
  
  bool  ctbMatch=ctbList->isMatched(iBin);
  bool  ctbVeto =ctbList->isVetoed(iBin);
  float ctbW    =ctbList->getWeight(iBin);
  
  t.updateAnyMatch(ctbMatch,ctbVeto,t.mCtb);
  t.weight*=ctbW;
  t.ctbBin=iBin;
}

//==========================================================
//==========================================================
void  
StPPVertexFinder::matchTrack2BEMC(const StiKalmanTrack* track,TrackData &t, float Rxy){
  
  StiKalmanTrackNode* ouNode=track->getOuterMostNode();

  StThreeVectorD posCyl;
  float path=-1;
  //alternative helix extrapolation:
  StThreeVectorD ou(ouNode->getX(),ouNode->getY(),ouNode->getZ());
  ou.rotateZ(ouNode->getAlpha());
  StPhysicalHelixD hlx(fabs(ouNode->getCurvature()),
		       ouNode->getDipAngle(),
		       ouNode->getPhase(),
		       ou,
		       ouNode->getHelicity());
  pairD  d2;
  d2 = hlx.pathLength(Rxy);
  path=d2.second;
  if(d2.first>=0 || d2.second<=0) {
    LOG_DEBUG <<Form("WARN MatchTrk , unexpected solution for track crossing BEMC Cyl\n")<<
      Form(" d2.firts=%f, second=%f, try first\n", d2.first, d2.second)<<endm;
    path=d2.first;
  }
  posCyl = hlx.at(path);
  // printf(" punch Cylinder x,y,z=%.1f, %.1f, %.1f path.second=%.1f\n",posCyl.x(),posCyl.y(),posCyl.z(),path);


  float phi=atan2(posCyl.y(),posCyl.x());
  if(phi<0) phi+=2*C_PI;// now phi is [0,2Pi] as for Cyl slats
  float eta=posCyl.pseudoRapidity();
  
  // cout<<"#e @bemcNode xyz="<<posCyl<<" etaDet="<<eta<<" phi/deg="<<phi/3.1416*180<<" path/cm="<<path<<endl;

  if(fabs(eta)<1) hA[11]->Fill(posCyl.z());
  
  int iBin=bemcList->addTrack(eta,phi);
  bool  bemcMatch=bemcList->isMatched(iBin);
  bool  bemcVeto =bemcList->isVetoed(iBin);
  float bemcW    =bemcList->getWeight(iBin);

  t.updateAnyMatch(bemcMatch,bemcVeto,t.mBemc);
  t.weight*=bemcW;
  t.bemcBin=iBin;

}


//==========================================================
//==========================================================
void  
StPPVertexFinder::matchTrack2EEMC(const StiKalmanTrack* track,TrackData &t,float z){
  
  const float minEta=0.7 ;// tmp cut
  const float maxPath=200 ;// tmp, cut too long extrapolation

  StiKalmanTrackNode* ouNode=track->getOuterMostNode();
  StiKalmanTrackNode* inNode=track->getInnerMostNode();

  //direction of extrapolation must be toward West (Z+ axis)
  if(inNode->getZ()> ouNode->getZ()) return;
  
  // droop too steep tracks
  if(track->getPseudoRapidity()<minEta) return;

  StThreeVectorD rSmd=StThreeVectorD(0,0,z); 
  StThreeVectorD n=StThreeVectorD(0,0,1);

  StThreeVectorD ou(ouNode->getX(),ouNode->getY(),ouNode->getZ());
  ou.rotateZ(ouNode->getAlpha());
  StPhysicalHelixD hlx(fabs(ouNode->getCurvature()),
		       ouNode->getDipAngle(),ouNode->getPhase(),
		       ou,ouNode->getHelicity());

   // path length at intersection with plane
   // double       pathLength(const StThreeVectorD& r,
   //                         const StThreeVectorD& n) const;

  double path = hlx.pathLength(rSmd,n);
  //cout<<" EEMC match: path="<<path<<endl;
  if(path>maxPath) return; // too long extrapolation

  StThreeVectorD r = hlx.at(path);
  float periodL=hlx. period();
 
  if(periodL<2*path) {
    LOG_DEBUG <<Form(" Warn, long path fac=%.1f ",path/periodL)<<
      Form("  punchEEMC1 x,y,z=%.1f, %.1f, %.1f path=%.1f period=%.1f\n",r.x(),r.y(),r.z(),path,periodL)<<endm; 
  }

  float phi=atan2(r.y(),r.x());
  if(phi<0) phi+=2*C_PI;// now phi is [0,2Pi] as for Cyl slats
  float eta=r.pseudoRapidity();

  int iBin=eemcList->addTrack(eta,phi);
  bool  eemcMatch=eemcList->isMatched(iBin);
  bool  eemcVeto =eemcList->isVetoed(iBin);
  float eemcW    =eemcList->getWeight(iBin);

  t.updateAnyMatch(eemcMatch,eemcVeto,t.mEemc);
  t.weight*=eemcW;
  t.eemcBin=iBin;

}

//==========================================================
//==========================================================
bool  
StPPVertexFinder::matchTrack2Membrane(const StiKalmanTrack* track,TrackData &t){
  const float RxyMin=59, RxyMax=199, zMax=200;
  const float zMembraneDepth=1; // (cm) ignore signe change for nodes so close to membrane

  //generate bitt pattern for TPC nodes with hits 
  vector<int> hitPatt;
  int nPos=0,nFit=0;
  int in=0;
  float lastRxy=9999;
  float lastZ=9999;

  int jz0=0;
  StiKTNBidirectionalIterator it;
  for (it=track->begin();it!=track->end();it++) {
    StiKalmanTrackNode* ktnp=& (*it);
    if(!ktnp->isValid()) continue; // from Victor
    //if(ktnp->getHit() && ktnp->getChi2() >1000) continue; // from Victor ---> those track need to be counted as npossiblehit, commented out
    float rxy=rxyG(ktnp); //ktn.getX();
    float z=zG(ktnp);  //ktn.z_g();
    if(rxy<RxyMin) continue;
    if(rxy>RxyMax) continue;
    if(fabs(z)>zMax) continue;
    // .........node is within TPC fiducial volume
    if(lastRxy<=rxy){
      LOG_WARN << "StPPVertexFinder::matchTrack2Membrane() \n the "<<in<<" node of the kalmanTrack below is out of order and is ignorred in (some) of vertex finder analysis"<<"\n  Z="<<z<<" rXY="<<rxy<<" last_rxy="<<lastRxy<<endm;
      //dumpKalmanNodes(track);
      continue;
    }
    lastRxy=rxy;
    if(in==0) lastZ=z;
    in++;
    if(fabsf(z)>zMembraneDepth) { //ignore hits too close to z=0
      if(lastZ*z<0) {             // track just crossed Z=0 plane
	if(jz0>0) {
	  LOG_WARN << "StPPVertexFinder::matchTrack2Membrane() \n the "<<in<<" node of the kalmanTrack crosses Z=0 for the 2nd time, this track has a strange list of nodes - continue"<<endm;
	  //dumpKalmanNodes(track);
	}
	//assert(jz0==0); // only one crosss point is expected
	jz0=hitPatt.size();
      }
      lastZ=z;
    }
    const StiDetector * det=ktnp->getDetector();
    assert(!(ktnp->x()) || det);
    bool active=!det || det->isActive(yL(ktnp), zL(ktnp));
    int hit=ktnp->getHit()?1:0;
    if(active) {
      hitPatt.push_back(hit);
      nPos++;
      if(hit && ktnp->getChi2() <=1000 ) nFit++;
    }
    // cout<<"#m in="<<in<<" act="<<active<<" hit="<<hit<<" size="<<hitPatt.size()<<" jz0="<<jz0<<" z="<<z<<" Rxy="<<rxy<<endl; 
  }
  // cout<<"#m nFit="<<nFit<<" of nPos="<<nPos<<endl;

  if(nFit<  mMinFitPfrac  * nPos) return false; // too short fragment of a track

  if( mFitPossWeighting)
    t.weight*=1.*nFit/nPos;// introduced in 2012 for pp510 to differentiate between global track quality, together with lowering the overall threshold from 0.7 to 0.51, Jan
  
  t.scanNodes(hitPatt,jz0); // if central membrane is crossed, scale weight inside


  return true;
}

//==========================================================
//==========================================================

bool StPPVertexFinder::isPostCrossingTrack(const StiKalmanTrack* track){
  const float RxyMin=59, RxyMax=199, zMax=200;
  const float zMembraneDepth=1.0; 
  const int   nWrongZHitCut=2;
  int nWrongZHit=0;
  StiKTNBidirectionalIterator it;
  for (it=track->begin();it!=track->end();it++) {
    StiKalmanTrackNode* ktnp=& (*it);
    if(!ktnp->isValid() || ktnp->getChi2()>1000 ) continue;
    StiHit* stihit=ktnp->getHit();
    if(stihit){
      StHit* sthit=(StHit*)stihit->stHit();
      if(sthit){
	if(sthit->detector()==kTpcId){
	  StTpcHit* hit=(StTpcHit*) sthit;
	  float r=hit->position().perp();
	  if (r < RxyMin) continue;
	  if (r > RxyMax) continue;
	  float z=hit->position().z();
	  if (fabs(z) > zMax) continue;
	  if ((z < -zMembraneDepth && hit->sector() <= 12) ||
	      (z >  zMembraneDepth && hit->sector() >  12)) {
	    nWrongZHit++;
	    if(nWrongZHit>=nWrongZHitCut) {return true;}
	  }	
	}
      }
    }
  }
  return false;
}

/**************************************************************************
 **************************************************************************
 * $Log: StPPVertexFinder.cxx,v $
 * Revision 1.46  2015/08/31 20:28:02  genevb
 * Correct a typo in a print statement present since version 1.1 : bemc->eemc
 *
 * Revision 1.45  2013/08/16 20:49:38  perev
 * PPV with only StEvent dependency
 *
 * Revision 1.44  2013/04/09 22:37:56  genevb
 * Remove boostEfficiency codes: DB usage implemented
 *
 * Revision 1.43  2013/04/09 21:11:58  genevb
 * Use database table for track selection cuts
 *
 * Revision 1.42  2013/04/05 21:00:02  jeromel
 * Implemented and merged back to source the boostEfficiency (i.e. change of
 * nFit /nPossible points on the track fract to consider). No DB imp yet.
 *
 * Fixed boostEfficiency()
 *
 * Changed cout to LOG_INFO
 *
 * Revision 1.41  2012/11/06 20:58:04  fisyak
 * Remove second addition of btofGeom to db maker
 *
 * Revision 1.40  2012/05/25 20:19:40  balewski
 * convert many LOG_INFO to LOG_DEBUG to make PPV more silent. No intencional change of PPV logic.
 *
 * Revision 1.39  2012/05/07 15:55:30  fisyak
 * Proper handing of btofGeometry
 *
 * Revision 1.38  2010/09/16 04:18:55  rjreed
 * Moved intialized of btof from init to initrun and changed it so that the btof class is not utilized if mUseBtof is false
 *
 * Revision 1.37  2010/09/10 21:08:35  rjreed
 * Added function UseBOTF and bool mUseBtof to switch the use of the TOF on and off in vertex finding.  Default value is off (false).
 * Added functions, and variables necessary to use the TOF in PPV for vertex finding.  Includes matching tracks to the TOF and changing the track weight based on its matched status with the TOF.
 *
 * Revision 1.36  2009/11/20 18:54:08  genevb
 * Avoid compiler warning about operator order precedence
 *
 * Revision 1.35  2009/11/05 21:40:08  rjreed
 * Last line of matchTrack2Membrane was deleted between version 1.29 and 1.30.  This line checks
 * tracks to determine whether they've crossed the TPC CM.  This rev reinstates the line.
 *
 * Revision 1.34  2009/07/09 21:29:03  balewski
 * allow export of prim tracks for 3D beam line fit (use VtxSeedCalG option),
 * oneTrack vertex thresholds was lowered form 15 to 10 GeV/c
 *
 * Revision 1.33  2009/02/05 21:43:59  balewski
 * Oleksandr renamed StEEmcDbMaker to StEEmcDb and requested this set of code corrections
 *
 * Revision 1.32  2008/12/02 14:35:05  balewski
 * I forgot to require EMC hit for highPT track, now it is in
 *
 * Revision 1.31  2008/12/01 22:57:39  balewski
 * Added capability to reco 1 high pT track vertices with positive rank. 2+ match vertices will have rank above 1e6. Sub-prime vertices (for Akio) have negative rank. More details is given at:
 * http://drupal.star.bnl.gov/STAR/comp/reco/vf/ppv-vertex/2009-algo-upgrade-1
 *
 * Revision 1.30  2008/10/21 19:23:05  balewski
 * store unqualified vertices on Akio's request
 *
 * Revision 1.29  2008/08/21 22:09:31  balewski
 * - In matchTrack2Membrane()
 *   - Cut on hit max R chanegd from 190 to 199cm
 *   - Fixed logic failure of counting possible hits
 *   - Fixed logic failure of crossing CM for certain pattern of hits
 * - Added a new function bool isPostCrossingTrack()
 *   - it returns true if track have 2 or more hits in wrong z
 * - Use isPostCrossingTrack() in fit()
 * - Added switch setDropPostCrossingTrack(bool), defaulted to true
 * All changes tested & implemented by Akio in preparation for 2008 pp production.
 * The key change (removing PostCrossingTrack) is in response to the change of the TPC cluster finder
 * - now we use the on-line version which allows for longer range of TPC time buckets to be used.
 *
 * Revision 1.28  2008/04/03 16:24:31  fisyak
 * replace sti->getToolkit() by StiToolkit::instance()
 *
 * Revision 1.27  2008/02/12 17:51:20  jeromel
 * Assert of Year number removed. Assert on beamLine left but added an explaination (so we won't have to rediscover this).
 *
 * Revision 1.26  2007/03/22 08:42:05  balewski
 * extend validity of PPV for 2007 data taking
 *
 * Revision 1.25  2006/10/17 13:38:03  fisyak
 * Remove dependencies from dead classes
 *
 * Revision 1.24  2006/07/31 17:57:54  balewski
 * cleanup before 2006 prod, no changes in algo, CTB stays out all the time
 *
 * Revision 1.23  2006/06/02 20:46:55  perev
 * Accoun DCA node added
 *
 * Revision 1.22  2006/05/04 20:01:31  jeromel
 * Switched to logger
 *
 * Revision 1.21  2006/04/26 15:37:04  jeromel
 * mVertexOrderMethod (To be tested)
 *
 * Revision 1.20  2006/03/12 18:47:29  balewski
 * small corrections of histograms and printouts
 *
 * Revision 1.19  2006/03/12 17:01:01  jeromel
 * Minor change + use ppvNoCtbVertexFinder
 *
 * Revision 1.18  2006/03/11 04:12:49  balewski
 * 2 changes in preparation for 2006 data processing:
 * - CTB matching  ON/OFF switch activated by m_Mode 0x8 or 0x10
 * - vertex enum extension depending on CTB usage - hack in the moment, 
 *   Jerome needs to provide actual new enum
 * - BTOW calibration wil change for 2006+ from maxt eT of ~27 --> 60 GeV
 * NOTE : this new code was NOT executed - it is late, I want to get it in CVS
 * Tomorrow I'll do some tests
 * Jan
 *
 * Revision 1.17  2006/01/24 17:53:39  balewski
 * small fix
 *
 * Revision 1.16  2006/01/24 17:26:06  balewski
 * drop hardcoded mask of BTOW lower East, now it takes BTOW mask & ped+sigPed from DB
 * Watch the DB  time stamp !
 *
 * Revision 1.15  2005/09/03 16:41:53  balewski
 * bug fix: <<endm replaced with <<endl
 *
 * Revision 1.14  2005/08/30 22:08:43  balewski
 * drop '*' from declaration of   mTrackData &  mVertexData
 *
 * Revision 1.13  2005/08/17 15:07:39  balewski
 * cleanup, irrelevant for pp200 production
 *
 * Revision 1.12  2005/08/15 13:04:08  balewski
 * Z-range +/- 250 cm
 *
 * Revision 1.11  2005/08/14 23:49:55  balewski
 * smaller bins for Z-likelihood, limit siZ >=1 mm
 *
 * Revision 1.10  2005/08/12 18:35:27  balewski
 * more accurate calculation of Z-vertex error
 * by accounting for average weight of tracks contributing to the likelihood,
 *  Now errZ is of 0.5-1.5 mm, was ~2x smaller
 *
 * Revision 1.9  2005/07/28 20:57:14  balewski
 * extand zMax range to 200cm, call it PPV-2
 *
 * Revision 1.8  2005/07/27 06:08:19  balewski
 * tuning PPV cuts
 *
 * Revision 1.7  2005/07/26 02:49:08  balewski
 * more flexible for node pathologies
 *
 * Revision 1.6  2005/07/22 21:02:08  balewski
 * bug fix & cleanup
 *
 * Revision 1.5  2005/07/20 05:34:16  balewski
 * cleanup
 *
 * Revision 1.4  2005/07/19 22:01:24  perev
 * MultiVertex
 *
 * Revision 1.3  2005/07/15 20:53:25  balewski
 * cleanup
 *
 * Revision 1.2  2005/07/14 15:39:25  balewski
 * nothing, to force recompilation of this code by Autobuild
 *
 * Revision 1.1  2005/07/11 20:38:12  balewski
 * PPV added for real
 *
 *
 **************************************************************************
 **************************************************************************
 **************************************************************************/

#if 0

old scraps of code

---------------------------
  const float maxZerr=1; // cm

  fprintf(fdOut,"%3d nEve %.2f   %d %d %d    %d %d %d   %d ",mTotEve,zGeant,ctbList->getnFired(),bemcList->getnFired(),eemcList->getnFired(),mTrackData->size(),nTpcM,nTpcV,mVertexData->size());
  if(mVertexData->size()>1) 
    fprintf(fdOut,"nV "); 
  else  
    fprintf(fdOut,"nv "); 
  fprintf(fdOut," %d\n",eveID);

  float del=888;
  for(i=0;i<mVertexData->size();i++) {
    VertexData *V=&mVertexData->at(i);
    printf("%.1f:%d:%d ",V->r.z(),V->nUsedTrack,V->nAnyMatch);
    fprintf(fdOut,"     v %d  %.2f   %d %d %d   %d %d   %d %d   %d %d   %d %d",i+1,V->r.z(),V->nUsedTrack,V->nAnyMatch,V->nAnyVeto,V->nCtb,V->nCtbV,V->nBemc,V->nBemcV,V->nEemc,V->nEemcV,V->nTpc,V->nTpcV);
    
    if(fabs(V->r.z()-zGeant)<maxZerr) {
      printf("****%d ",i+1);
      fprintf(fdOut," *\n");
      del=zGeant-V->r.z();
      hA[13]->Fill(del);
    } else	
      fprintf(fdOut," ,\n");
    
  }
  printf(" del=%.1f\n",del);


===============================================================

  int PlotEveN=-1; //save Z-likelihood as histo for first events

    if(mTotEve==PlotEveN )  // for every vertex found
      plotVertex(&V);//....... save Z-likelihood plot for first events
  }
  if(mTotEve==PlotEveN ) plotTracksDca() ; // save trask used


//-------------------------------------------------
//-------------------------------------------------
void StPPVertexFinder::plotVertex(VertexData *V) {
    float z0=V->r.z();
    float ez=V->er.z();
    TH1D *h=( TH1D *) hL->Clone();
    char txt[100];
    sprintf(txt,"e%dV%d",mTotEve,V->id);
    h->SetName(txt);
    sprintf(txt,"lnLike vertZ=%d  nTr=%d eveID=%d",V->id,V->nUsedTrack,eveID);
    h->SetTitle(txt);
    h->SetAxisRange(z0-8, z0+8);
    HList->Add(h);

    TList* LL= h->GetListOfFunctions();

    // add Likelyhood shape  function
    sprintf(txt,"e%dV%dL",mTotEve,V->id);
    TF1  *fL = new TF1(txt,"[2]- (x-[0])*(x-[0])/2/[1]/[1]",-200,200);
    fL->SetParName(0,"z0");
    fL->SetParName(1,"sig");
    fL->SetParName(2,"Lmax");
    fL->SetLineColor(kRed);
    fL->SetLineWidth(1);
    fL->SetParameter(0,z0);
    fL->SetParameter(1,ez); 
    fL->SetParameter(2,V->Lmax);
    LL->Add(fL);
    TLine *ln=new TLine(z0,0,z0,V->Lmax); ln->SetLineColor(kRed); 
    LL->Add(ln);
    ln=new TLine(z0+ez,0,z0+ez,V->Lmax); ln->SetLineColor(kRed); 
    ln->SetLineStyle(2); LL->Add(ln);
    ln=new TLine(z0-ez,0,z0-ez,V->Lmax); ln->SetLineColor(kRed); 
    ln->SetLineStyle(2); LL->Add(ln);
}

//-------------------------------------------------
//-------------------------------------------------
void StPPVertexFinder::plotTracksDca() {
#if 0
    char txt[100];

    sprintf(txt,"e%dGin",mTotEve);
    TGraphErrors *g1=new TGraphErrors;
    g1->SetMarkerStyle(27);
    g1->SetName(txt);
    sprintf(txt,"in-time Z along beam @ DCA, eveID=%d; Z (cm); pT (GeV/c)",eveID);
    g1->SetTitle(txt);
    g1->SetLineColor(kBlue);
    g1->SetMarkerColor(kBlue);

    sprintf(txt,"e%dGout",mTotEve);
    TGraphErrors *g2=new TGraphErrors;
    g2->SetMarkerStyle(5);
    g2->SetName(txt);
    sprintf(txt,"out-of-time Z along beam @ DCA, eveID=%d; Z (cm); pT (GeV/c)",eveID);
    g2->SetTitle(txt);
    g2->SetLineColor(kMagenta);
    g2->SetMarkerColor(kMagenta);

    int i;
    int nt=mTrackData->size();
    for(i=0;i<nt;i++) {
      TrackData *t=&mTrackData->at(i);
      TGraphErrors *g=g1;
      if(!t->ctbMatch) g=g2; // out-of-time
      int n=g->GetN();
      g->SetPoint(n,t->zDca,t->gPt);
      g->SetPointError(n,t->ezDca,0);
    }
    HList->Add(g1);
    HList->Add(g2);
#endif
}


#endif