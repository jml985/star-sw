/***************************************************************************
 *
 * $Id: StTofMaker.cxx,v 1.15 2004/09/10 22:09:21 perev Exp $
 *
 * Author: W.J. Llope / Wei-Ming Zhang / Frank Geurts
 *
 ***************************************************************************
 *
 * Description: TOF offline software: maker to insert TOFp raw data onto DST
 *
 ***************************************************************************
 *
 * $Log: StTofMaker.cxx,v $
 * Revision 1.15  2004/09/10 22:09:21  perev
 * more defence agains corrupted DAQ data
 *
 * Revision 1.14  2004/01/27 23:17:01  dongx
 *  change for year4 run (pVPD+TOFp+TOFr')
 *  - Additional TOFr' ADC and TDC channels put in
 *  - Add TOTs of TOFr' in
 *
 *
 * Revision 1.12  2003/09/17 19:54:28  geurts
 * zeroed geometry pointer
 *
 * Revision 1.11  2003/08/08 00:20:41  geurts
 * moved local collection code to StTofUtil, changed StTofMaker accordingly
 *
 * Revision 1.10  2003/07/18 18:31:49  perev
 * test for nonexistance of XXXReader added
 *
 * Revision 1.9  2003/02/06 05:02:05  geurts
 * Added TOFr and extra pVPD-ADC channels to the datastream:
 * StTofMaker is now aware of year2 (TOFp+pVPD) and year3 (TOFp+pVPD+TOFr) raw data.
 *
 * Revision 1.8  2002/01/22 06:50:34  geurts
 * modifications for STAR dBase access and doxygenized
 *
 * Revision 1.7  2001/10/09 03:06:38  geurts
 * TofTag introduced
 *
 * Revision 1.6  2001/10/07 19:01:46  geurts
 * change default operation mode to DAQ-reader
 *
 * Revision 1.5  2001/10/05 21:08:40  geurts
 * clean-up histograms and private root file
 *
 * Revision 1.4  2001/09/28 18:40:03  llope
 * first release
 *
 *
 **************************************************************************/
//! Time-of-Flight Raw Data Maker
/*! \class StTofMaker
    \author  W.J. Llope, Wei-Ming Zhang, Frank Geurts

    <p>TOF offline software. This maker inserts the TOFp raw data onto the DST.
    It distinguishes between so-called beam and strobe event. If available
    the raw data is unpacked into a tofDataCollection() which is part of the
    StEvent::tofCollection(). Based on a rough estimate of the number of hits
    a tofTag is constructed and stored.</p>

    <p>History:
    <ul>
    <li> StTofDataCollection added </li>
    <li> ADC and TDC data for pVPD and Ramp now being saved </li>
    <li> Confirmed proper fill of DataCollection </li>
    <li> TOFP_DEBUG switch added to StTofMaker.h for print statements  </li>
    <li> Changed or Prettified all print statements (TOFP_DEBUG directive) </li>
    <li> Changed histograms filled & saved (TOFP_HISTOS directive) </li>
    <li> Overloaded << operator for dumping data to screen  </li>
    <li> Added direct check of StTofCollection entries in fillStEvent()  </li>
    <li> Added tofTag variable </li>
    </ul>
    </p>
*/
#include "StTofMaker.h"
#include <stdlib.h>
#include "StEventTypes.h"
#include "StTofUtil/StTofGeometry.h"
#include "StTofUtil/StTofDataCollection.h"
//VP#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDaqLib/TOF/TOF_Reader.hh"
#include "StDAQMaker/StDAQReader.h"
#include "tables/St_TofTag_Table.h"
#include "TFile.h"
#include "TH1.h"

ClassImp(StTofMaker)


/// default constructor
StTofMaker::StTofMaker(const char *name):StMaker(name) {
  drawinit=kFALSE;
  mTofGeom = 0;
 }

/// default empty destructor
StTofMaker::~StTofMaker(){/* nope */}



/// Init method, book histograms
Int_t StTofMaker::Init(){
  //  create histograms
  nAdcHitHisto = new TH1S("tof_nadchit","Crude No. ADC Hits/Event",181,-0.5,180.5);
  nTdcHitHisto = new TH1S("tof_ntdchit","Crude No. TDC Hits/Event",185,-0.5,184.5);
  return StMaker::Init();
}



/// InitRun method, (re)initialize TOFp data from STAR dBase
Int_t StTofMaker::InitRun(int runnumber){
  cout << "StTofMaker::InitRun  -- initializing TofGeometry --" << endl;
  mTofGeom = new StTofGeometry();
  mTofGeom->init(this);
  return kStOK;
}



/// FinishRun method, clean up TOFp dBase entries
Int_t StTofMaker::FinishRun(int runnumber){
  cout << "StTofMaker::FinishRun -- cleaning up TofGeometry --" << endl;
  if (mTofGeom) delete mTofGeom;
  mTofGeom=0;
  return 0;
}



