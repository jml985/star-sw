//  StiMaker.cxx
// M.L. Miller
// 5/00

#include <iostream.h>
#include <math.h>
#include <string>

// StRoot
#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "StMessMgr.h"

// SCL
#include "SystemOfUnits.h"
#include "PhysicalConstants.h"

// StEvent
#include "StEventTypes.h"

// Sti
#include "Sti/StiHitContainer.h"
#include "Sti/StiHitFactory.h"
#include "Sti/StiHitFiller.h"
#include "Sti/StiDetectorLayerContainer.h"
#include "Sti/StiDrawableDetector.h"

// StiMaker
#include "StiDisplayManager.h"
#include "StiMaker.h"

StiMaker* StiMaker::sinstance = 0;
bool StiMaker::mdone = true;
int StiMaker::mcounter = 0;

ClassImp(StiMaker)
  
StiMaker::StiMaker(const Char_t *name) : StMaker(name)
{
    cout <<"StiMaker::StiMaker()"<<endl;
    sinstance = this;
}

StiMaker* StiMaker::instance()
{
    return (sinstance) ? sinstance : new StiMaker();
}

void StiMaker::kill()
{
    if (sinstance) {
	delete sinstance;
	sinstance = 0;
    }
    return;
}

StiMaker::~StiMaker() 
{
    delete mhitstore;
    mhitstore = 0;
    delete mhitfactory;
    mhitfactory = 0;
    delete mhitfiller;
    mhitfiller = 0;
    delete mdisplay;
    mdisplay = 0;
    StiDetectorLayerContainer::kill();
    mdetector = 0;
}

void StiMaker::Clear(const char*)
{
    mhitstore->clear();
    StMaker::Clear();
}

Int_t StiMaker::Finish()
{
    return StMaker::Finish();
}

Int_t StiMaker::Init()
{
    mdisplay = StiDisplayManager::instance(); //Must come before anything that you want to be drawn

    mhitstore = new StiHitContainer();
    mhitfactory = new StiHitFactory();
    mhitfiller = new StiHitFiller();

    mhitfiller->addDetector(kTpcId);
    //mhitfiller->addDetector(kSvtId);
    //cout <<"Hits used from detectors:\t"<<*mhitfiller<<endl;

    cout <<"cd()"<<endl;
    mdisplay->cd();
    cout <<"Draw Display"<<endl;
    mdisplay->draw();
    cout <<"Update Display"<<endl;
    mdisplay->update();

    const char* buildfile = "/star/rcf/pwg/spectra/mmiller/StiGeometryParameters/Detectors/";
    mdetector = StiDetectorLayerContainer::instance();
    mdetector->setSectors(1, 3);
    mdetector->setPadrows(1, 45);
    mdetector->buildReset();
    while (mdetector->hasMoreToBuild()) {
	mdetector->buildNext(buildfile);
	//mdisplay->draw();
	//mdisplay->update();
    }
    mdetector->reset();
    
    mdisplay->draw();
    mdisplay->update();
    
    return StMaker::Init();
}

Int_t StiMaker::Make()
{
    /*
      StEvent* rEvent = 0;
      rEvent = (StEvent*) GetInputDS("StEvent");
      if (rEvent) {
      mevent = rEvent;
      
      cout <<"\n---------- StiMaker::Make() ------------\n"<<endl;
      cout <<"Number of Primary Vertices:\t"<<mevent->numberOfPrimaryVertices()<<endl;
      mhitfiller->setEvent(mevent);
      mhitfiller->fillHits(mhitstore, mhitfactory);
      }
    */

    
    return kStOK;
}

void StiMaker::reset()
{
    mdone=false;
    mcounter=0;
    StiDetectorLayerContainer::instance()->reset();
}

void StiMaker::doNextAction()
{
    if (mdone) {
	cout <<"StiMaker::doNext()\t Nothing Left to do"<<endl;
	return;
    }

    StiDetectorLayerContainer& rdet = *(StiDetectorLayerContainer::instance());
    const StiDrawableDetector* layer = dynamic_cast<const StiDrawableDetector*>(*rdet);
    if (!layer) return;
    StiDisplayManager::instance()->setVisible(layer);
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
    bool cangofurther = rdet.padrowStepMinus();
    if (!cangofurther) {
	rdet.setRefDetector( layer->getSector()+1 );
    }
    //if (!cangofurther) mdone = true;
    
    //cout <<"StiMaker::doNext()\t mcounter:\t"<<mcounter<<endl;
    //++mcounter;
    //if (mcounter>10) mdone=true;

    return;
}

bool StiMaker::hasMore()
{
    return (!mdone);
}

