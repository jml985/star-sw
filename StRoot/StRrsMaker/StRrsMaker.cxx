/******************************************************
 * $Id: StRrsMaker.cxx,v 1.9 2000/02/14 01:08:02 lasiuk Exp $
 * Description:
 *  Implementation of the Maker main module.
 *
 * $Log: StRrsMaker.cxx,v $
 * Revision 1.9  2000/02/14 01:08:02  lasiuk
 * write the data set
 * add two member functions for pedestal and noise switches
 * add coordinate conditional and StCoordinateTransform
 * incorporate track_p into GHit
 *
 *
 * Revision 1.10  2000/02/15 18:07:20  lasiuk
 * check if pointer exists.  If not, return a warning status.
 *
 * Revision 1.9  2000/02/14 01:08:02  lasiuk
 * write the data set
 * add two member functions for pedestal and noise switches
 * add coordinate conditional and StCoordinateTransform
 * incorporate track_p into GHit
 *
 * Revision 1.8  2000/02/12 21:54:25  lasiuk
 * Introduce provisions to read in local coordinates
 *
 * Revision 1.7  2000/02/08 23:46:46  lasiuk
 * comment to prevent streamer for ionize and inducesignal. Remove filter
 *
 * Revision 1.6  2000/02/08 16:36:49  lasiuk
 * Bring into line with HP
 *
 * Revision 1.5  2000/01/28 20:35:08  lasiuk
 * namespace std is NOT in!
 *
 * Revision 1.4  2000/01/27 17:10:03  lasiuk
 *
 ******************************************************/
#ifdef __ROOT__
#include "StRrsMaker.h"             

#include <iostream.h>
#define rICH_DIAGNOSTIC 1
#define rICH_DECODE_DATA 1
#define rICH_WITH_PADMONITOR 1
#define rICH_WITH_RINGS 1

#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_ObjectSet.h"

//#include <iostream.h>
#include "StMemoryInfo.hh"
#endif
#include "StParticleTable.hh"

// DataBases
#include "StRichGeometryDb.h"
#include "StRichPhysicsDb.h"
#include "StRichMomentumTransform.h"
#include "StRichGeantReader.h"
#include "StRichPadPlane.h"
#include "StRichWriter.h"
#endif
// #include "StRichRingDefinition.h"
// #include "StRichTrack.h"
// #include "StRichRingPoint.h"
// #include "StRichRingCalculator.h"
// #include "StParticleDefinition.hh"
// #include "StParticleTypes.hh"
// #endif
//////


#ifdef RICH_DECODE_DATA
#include "StRrsReader.h"
// SCL
#include "StGlobals.hh"
#include "StThreeVector.hh"
#endif

#ifdef  RICH_WITH_VIEWER
#include "StRichViewer.h"              // view class
#endif


// g2t tables
#include "tables/St_g2t_tpc_hit_Table.h"
#include "tables/St_g2t_rch_hit_Table.h"
#include "tables/St_g2t_track_Table.h"


// Magnetic Field
#ifdef __ROOT__
#define gufld   gufld_
//#define gufld   GUFLD
extern "C" {void gufld(Float_t *, Float_t *);}
#endif

ClassImp(StRrsMaker)          // Root Macro

StRrsMaker::StRrsMaker(const char *name)
    : StMaker(name), mUseLocalCoordinate(0), mAddPedestal(0), mAddElectricNoise(0)
{     }

StRrsMaker::~StRrsMaker()
{  /* nopt */ }

int StRrsMaker::readFile(char* file)
{
    mInputFileName = file;
    mReadFromFile = 1;
    PR(mInputFileName);
    PR(mReadFromFile);
    return kStOK;
}

int StRrsMaker::writeFile(char* file, int numEvents)
{
    mOutputFileName = file;
    mNumberOfEvents = numEvents;
    mWriteToFile = 1;
    PR(mNumberOfEvents);
    PR(mOutputFileName);
    return kStOK;
}

//
// Flags that are macro settable
// Inital value is 0!
void StRrsMaker::useLocalCoordinate(int b)
{
    mUseLocalCoordinate = b;
}

void StRrsMaker::addPedestal(int b)
{
    mAddPedestal = b;
}

void StRrsMaker::addElectricNoise(int b)
{
    mAddElectricNoise = b;
}
///////////////////////////////////////////////////////
 
