#include "StMessMgr.h"
#include "StDetectorDbTriggerID.h"
#include "tables/St_triggerID_Table.h"
#include "tables/St_trigPrescales_Table.h"
#include "tables/St_L0TriggerInfo_Table.h"
#include "tables/St_defaultTrgLvl_Table.h"
#include "TUnixTime.h"

/*!
   This class provides an interface to triggerID's from the database.

*/

/// Initialize Instance
StDetectorDbTriggerID* StDetectorDbTriggerID::sInstance = 0;

/// Returns previous instance if exits, if not makes new one
StDetectorDbTriggerID* StDetectorDbTriggerID::instance()
{

    if(!sInstance){
	sInstance = new StDetectorDbTriggerID();
    }
    // Need to reinitilize array for some unkown reason
    // the array memeory changed althoug the TTable stays the same
    if(sInstance->mIDTable)
	sInstance->mTriggerID = (triggerID_st*)(sInstance->mIDTable->GetArray());
    if(sInstance->mSTable)
	sInstance->mTrigPrescales = (trigPrescales_st*)(sInstance->mSTable->GetArray());
    if(sInstance->mL0Table)
	sInstance->mL0TriggerInfo = (L0TriggerInfo_st*)(sInstance->mL0Table->GetArray());
    if(sInstance->mDefTrgLvlTable)
	sInstance->mDefaultTriggerLevel = (defaultTrgLvl_st*)(sInstance->mDefTrgLvlTable->GetArray());
    
    return sInstance;
};

/// Updates data in instance from database
void StDetectorDbTriggerID::update(StMaker* maker){

    mMaker = maker;
    
    if(maker){
	
	// TTable of triggerID	
	TDataSet* dataSet = maker->GetDataBase("RunLog/onl");
	
	if(dataSet){
	    mIDTable = dynamic_cast<TTable*>(dataSet->Find("triggerID"));
	    
	    if(mIDTable){
		mIDNumRows = mIDTable->GetNRows();
		mTriggerID = (triggerID_st*)(mIDTable->GetArray());
	    }
	    // TTable of trigPrescales	
	    mSTable = dynamic_cast<TTable*>(dataSet->Find("trigPrescales"));
	    
	    if(mSTable){
		mSNumRows = mSTable->GetNRows();
		mTrigPrescales = (trigPrescales_st*)(mSTable->GetArray());
            }

	    // TTable of L0TriggerInfo	
	    mL0Table = dynamic_cast<TTable*>(dataSet->Find("L0TriggerInfo"));
	    
	    if(mL0Table){
		mL0NumRows = mL0Table->GetNRows();
		mL0TriggerInfo = (L0TriggerInfo_st*)(mL0Table->GetArray());
            }

	}

	dataSet = 0;
	dataSet = maker->GetDataBase("Calibrations/trg");
	
	if(dataSet){
	    // TTable of default trigger levels
	    mDefTrgLvlTable = dynamic_cast<TTable*>(dataSet->Find("defaultTrgLvl"));
	    if(mDefTrgLvlTable){
		mDefaultTriggerLevel = (defaultTrgLvl_st*)(mDefTrgLvlTable->GetArray());
	    }
	}
	
	
    }
};

/// Default constructor
StDetectorDbTriggerID::StDetectorDbTriggerID(){
    ~gMess << "StDetectorDbTriggerID::StDetectorDbTriggerID" << endm;
    mTriggerID = 0;
    mIDNumRows = 0;
    mIDTable = 0;

    mTrigPrescales = 0;
    mSNumRows = 0;
    mSTable = 0;

    mL0TriggerInfo = 0;
    mL0NumRows = 0;
    mL0Table = 0;

    mDefaultTriggerLevel = 0;
    mDefTrgLvlTable = 0;
    
};

/// triggerID members
/// Returns Number of Entries in database
unsigned int StDetectorDbTriggerID::getIDNumRows(){
    return mIDNumRows;
};

/// Returns Run Number
unsigned int StDetectorDbTriggerID::getIDRunNumber(){
    unsigned int value = 0;
    if(mTriggerID)
	value = mTriggerID[0].runNumber;
    return value;
};

/// Default destructor
StDetectorDbTriggerID::~StDetectorDbTriggerID(){
  delete sInstance;
  sInstance = 0;
};

/// Returns idxTrg
unsigned int StDetectorDbTriggerID::getIdxTrg(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].idxTrg;
    return value;
};

/// Returns daqTrgId
unsigned int StDetectorDbTriggerID::getDaqTrgId(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].daqTrgId;
    return value;
};

/// Returns offlineTrgId
unsigned int StDetectorDbTriggerID::getOfflineTrgId(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].offlineTrgId;
    return value;
};

/// Returns trgNameVersion
unsigned int StDetectorDbTriggerID::getTrgNameVersion(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].trgNameVersion;
    return value;
};

/// Returns trgVersion
unsigned int StDetectorDbTriggerID::getTrgVersion(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].trgVersion;
    return value;
};

/// Returns threashVersion
unsigned int StDetectorDbTriggerID::getThreashVersion(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].threashVersion;
    return value;
};

/// Returns psVersion
unsigned int StDetectorDbTriggerID::getPsVersion(unsigned int entry){
    unsigned int value = 0;
    if(mTriggerID && entry < this->getIDNumRows() )
	value = mTriggerID[entry].psVersion;
    return value;
};