//_________________________________________________________________________
/// Make method, check for collections; create and fill them according to m_Mode
Int_t StTofMaker::Make(){
  // 
  cout << "StTofMaker::Make() -- All Your Base Are Belong To Us --"  << endl;

  mDataCollection = new StTofDataCollection; 
  mTofCollectionPresent  = 0;
  mDataCollectionPresent = 0;

//--- check for existence of collections.................
//
  tofTag = -99;
  mEvent = (StEvent *) GetInputDS("StEvent");
  if (mEvent) {
    cout << "StTofMaker Checking for Tof and TofData collections ..." << endl;
    mTheTofCollection = mEvent->tofCollection();
    if(mTheTofCollection) {
      mTofCollectionPresent = 1;
      cout << " + tofCollection Exists" << endl;
//--
      if(mTheTofCollection->dataPresent()) {
	cout << " + tofDataCollection Exists" << endl;
	mDataCollectionPresent = 1;
      }
      else cout << " - tofDataCollection does not exist" << endl;
//--
    } else {
      cout << " - tofCollection does not exist" << endl;
      cout << " - tofDataCollection does not exist" << endl;
    }
//-- create tofcollection...
    if (!mTheTofCollection){
      cout << "StTofMaker Creating tofCollection in StEvent ... ";
      mTheTofCollection = new StTofCollection();
      mEvent->setTofCollection(mTheTofCollection);
      mTheTofCollection = mEvent->tofCollection();
      if (mTheTofCollection) cout << "OK"<< endl;
      else                   cout << "FAILED" << endl;
    } 	// end !mTheTofCollection check
  }   	// end mEvent check
  else cout << "StTofMaker   No StEvent structures detected ... not good!" << endl;
//
//--- end check for collections....

// check if slatCollection not already available, get data if not
  if(!mDataCollectionPresent) {
    if (m_Mode ==0) {             // default DAQ-reader setting 
      cout << "StTofMaker Will get real data from TOFpDAQ Reader ... " << endl;
      mTheTofData = GetDataSet("StDAQReader");
      if (!mTheTofData) {
	cout << "StTofMaker No StDAQReader dataset. Event skipped" << endl;
	return kStWarn;
      }
      mTheDataReader = (StDAQReader*)(mTheTofData->GetObject()); 
      if (!mTheDataReader) {
	cout << "StTofMaker No StDAQReader object. Event skipped" << endl;
	return kStWarn;
      }
      if (!(mTheDataReader->TOFPresent())) {
	cout << "StTofMaker TOF is not in datastream. Event skipped" << endl;
	return kStWarn;
      }
//
      mTheTofReader = mTheDataReader->getTOFReader();
      if (!mTheTofReader) {
	cout << "StTofMaker Failed to getTofReader()...." << endl;
	return kStWarn;
      }

#ifdef TOFP_DEBUG
      TOF_Reader* MyTest = dynamic_cast<TOF_Reader*>(mTheDataReader->getTOFReader());
      if (MyTest) MyTest->printRawData();
      if (MyTest->year2Data()) cout << "StTofMaker: year2 data - TOFp+pVPD" << endl;
      if (MyTest->year3Data()) cout << "StTofMaker: year3 data - TOFp+pVPD+TOFr" << endl;
      if (MyTest->year4Data()) cout << "StTofMaker: year4 data - TOFp+pVPD+TOFrprime" << endl;
#endif

//
//--- copy daq data to collection. (TOFp and pVPD)
//
      int nadchit=0;
      int ntdchit=0;
      int iStrobe = 0;

      // set limits on basic event statistics
      int tdcHitMax=1600;
      int pvpdStrobeMin=1500;
      if (mTheTofReader->year3Data()||mTheTofReader->year4Data()){
	tdcHitMax=880;
	pvpdStrobeMin=950;
      }

      for (int i=0;i<48;i++){
	unsigned short slatid = mTofGeom->daqToSlatId(i);
	unsigned short rawAdc = mTheTofReader->GetAdcFromSlat(i);
	unsigned short rawTdc = mTheTofReader->GetTdcFromSlat(i);  
	short rawTc  = 0;
	if (i<32) rawTc = mTheTofReader->GetTc(i);
	unsigned short rawSc = 0;
	if (i<12) rawSc = mTheTofReader->GetSc(i);
	StTofData *rawTofData = new StTofData(slatid,rawAdc,rawTdc,rawTc,rawSc);
	mDataCollection->push_back(rawTofData);      // local collection
	if(i<41) {
	  if (rawAdc>  50) nadchit++; 
	  if (rawTdc<tdcHitMax) ntdchit++; 
	}
	if(i>41) {
	  if (rawTdc>pvpdStrobeMin) iStrobe++; 
	}
      }    
      if (mTheTofReader->year3Data()){
	for (int i=48;i<132;i++){
	  //arbitrary id, start from at 100.
	  unsigned short id = (100-48)+i;
	  unsigned short rawAdc = mTheTofReader->GetAdc(i);
	  unsigned short rawTdc = 0;
	  if (i<120) rawTdc = mTheTofReader->GetTdc(i);  
	  StTofData *rawTofData = new StTofData(id,rawAdc,rawTdc,0,0);
	  mDataCollection->push_back(rawTofData);
	  if ((i>59) && (rawAdc>50))    nadchit++;
	  if ((i<120) && (rawTdc<tdcHitMax)) ntdchit++; 
	}
      }

      if (mTheTofReader->year4Data()){
	for (int i=48;i<184;i++){
	  //arbitrary id, start from at 100.
	  unsigned short id = (100-48)+i;
	  unsigned short rawAdc = 0;
	  if(i<180) {
	    rawAdc = mTheTofReader->GetAdc(i);
	  }

	  unsigned short rawTdc = 0;
	  rawTdc = mTheTofReader->GetTdc(i);

	  StTofData *rawTofData = new StTofData(id,rawAdc,rawTdc,0,0);
	  mDataCollection->push_back(rawTofData);
	  if ((i>59) && (rawAdc>50))    nadchit++;
	  if ((rawTdc<tdcHitMax)) ntdchit++; 
	}
      }

//--- end loop over data words...
//
//--- 
      if (iStrobe>4) {
	cout << "StTofMaker ... Strobe Event:" << iStrobe << endl;
	tofTag = -1;     	// set tag to show this was a strobe event   
	nAdcHitHisto->Fill(-1.);
 	nTdcHitHisto->Fill(-1.);
      } else {
	cout << "StTofMaker ... Beam Event: ~" << nadchit << " ADC and ~" 
	     << ntdchit << " TDC Raw Hits in TRAY" << endl;
	tofTag = nadchit;	// set tag to show this was physics event (tofTag>=0)
	nAdcHitHisto->Fill(nadchit);
	nTdcHitHisto->Fill(ntdchit);
      }
//---- 
//
    }
    else if(m_Mode == 1)        // no action
      cout << "StTofMaker No special action required for DataCollection"<<endl;
    else {
      cout << "WRONG SetMode (" << m_Mode << ")! "
	   << "... should be either 0 (DAQ reader) or 1(no action, SIM)" << endl;
      return kStOK;
    }
//--- end m_mode check....

  }  
//--- end section if !mDataCollectionPresent

  cout << "StTofMaker tofTag = " << tofTag << endl;
  if (mEvent) storeTag();

//--- fill StEvent, clean-up, and return...
//
  if (mEvent) this->fillStEvent();
  delete mDataCollection;
  cout << "StTofMaker::Make() -- see you next event --" << endl;

  return kStOK;
}
              