{
#ifdef USE_MEMORY_INFO
    StMemoryInfo* info = StMemoryInfo::instance();
    info->snapshot();
    info->print();
#endif
    // DBs are here
    mPhysicsDb  = StRichPhysicsDb::getDb();
    mGeometryDb = StRichGeometryDb::getDb();

    if ( !mGeometryDb ) {
      cerr << "Geometry database could not be initialized. Aborting!!!\n";
      return 1;
      cerr << "Physics database could not be initialized. Aborting!!!\n";
    //mPhysicsDb->print();

    
    // GEANT Table
    // ADC
    mCoordinateTransform = StRichCoordinateTransform::getTransform(mGeometryDb);
    // adds a DC level to each pad
    mADC.setAddPedestal(mAddPedestal);
    
    // PadPlane
    

    //
    // Construct constant data set.  This is what is passed downstream
    //

    mPadPlane =
	new StRichPadPlane(mGeometryDb->numberOfRowsInAColumn(),
			   mGeometryDb->numberOfPadsInARow());
    AddConst(new St_ObjectSet("richPixels", mPadPlane));

    mWriter = StRichWriter::getInstance(mPadPlane);
    //AddData(mPadPlane);

    // Construct constant data set.  This is what is passed downstream
    // The processors
    AddConst(new St_ObjectSet("richPixels", mPadPlane));
    // ASG        is a SINGLETON
    mAnalogSignalGenerator = StRichAnalogSignalGenerator::getInstance(mWriter);
#ifdef USE_MEMORY_INFO
    info->snapshot();
    info->print();
#endif
	     
    return Init(true);
}

Int_t StRrsMaker::Init(int useHistos)
{
#ifdef RICH_WITH_VIEWER
    // Viewer is here
    cout << "Try make a viewer" << endl;
    StRichViewer* view = 0;
    StRichViewer::histograms = useHistos;
    StRichViewer::foo = useHistos;
    if (StRichViewer::histograms )
	view = StRichViewer::getView();

    if ( !view ) {
	cerr << "No histograming will be done.\n";
	StRichViewer::histograms = 0;
    }
#endif

    return kStOk;
}

int StRrsMaker::whichVolume(int val, string* vName)
{
    //
    // coding from GEANT is:
    //    volume+Isys*1000
    // where:
	break;
    case 2:
	*vName = string("RCSI");
	break;
    case 3:
	*vName = string("QUAR");
	break;
    case 4:
	*vName = string("FREO");
	break;	
    default:
	*vName = string("");
    return volumeNumber;
//     cout << "-- Press return to continue -- ";
    ofstream raw("/afs/rhic/star/users/lasiuk/data/rrs.txt");
//       char c = cin.get();
#ifdef USE_MEMORY_INFO
    StMemoryInfo* info = StMemoryInfo::instance();
    info->snapshot();
    info->print();
    list<StRichMiniHit*>::iterator iter;
    
    //
    // scope for transformed momentum
    //
    StThreeVector<double> lTrackMomentum;
    StThreeVector<double> gTrackMomentum;

    
    else {  // else
    // Either  Read mPadPlane from file
    //
    if (mReadFromFile) {
	//mInputStream->fillTrsEvent(mPadPlane);
	cout << "Done Filling mPadPlane from File" << endl;
    }
    //
    // or do the normal processing of RRS from GEANT
    else {  // else process from stream
	if (!m_DataSet->GetList())  {
	    //if DataSet is empty fill it
	    //
	    // Read the GEANT info
	    St_g2t_track *g2t_track =
	    PR(numberOfTracks);

 	    if(!g2t_track){
	    // TPC HITS
		cout << "\tNo g2t_tpc_hit pointer";
		cout << "\treturn from StRrsMaker::Make()" << endl;
		return kStWarn;
	    // can we check if the dataset exists?
	    St_g2t_rch_hit *g2t_rch_hit =
		static_cast<St_g2t_rch_hit *>(geant("g2t_rch_hit"));
	    if(!g2t_rch_hit){
		cout << "StRrsMaker::Make()";
		cout << "\tNo g2t_rch_hit pointer";
		cout << "\treturn from StRrsMaker::Make()" << endl;
	    ///////////////////////
	    //numberOfRichHits = 10;
	    StRichCoordinateTransform tmpTform(mGeometryDb);
	    PR(numberOfRichHits);
	    ///////////////////////
	    string volumeName;
	    int    quadrant;

		StThreeVector<double>
		    momentum(rch_hit->p[0],rch_hit->p[1],rch_hit->p[2]);

		// Input is in local or global coordinates
		    StGlobalCoordinate global(rch_hit->x[0], rch_hit->x[1], rch_hit->x[2]);
		    tmpTform(global,local);
		//
		    //
		    StGlobalCoordinate global(rch_hit->x[0]*centimeter,
		    StRichQuadrantCoordinate quad(rch_hit->x[0],rch_hit->x[1],rch_hit->x[2],quadrant);
		    tmpTform(quad,local);
		// and x-> -x, y-> -y
		else {
		    local.position().setX(rch_hit->x[0]);
		    local.position().setY(rch_hit->x[1]);
		    local.position().setZ(rch_hit->x[2]);
		    // z-component okay
		}
		hit.fill(local.position().x(), local.position().y(), local.position().z(),
		    local.position().setZ(rch_hit->x[2]*centimeter);
			 (momentum.x()/abs(momentum))*GeV,
			 (momentum.y()/abs(momentum))*GeV,
			 (momentum.z()/abs(momentum))*GeV,
		
		    (!mTable->findParticleByGeantId((track[(rch_hit->track_p)-1].ge_pid))) ?
		    0. : mTable->findParticleByGeantId((track[(rch_hit->track_p)-1].ge_pid))->mass();
		    
		hit.fill(local.position(),
		    gTrackMomentum.setZ(track[(rch_hit->track_p)-1].p[2]*GeV);
		    mMomentumTransform->localMomentum(gTrackMomentum,lTrackMomentum);
		raw << "volumeName= " << volumeName.c_str() << endl;
		raw << "volume_id= "  << rch_hit->volume_id << endl;
		raw << "hit= "        << hit                << endl;
		raw << "p= "          << abs(momentum)
		    << " track id= "  << rch_hit->id
		    << " tpchit= "    << track[(rch_hit->track_p-1)].n_tpc_hit
		    << " eg_lab= "    << track[(rch_hit->track_p-1)].eg_label
		    << " egpid= "     << track[(rch_hit->track_p-1)].eg_pid << endl;
// 			    << tpc_hit[zz].x[1] << " "
// 			    << tpc_hit[zz].x[2] << " ";
// 			double ptot = (sqrt(tpc_hit[zz].p[0]*tpc_hit[zz].p[0]+
// 		if ( hit.volumeID() != "RCSI" ) { 
		if ( hit.volumeID() == "RGAP" ) { 
		    mIonize( hit );		 
		
		else {
#endif
		    // check if it is photon

			mInduceSignal ( hit );
		    // Check if it is photon, and induce signal if so
		    //
// 		    cout << "RGAP" << "ii/size " << ii << " " << theList.size() << endl;
			mInduceSignal(hit);
			theList.push_back(new StRichMiniHit(hit.position(),
							    hit.momentum(),
		
		    }
		else {
		}
    } //else
		iter != theList.end();
		wireNumber = mWireSelector.whichWire(*iter);
		chargeMultiplied = mAmplification.avalanche(*iter, wireNumber, theList);
		mAnalogSignalGenerator->induceSignal(*iter,chargeMultiplied);

	    }
	    
	}  // if (m_DataSet)

    } //else process from stream


    //cout << "Try Write" << endl;
    for ( int i = 0; i < mWriter->rows(); i++ )
	for ( int j = 0; j < mWriter->cols(); j++ ) {

	    if(mAddElectricNoise)
		mWriter->getSignal(i,j).signal +=  mNoiseSimulator();
	    
	    mWriter->getSignal(i,j).signal =
	cout << "StRrsMaker::Maker() Write DATA out" << endl; 
#ifdef RICH_WITH_VIEWER
	    if (StRichViewer::histograms )
		StRichViewer::getView()->mADCSignal->Fill(i,j,mWriter->getSignal(i,j).signal);
#endif
	}	      

    
	iter++) {
	*iter = 0;
    thePadMonitor->clearPads();

#ifdef RICH_DECODE_DATA
    int version = 1;

#ifdef RICH_WITH_PADMONITOR
    cout << "Get Instance of Pad Monitor" << endl;
	    theADCValue = theReader.GetADCFromCoord(iRow,iCol);
    cout << "Try Clear" << endl;
		//cout << "r/c/adc: " << iRow << ' ' << iCol << ' ' << theADCValue << endl;

    cout << "DECODER " << endl;
    int ctr = 0;
    for(int iRow=0; iRow<(mGeometryDb->numberOfRowsInAColumn()); iRow++) {
	for(int iCol=0; iCol<(mGeometryDb->numberOfPadsInARow()) ; iCol++) {
#ifdef RICH_DIAGNOSTIC
		raw << "r/c/adc: "
		    << iRow << ' '
		    << iCol << ' ' << theADCValue << endl;
		anIDList MCInfo = theReader.GetMCDetectorInfo(iRow, iCol);
		anIDList::iterator iter;
		for(iter = MCInfo.begin();
		    iter!= MCInfo.end();
		    iter++) {
#ifdef __SUNPRO_CC
		    raw << ">>* MCinfo.G_ID= "
			<< (*iter).mG_ID << "MCinfo.trackp= "
			<< (*iter).mTrackp << "MCinfo.amount= "
			<< (*iter).mAmount << endl;
#else
		    raw << ">>* MCinfo.G_ID= "
			<< iter->mG_ID << "MCinfo.trackp= "
			<< iter->mTrackp << "MCinfo.amount= "
			<< iter->mAmount << endl;
#endif
		}
#endif
		
// 			<< (*iter).mAmount << endl;
// #else
	    thePadMonitor->drawPads();
// 			<< iter->mG_ID << "MCinfo.trackp= "
// 			<< iter->mTrackp << "MCinfo.amount= "
// 			<< iter->mAmount << endl;
//     StThreeVector<double> bPoint;


//     for(int kk=90; kk<270;kk+=5) {
// 	bool status = myCalculator.getRing(eInnerRing)->getPoint(kk*degree, aPoint);
// 	thePadMonitor->addInnerRingPoint(aPoint.x(), aPoint.y());
// #endif
    thePadMonitor->update();
#endif
#ifdef USE_MEMORY_INFO
    info->snapshot();
    info->print();
#endif

    return 0;
}


    delete mGeometryDb;
    delete mPhysicsDb;

int StRrsMaker::Finish()
{
    delete mWriter;
#ifdef RICH_WITH_VIEWER
    delete StRichViewer::getView();
#endif
    delete mPadPlane;

    return 0;
  } 

void StRrsMaker::drawParticleId()      // in Filter 
{
#ifdef RICH_WITH_VIEWER

    StRichViewer::getView()->mParticleId->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
  }

void StRrsMaker::drawWhichQuadrant()   // in Filter 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mWhichQuadrant->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}


void StRrsMaker::drawClusterElectrons()// in Ionization 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mClusterElectrons->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawErrorDetection()  // in Filter 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mErrorDetection->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawWhichWire()       // in SelectWire 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mWhichWire->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawFeedback()        // in Gas Gain 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mFeedback->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawPolia()           // in GasGain 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mPolia->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawAnalogSignals()       // in ASG 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mAnalogSignals->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawTotalCharge()     // in ASG 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mTotalCharge->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawADCSignal()       // in ADC
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mADCSignal->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawPadPlane()        // displays the Pad plane (Filter)
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mPadPlane->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

void StRrsMaker::drawNoise()           // electrical noise simulation 
{
#ifdef RICH_WITH_VIEWER
    StRichViewer::getView()->mNoise->Draw();
    StRichViewer::getView()->mCanvas1->Modified();
    StRichViewer::getView()->mCanvas1->Update();
    StRichViewer::getView()->mHFile->Write();
#endif
}

//   St_DataSet * StRrsMaker::getPadPlaneTable() {
//     Writer* output = Writer::getInstance();
//     return output->getPadsTable(); 
//   }
//   St_DataSet * StRrsMaker::getIDTable() {
//     Writer* output = Writer::getInstance();
//     return output->getIDTable(); 
//   }
//   int StRrsMaker::getADC(int row, int col) {
//     Writer* output = Writer::getInstance();
//     return output->getADC(row,col);
//   }

#endif // __ROOT__