/// trigPrescales members
/// Returns Number of Entries in database
unsigned int StDetectorDbTriggerID::getSNumRows(){
    return mSNumRows;
};

/// Returns Run Number
int StDetectorDbTriggerID::getSRunNumber(){
    int value = 0;
    if(mTrigPrescales)
	value = mTrigPrescales[0].runNumber;
    return value;
};

/// Returns idxTrigger
int StDetectorDbTriggerID::getIdxTrigger(unsigned int entry){
    int value = 0;
    if(mTrigPrescales && entry < this->getSNumRows() )
	value = mTrigPrescales[entry].idxTrigger;
    return value;
};

/// Returns idxLevel
int StDetectorDbTriggerID::getIdxLevel(unsigned int entry){
    int value = 0;
    if(mTrigPrescales && entry < this->getSNumRows() )
	value = mTrigPrescales[entry].idxLevel;
    return value;
};

/// Returns id (algorithms)
int StDetectorDbTriggerID::getId(unsigned int entry){
    int value = 0;
    if(mTrigPrescales && entry < this->getSNumRows() )
	value = mTrigPrescales[entry].id;
    return value;
};


/// Returns ps 
float StDetectorDbTriggerID::getPs(unsigned int entry){
    float value = 0;
    if(mTrigPrescales && entry < this->getSNumRows() )
	value = mTrigPrescales[entry].ps;
    return value;
};

/// L0TriggerInfo members
/// Returns Number of Entries in database
unsigned int StDetectorDbTriggerID::getL0NumRows(){
    return mL0NumRows;
};

/// Returns L0 Run Number
int StDetectorDbTriggerID::getL0RunNumber(){
    int value = 0;
    if(mL0TriggerInfo)
	value = mL0TriggerInfo[0].runNumber;
    return value;
};

/// Returns daqTriggerId 
int StDetectorDbTriggerID::getL0DaqTrgId(unsigned int entry){
    int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].daqTriggerId;
    return value;
};

/// Returns offlineTriggerId 
int StDetectorDbTriggerID::getL0OfflineTrgId(unsigned int entry){
    int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].offlineTriggerId;
    return value;
};

/// Returns psL0 
int StDetectorDbTriggerID::getPsL0(unsigned int entry){
    int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].psL0;
    return value;
};

/// Returns name 
char* StDetectorDbTriggerID::getName(unsigned int entry){
    char* value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].name;
    return value;
};

/// Returns detectorLiveOnBits 
unsigned int StDetectorDbTriggerID::getDetectorLiveOnBits(unsigned int entry){
    unsigned int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].detectorLiveOnBits;
    return value;
};

/// Returns detectorLiveOffBits 
unsigned int StDetectorDbTriggerID::getDetectorLiveOffBits(unsigned int entry){
    unsigned int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].detectorLiveOffBits;
    return value;
};

/// Returns detectorRequest 
unsigned int StDetectorDbTriggerID::getDetectorRequest(unsigned int entry){
    unsigned int value = 0;
    if(mL0TriggerInfo && entry < this->getL0NumRows() )
	value = mL0TriggerInfo[entry].detectorRequest;
    return value;
};

unsigned int StDetectorDbTriggerID::getDefaultTriggerLevel(){
    unsigned int value = 999;
    if(mDefaultTriggerLevel)
	value = mDefaultTriggerLevel->level;
    return value;
};

/// outputs to ostream the entire class
ostream& operator<<(ostream& os, StDetectorDbTriggerID& v){
    os << endl << "Run shown in triggerID: " << v.getIDRunNumber() << endl;
    
    for(unsigned int i = 0; i < v.getIDNumRows(); i++){
	os << "idx_trg: " << v.getIdxTrg(i) 
	   << "  daqTrg: " << v.getDaqTrgId(i)  
	   << "  offlineTrgId: " << v.getOfflineTrgId(i)  
	   << "  trgNameVersion: " << v.getTrgNameVersion(i) << endl
	   << "  trgVersion: " << v.getTrgVersion(i)  
	   << "  threashVersion: " << v.getThreashVersion(i)  
	   << "  psVersion: " << v.getPsVersion(i) << endl;
    }
    os << endl << "Run shown in trigPreScales: " << v.getSRunNumber() << endl;
    
    for(unsigned int i = 0; i < v.getSNumRows(); i++){
	os << "idxTrigger: " << v.getIdxTrigger(i) 
	   << "  idxLevel: " << v.getIdxLevel(i)  
	   << "  id (algorithm) : " << v.getId(i)  
	   << "  prescale : " << v.getPs(i) << endl; 
    }
    
    os << endl<<  "Run shown in L0TriggerInfo: " << v.getL0RunNumber() << endl;

    for(unsigned int i = 0; i < v.getL0NumRows(); i++){
	os << "daqTriggerId: " << v.getL0DaqTrgId(i) 
	   << " OfflineTriggerId: " << v.getL0OfflineTrgId(i)   
	   << " psL0: " << v.getPsL0(i)  
	   << " name : " << v.getName(i) << endl 
	   << "detectorLiveOnBits: " << v.getDetectorLiveOnBits(i)  
	   << " detectorLiveOffBits: " << v.getDetectorLiveOffBits(i) 
	   << " detectorRequest: " << v.getDetectorRequest(i)  << endl; 
    }

    os << "Default Trigger Level: " << v.getDefaultTriggerLevel() << endl;
    
    return os;
};