//_________________________________________________________________________
/// Fill and store TOF Collections in StEvent. Create TofCollection if necessary
void StTofMaker::fillStEvent() {

  cout << "StTofMaker::fillStEvent() Starting..." << endl;

//--- make sure we have a tofcollection
  if(!mTheTofCollection){
    mTheTofCollection = new StTofCollection();
    mEvent->setTofCollection(mTheTofCollection);
  }

//--- fill mTheTofCollection with ALL RAW DATA 
  if(mDataCollectionPresent != 1) {
    cout << "StTofMaker::fillStEvent()  Size of mDataCollection = " << mDataCollection->size() << endl;
    for(size_t jj = 0; jj < mDataCollection->size(); jj++)
      mTheTofCollection->addData(mDataCollection->getData(jj));
  }

//--- verify existence of tofCollection in StEvent (mEvent) 
//
// also check contents of StTofRaws filled in TofCollection. This kind of check
// has never been done for StTofMCSlat too! (We only verified in local
// collections) Here, we would just make sure that raw data is really in
// TofCollection.  WMZ
//
  cout << "StTofMaker::fillStEvent() Verifying TOF StEvent data ..." << endl;
  StTofCollection* mmTheTofCollection = mEvent->tofCollection();
  if(mmTheTofCollection){
    cout << " + StEvent tofCollection Exists" << endl;
    if(mmTheTofCollection->dataPresent()) {
      cout << " + StEvent TofDataCollection Exists" << endl;
      StSPtrVecTofData& rawTofVec = mmTheTofCollection->tofData();
#ifdef TOFP_DEBUG
      for (size_t i = 0; i < rawTofVec.size(); i++) {
	StTofData* p = rawTofVec[i];
	cout << *p;
      }
#endif                      
      cout << "   StEvent TofDataCollection has " << rawTofVec.size() << " entries..." << endl;
    }
    else cout << " - StEvent TofDataCollection does not Exist" << endl;
  }
  else {
    cout << " - StEvent tofCollection does not Exist" << endl;
    cout << " - StEvent tofDataCollection does not Exist" << endl;
  }
}

//_____________________________________________________________________________
/// store tofTag in StEvent structure.
void StTofMaker::storeTag(){
  // instantiate new TofTag class
  St_TofTag *tagTable = new St_TofTag("TofTag",1);
  if (!tagTable) return;
  tagTable->SetNRows(1);

  // add table to root .data directory
  AddData(tagTable,".data");
  TofTag_st *pTofTag = tagTable->GetTable();

  if (pTofTag) {
    pTofTag->tofEventType = tofTag;
    cout << "StTofMaker::storeTag()  tofTag stored" << endl;
  }
  else
    cout << "StTofMaker::storeTag()  unable to store tofTag" << endl;

}

//_____________________________________________________________________________
/// default Finish method (empty)
Int_t StTofMaker::Finish(){
  // nope 
  return kStOK;
}
