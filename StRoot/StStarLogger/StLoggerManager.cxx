//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StLoggerManager                                                      //
//                                                                      //
// This class manages the messages in STAR software. It is a singleton. //
// It inherits from StMessMgr, which provides the external interface.   //
// Messages are stored in a vector, and come in several types           //
// (i.e. info, error, debug ). The types "I" (info), "W" (warning),     //
// "E" (error), "D" (debug), and "Q" (QAInfo) are predefined. Message   //
// finding and summary tools are also available.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
#ifdef __ROOT__
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TError.h"
#include "TEnv.h"
#include "TVirtualMutex.h"
#endif

#ifndef  _NO_IMPLEMENTATION_
#define  _NO_IMPLEMENTATION_   {       \
   const char *text = __FUNCTION__;    \
   NDC::push(_T("NO IMPLEMENTATION")); \
   PrintLogger(text,"D","");           \
   NDC::pop();                         \
}
#endif 

#include "StLoggerManager.h"

// #include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/ndc.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/layout.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::varia;
// using namespace log4cxx::xml;

class  StMessage  {
  public: StMessage(){}
};

const char *StLoggerManager::fgLevels = "FEWIDQ";
#ifdef __ROOT__
//    ROOT Error handling subrotuine
//______________________________________________________________________________
static void Log4cxx4RootErrorHandler(Int_t level, Bool_t abort, const char *location, const char *msg)
{
   // This is derived from  the ROOT "default error handler function" TError.cxx
   // The default error handler function. It prints the message on stderr and
   // if abort is set it aborts the application.

   
 //  The level is defined by the current logger level
   NDC::push("ROOT");
         
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,00,0)
if (gErrorIgnoreLevel == kUnset) {
      
       R__LOCKGUARD2(gErrorMutex);
#else 
       {       
#endif       

      gErrorIgnoreLevel = 0;
      if (gEnv) {
         TString level = gEnv->GetValue("Root.ErrorIgnoreLevel", "Info");
         if (!level.CompareTo("Info",TString::kIgnoreCase))
            gErrorIgnoreLevel = kInfo;
         else if (!level.CompareTo("Warning",TString::kIgnoreCase))
            gErrorIgnoreLevel = kWarning;
         else if (!level.CompareTo("Error",TString::kIgnoreCase))
            gErrorIgnoreLevel = kError;
         else if (!level.CompareTo("Break",TString::kIgnoreCase))
            gErrorIgnoreLevel = kBreak;
         else if (!level.CompareTo("SysError",TString::kIgnoreCase))
            gErrorIgnoreLevel = kSysError;
         else if (!level.CompareTo("Fatal",TString::kIgnoreCase))
            gErrorIgnoreLevel = kFatal;
      }
   }

   if (level < gErrorIgnoreLevel)
      return;
   

   if (level >= kFatal) {
        LOG_FATAL << location <<msg << endm;
     } else if (level >= kSysError) {
        LOG_FATAL << location << " : " << msg << endm;  
     } else if (level >= kBreak) {
        LOG_FATAL << location << " : " << msg << endm;  
     } else if (level >= kError) { 
        LOG_ERROR << location << " : " << msg << endm;
//   else if (level >= kWarning)
//        LOG_WARN << location << " : " << msg << endm;
     } else if (level >= kInfo) {
        LOG_INFO << location << " : " << msg << endm;
     }

   if (abort) {
      if (gSystem) {
         gSystem->StackTrace();
         gSystem->Abort();
      } else
         ::abort();
   }
   NDC::pop();
}
#endif
//________________________________________
std::ostream& StLoggerManager::OperatorShift(std::ostream& os, StMessage* stm) {
// std::ostream& operator<<(std::ostream& os, StMessage* stm) {
  if (((&os) == (std::ostream*) StMessMgr::CurrentMessager()) && (stm == endm)) {
    // There was a StMessage terminator
    *this << ends;
     StMessMgr::CurrentMessager()->Print();
     seekp(0);*this << ends;seekp(0);
  } else {
    // fprintf(stderr,"StLoggerManager::OperatorShift os  %p StMessMgr = %Lp, stm = %Lp endm = %Lp\n",
    //       &os, (std::ostream*) StMessMgr::Instance(), stm, endm);
     assert(0);
    // if (stm) os << stm->GetMessage();  // Output this message to the ostream
  }
  return os;
}

//static const char defaultMessType = 'I';
//static char emptyString[] = "";
//static char oOpt[] = "O";
//static char otsOpt[] = "OTS";
//static char eOpt[] = "E";
//static char nullMess[] = "Null message!!!";

StMessMgr* StLoggerManager::mInstance = 0;
//
// C++ routines:
//_____________________________________________________________________________
#ifdef __ROOT__
ClassImp(StLoggerManager)
#endif
//_____________________________________________________________________________
StLoggerManager::StLoggerManager(const char *loggerName) 
                : StMessMgr(), fAllowRepeat(-1),fLastRepeatCounter(0),fStarOptionFilter(0)
{
//
// Constructor - only called once when the library is loaded to
// instantiate the global message manager.
//
// Format 
//   %c  - logger name %c{2} = b.c  ( a.b.c. )
//   %d  - date        %d{%H:%M:%S} or %d{%d %b %Y %H:%M:%S}. 
//                      a - short week date name, A - full week date name
//                      b - short month name,     B - full month name  m - month number (1-12)
//                      d - day of the month
//                      H - hours (0-23)          I - hours ( 1 - 12)
//   %F  - the file name where the logging request was issued
//   %l  - location information of the caller which generated the logging event
//   %L  - the line number from where the logging request was issued
//   %m  - the application supplied message associated with the logging event
//   %n  - the platform dependent line separator character or characters
//   %p  - the level(priority) of the logging event
//   %r  - the number of milliseconds elapsed since the start 
//   %x  - the NDC (nested diagnostic context) 
//
//   This is essentially the TTCC layout.
//   %-6r [%15.15t] %-5p %30.30c %x - %m

  
  unsigned int i;
  for (i=0;i<strlen(fgLevels);i++){
     fSourceFileNames.push_back("");
     fLineNumbers[i] = -1;
  }
  fLogger   = Logger::getLogger(_T(loggerName));
  fDefaultLevel = fLogger->getLevel();
  fCurType     = new char[ 2];  fCurType[0] = 0; fCurType[1] = 0;
  fCurOpt      = new char[32];  fCurOpt [0] = 0; 
  SwitchOff("D");
  MemoryOn();
}

//_____________________________________________________________________________
StLoggerManager::~StLoggerManager() {
//
// Destructor - must delete the message lists
//
  if (StLoggerManager::Instance() == this) gMessMgr = 0;
  delete [] fCurType; fCurType = 0;
  delete [] fCurOpt;  fCurOpt = 0;
}
log4cxx::LoggerPtr StLoggerManager::fgQALogger;      //!  Logger to server QA stream
//_____________________________________________________________________________
StMessMgr* StLoggerManager::Instantiate() 
{return StLoggerManager::StarLoggerInit(); }

//_____________________________________________________________________________
StMessMgr* StLoggerManager::Instantiate(const char *loggerName)
{return StLoggerManager::StarLoggerInit(loggerName); }

//_____________________________________________________________________________
StMessMgr* StLoggerManager::StarLoggerInit(const char *loggerName) 
{
   return  new StLoggerManager(loggerName);
}
//_____________________________________________________________________________
StMessMgr* StLoggerManager::StarLoggerInit() {
//
//   Allows anyone to get a pointer to the single message manager:
//   StLoggerManager::Instance()->(member function)
//
  if (!mInstance) {
    // BasicConfigurator::configure();
    const char *proEnv = 0;
#ifndef __ROOT__
    String propertyFile = "log4j.xml";
#else    
    TString fullPropertyFileName = gEnv->GetValue("Logger.Configuration","log4j.xml");
    gSystem->ExpandPathName(fullPropertyFileName);
    String propertyFile = (const char*)fullPropertyFileName;
    proEnv = gSystem->Getenv("STAR_LOGGER_PROPERTY");
#endif    
    StarOptionFilterPtr filter;
    if (proEnv && proEnv[0] ) propertyFile=proEnv;
    if (!gSystem->AccessPathName(propertyFile.c_str())) {
       xml::DOMConfigurator::configure(propertyFile);
//             PropertyConfigurator::configure(propertyFile);
    } else {       
     	// BasicConfigurator::configure();
       LoggerPtr root = Logger::getRootLogger();
       // Add the STAR default appender
       ConsoleAppenderPtr appender = new ConsoleAppender(
		      new PatternLayout("%-3c{2}:%-5p - %m%n"));
//		      new PatternLayout(PatternLayout::TTCC_CONVERSION_PATTERN)));
       appender->setName(_T("defaultAppender"));       
       filter = new StarOptionFilter();
       appender->addFilter(filter);
       root->addAppender(appender);
       //Set the default threashold to be 
       root->setLevel(Level::INFO);
    }
    Logger::getRootLogger();
    fgQALogger = Logger::getLogger("QA");
    //Almost all QA messages are on the info level
    NDC::push(_T(":"));

    mInstance = StarLoggerInit("BFC");
   ((StLoggerManager *)mInstance)->SetStarOptionFilter(filter);
    // if (gMessMgr) delete gMessMgr; gMessMgr = 0;
    gMessMgr  = mInstance;
    // Set the ROOT ErrorHanlding via logger as well
    // See: $ROOTSYS/include/TError.h
    SetErrorHandler(Log4cxx4RootErrorHandler);
  }
  return mInstance;
}
//______________________________________________________________________________
bool  StLoggerManager::isDebugEnabled()  const{ return fLogger->isDebugEnabled(); }
//______________________________________________________________________________
bool  StLoggerManager::isWarnEnabled()   const{ return fLogger->isWarnEnabled(); }
//______________________________________________________________________________
bool  StLoggerManager::isErrorEnabled()  const{ return fLogger->isErrorEnabled(); }
//______________________________________________________________________________
bool  StLoggerManager::isInfoEnabled()   const{ return fLogger->isInfoEnabled(); }
//______________________________________________________________________________
bool  StLoggerManager::isFatalEnabled()  const{ return fLogger->isFatalEnabled(); }
//______________________________________________________________________________
bool  StLoggerManager::isEnabledFor()    const{ return true; /*fLogger->isEnabledFor();*/ }
//______________________________________________________________________________
bool  StLoggerManager::isQAInfoEnabled() const{ return fgQALogger? fgQALogger->isInfoEnabled():false; }
//_____________________________________________________________________________
StMessMgr& StLoggerManager::Message(const char* mess, const char* type,
  const char* opt,const char *sourceFileName,int lineNumber) {
//
// Message declarator - creates a new message if mess is not empty,
// otherwise, prepares for << input.
//
//
// Instantiate an StMessage and place on the lists of known messages
// type = 
//   'E' - error
//   'W' - warning
//   'I' - info
//   'D' - debug
//   'Q' - QA 
//------------------------------
//
// opt = 
//   'O' - stdout
//   'E' - stderr
//   'S' - skip "St"
//   'P' - plain output no decoration
//   'T' - print timestampt  
//------------------------------
  unsigned char typeChar = 'I';
  if (type && type[0]) typeChar = type[0];
  if (!opt) opt = "";
  size_t messSize = (mess && mess[0]) ? strlen(mess) : 0;
  *fCurType = typeChar;
  strcpy(fCurOpt,opt);
  if (tellp() > 0 ) *this << endm;  // print out the previous line if any
  
  if (sourceFileName && sourceFileName[0] ) fSourceFileNames[LevelIndex(*fCurType)] = sourceFileName;
  else fSourceFileNames[LevelIndex(*fCurType)].clear();
  fLineNumbers[LevelIndex(*fCurType)]     = lineNumber;
 
  if (messSize > 0) *this << mess << endm; // print out the previous this message if present
  return *((StMessMgr*) this);
}
//_____________________________________________________________________________
void StLoggerManager::BuildMessage(const char* mess, const char* type,
  const char* opt,const char *sourceFileName, int lineNumber) {
//
// Instantiate an StMessage and place on the lists of known messages
// type = 
//   'F' - fatal
//   'E' - error
//   'W' - warning
//   'I' - info
//   'D' - debug
//   'Q' - QA 
//------------------------------
//
// opt = 
//   'O' - stdout
//   'E' - stderr
//   'S' - skip "St"
//   'P' - plain output no decoartion
//   'T' - print timestampt  
//------------------------------

     if (!opt) opt = "";
     PrintLogger(mess,type,opt,sourceFileName,lineNumber);
}
//_____________________________________________________________________________
void StLoggerManager::PrintLogger(const char* mess, const char* type,
    const char* opt,const char *sourceFileName, int lineNumber) 
{
   if (!opt) opt = "";
   unsigned char typeChar = 'I';
   if (type && type[0]) typeChar = type[0];
   if (!(sourceFileName || fSourceFileNames[LevelIndex(typeChar)].empty() ) ) 
   {
      sourceFileName = fSourceFileNames[LevelIndex(typeChar)].c_str();
   }

   if ( lineNumber == -1) 
      lineNumber = fLineNumbers[LevelIndex(typeChar)];
   bool canPrint = true;
   if (fAllowRepeat >= 0 ) {
        // compare the current message with the buffered
      if ( mess == fLastMessage ) {
         fLastRepeatCounter++;
         canPrint = (fAllowRepeat >= fLastRepeatCounter);
      } else {
        fLastRepeatCounter= 0;
        fLastMessage = mess;
      }
   }
   if (canPrint) {
      if ( (mess == 0) || (mess[0] == 0)) mess = "."; // logger doesn't like the empty messages 
      // fprintf(stderr, " **** LOGGER **** %c %s\n", typeChar, mess);
      switch (typeChar)  {
        case 'F': fLogger->fatal  (_T(mess),sourceFileName,lineNumber); break;
        case 'E': fLogger->error  (_T(mess),sourceFileName,lineNumber); break;
        case 'W': fLogger->warn   (_T(mess),sourceFileName,lineNumber); break;
        case 'I': fLogger->info   (_T(mess),sourceFileName,lineNumber); break;
        case 'D': fLogger->debug  (_T(mess),sourceFileName,lineNumber); break;
        case 'Q': fgQALogger->info(_T(mess),sourceFileName,lineNumber); break;
        default: fLogger->info(_T(mess),sourceFileName,lineNumber);     break;
      };
   }
//   seekp(0);
   if (lineNumber != -1) { // restore the context
         NDC::pop();
   }
}
//_____________________________________________________________________________
void StLoggerManager::Print() {
//
// Empty the buffer into the current message and print it.
// If not currenty building a message, print the last one created.
//
  string message = ostringstream::str();
  BuildMessage(message.c_str(),fCurType,fCurOpt);
}
//_____________________________________________________________________________
int StLoggerManager::PrintList(messVec* list) {
//
// Print a list of messages from a messVec list.
//
 _NO_IMPLEMENTATION_;
 return -1;
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindMessage(const char* s1, const char* s2,
               const char* s3, const char* s4, messVec* list) {
 _NO_IMPLEMENTATION_;
 return 0;
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindMessageList(const char* s1, const char* s2,
               const char* s3, const char* s4, messVec* list) {
 _NO_IMPLEMENTATION_;
 return 0;
}
//_____________________________________________________________________________
int StLoggerManager::RemoveMessage(StMessage* mess) {
_NO_IMPLEMENTATION_;
 return 0;
}
//_____________________________________________________________________________
void StLoggerManager::Summary(size_t nTerms) {
//
// Output a summary of the messages printed so far.
//   nTerms - number of tokens (text separated by spaces) to use in
//            comparisons between messages (default is 1). Messages are
//            compared only if they are of the same message type, and
//            only the message string (not the message type) is included
//            in the number of tokens compared. For example, Summary(0)
//            would give the number of each type of message with no
//            differentiation based on the message string.
//
_NO_IMPLEMENTATION_;
}
//_____________________________________________________________________________
void StLoggerManager::MemorySummary() {
//
// Output a summary of the memory usage of the message manager so far.
// Loops over all stored messages to calculate message sizes and overhead.
//
  _NO_IMPLEMENTATION_;
}
//_____________________________________________________________________________
int StLoggerManager::AddType(const char* type, const char* text) {
//
// Add an additional message type. ListTypes() will return a list of all
// currently defined message types. AddType returns a integer which is zero
// if type has already been defined, otherwise returns the number of types
// defined (including the new one).
//   type - a single character string which will represent the message type.
//   text - a short charact string which will be printed with all messages
//          of this type.
//
 _NO_IMPLEMENTATION_;
 return 0;
}
//_____________________________________________________________________________
void StLoggerManager::PrintInfo() {
   fLogger->info("**************************************************************\n");
   fLogger->info("* $Id: StLoggerManager.cxx,v 1.28 2007/08/08 20:50:22 fine Exp $\n");
   //  printf("* %s    *\n",m_VersionCVS);
   fLogger->info("**************************************************************\n");
}

//_____________________________________________________________________________

//_____________________________________________________________________________
//
// Info Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::Info(const char* mess, const char* opt,const char *sourceFileName,int lineNumber)
{ return Message(mess, "I", opt,sourceFileName,lineNumber);}
//_____________________________________________________________________________
int StLoggerManager::PrintInfos() 
{
   _NO_IMPLEMENTATION_;
   return 0;
   // return PrintList(messCollection[1]); 
}
//_____________________________________________________________________________
const messVec* StLoggerManager::GetInfos() 
{   
   _NO_IMPLEMENTATION_;
   return 0;
   // return (messCollection[1]);
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindInfo(const char* s1, const char* s2,
                                     const char* s3, const char* s4)
{
   _NO_IMPLEMENTATION_;
   return 0;
   // return FindMessage(s1,s2,s3,s4,messCollection[1]);
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindInfoList(const char* s1, const char* s2,
                                       const char* s3, const char* s4)
{   
   _NO_IMPLEMENTATION_;
   return 0;
   // return FindMessageList(s1,s2,s3,s4,messCollection[1]);
}

//_____________________________________________________________________________
//
// Warning Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::Warning(const char* mess, const char* opt,const char *sourceFileName,int lineNumber)
{ return Message(mess, "W", opt,sourceFileName,lineNumber);}
//_____________________________________________________________________________
int StLoggerManager::PrintWarnings() 
{  
   _NO_IMPLEMENTATION_;
   return 0;
   // return PrintList(messCollection[2]); 
}
//_____________________________________________________________________________
const messVec* StLoggerManager::GetWarnings() 
{
   _NO_IMPLEMENTATION_;
   return 0;
   //   return (messCollection[2]);
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindWarning(const char* s1, const char* s2,
                                        const char* s3, const char* s4)
{
   _NO_IMPLEMENTATION_;
   return 0;
    // return FindMessage(s1,s2,s3,s4,messCollection[2]);
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindWarningList(const char* s1, const char* s2,
                                          const char* s3, const char* s4)
{
   _NO_IMPLEMENTATION_;
   return 0;
    // return FindMessageList(s1,s2,s3,s4,messCollection[2]);
}

//_____________________________________________________________________________
//
// Error Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::Error(const char* mess, const char* opt,const char *sourceFileName,int lineNumber)
{ return Message(mess, "E", opt,sourceFileName,lineNumber);}
//_____________________________________________________________________________
int StLoggerManager::PrintErrors() 
{
   _NO_IMPLEMENTATION_;
   return 0;
   //return PrintList(messCollection[3]); 
}
//_____________________________________________________________________________
const messVec* StLoggerManager::GetErrors() 
{
   _NO_IMPLEMENTATION_;
   return 0;
 // return (messCollection[3]);
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindError(const char* s1, const char* s2,
                                      const char* s3, const char* s4)
{
   _NO_IMPLEMENTATION_;
   return 0;
    // return FindMessage(s1,s2,s3,s4,messCollection[3]);
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindErrorList(const char* s1, const char* s2,
                                        const char* s3, const char* s4)
{  
   _NO_IMPLEMENTATION_;
   return 0;
    // return FindMessageList(s1,s2,s3,s4,messCollection[3]);
}

//_____________________________________________________________________________
//
// Debug Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::Debug(const char* mess, const char* opt,const char *sourceFileName,int lineNumber)
{ return Message(mess, "D", opt,sourceFileName,lineNumber);}
//_____________________________________________________________________________
int StLoggerManager::PrintDebug() 
{
   _NO_IMPLEMENTATION_;
   return 0;
   //   return PrintList(messCollection[4]); 
}
//_____________________________________________________________________________
const messVec* StLoggerManager::GetDebugs() 
{
_NO_IMPLEMENTATION_;   return 0;

//   return (messCollection[4]);
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindDebug(const char* s1, const char* s2,
                                      const char* s3, const char* s4)
{
   _NO_IMPLEMENTATION_; return 0;
   // return FindMessage(s1,s2,s3,s4,messCollection[4]);
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindDebugList(const char* s1, const char* s2,
                                        const char* s3, const char* s4)
{    
_NO_IMPLEMENTATION_;   return 0;
    // return FindMessageList(s1,s2,s3,s4,messCollection[4]);
}

//_____________________________________________________________________________
//
// QAInfo Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::QAInfo(const char* mess, const char* opt,const char *sourceFileName,int lineNumber)
{ return Message(mess, "Q", opt,sourceFileName,lineNumber);}
//_____________________________________________________________________________
void StLoggerManager::IgnoreRepeats()
{ 
    AllowRepeats(0);
}

//_____________________________________________________________________________
void StLoggerManager::AllowRepeats(int nRepeats)
{  
   // fAllowRepeat   > 0 the max number to repeat one and the same message
   //               == 0 the repating is not allowed
   //               == -1 there is no limit;
   fAllowRepeat       = nRepeats;
   fLastRepeatCounter = 0;
   StarOptionFilterPtr& filter = ((StLoggerManager *)mInstance)->GetStarOptionFilter();
   if (filter != 0 ) 
       filter->setRepeatCounterOption(nRepeats);

}


//_____________________________________________________________________________
void StLoggerManager::AllowRepeats()
{ 
   AllowRepeats(-1);
}


//_____________________________________________________________________________
int StLoggerManager::PrintQAInfo() 
{ 
_NO_IMPLEMENTATION_;   return 0;
    // return PrintList(messCollection[5]); 
}
//_____________________________________________________________________________
const messVec* StLoggerManager::GetQAInfos() 
{
_NO_IMPLEMENTATION_;   return 0;
  // return (messCollection[5]);
}
//_____________________________________________________________________________
StMessage* StLoggerManager::FindQAInfo(const char* s1, const char* s2,
                                       const char* s3, const char* s4)
{
_NO_IMPLEMENTATION_;    return 0;
    // return FindMessage(s1,s2,s3,s4,messCollection[5]);
}
//_____________________________________________________________________________
messVec* StLoggerManager::FindQAInfoList(const char* s1, const char* s2,
                                         const char* s3, const char* s4)
{
_NO_IMPLEMENTATION_;   return 0;
   // return FindMessageList(s1,s2,s3,s4,messCollection[5]);
}

//_____________________________________________________________________________
StMessMgr& StLoggerManager::Fatal(const char* mess, const char* opt,const char *sourceFileName, int lineNumber)
{ return Message(mess,"F",opt,sourceFileName,lineNumber);}

// "As is" Messages:
//_____________________________________________________________________________
StMessMgr& StLoggerManager::out(const char* mess)
{return Message(mess,"I","OP-");}
//_____________________________________________________________________________
StMessMgr& StLoggerManager::err(const char* mess)
{return Message(mess,"E","EP-");}



// Dummy methods
//_____________________________________________________________________________
int StLoggerManager::PrintAll() 
{  
_NO_IMPLEMENTATION_;   return 0;
  //  return PrintList(&messList);        
}

//_____________________________________________________________________________
const messVec* StLoggerManager::GetAll() 
{
_NO_IMPLEMENTATION_;   return 0;
 
//   return (&messList);                 
}

//_____________________________________________________________________________
int StLoggerManager::RemoveMessage(const char* s1, const char* s2,
         const char* s3, const char* s4)
{  return RemoveMessage(FindMessage(s1,s2,s3,s4));             }

//_____________________________________________________________________________
void StLoggerManager::SetLimit(const char* str, int n)
{  
     if (str && str[0] && str[1]) {
        LoggerPtr root = Logger::getRootLogger();
         AppenderPtr defaultAppender = root->getAppender(_T("defaultAppender"));
        if (defaultAppender != 0){
           StarOptionFilterPtr filter = new StarOptionFilter();
           filter->setRepeatCounterOption(n);
           filter->setOption(_T("StringToCount"),str);
//           cout << endl<< "--  Appedner " << defaultAppender->getName() << " -* -* -* -* -* -* -* -* " 
//                 << str << "n = " << n << " -* -* -* -* -* -* -* -* " << endl << endl;               
           defaultAppender->addFilter(filter);
        }
    }
  
//   messCounter->SetLimit(str,n);                               
}
//_____________________________________________________________________________
int StLoggerManager::GetLimit(const char* str)
{  
_NO_IMPLEMENTATION_;   return -1;
   // return messCounter->GetLimit(str);                          
}
//_____________________________________________________________________________
void StLoggerManager::ListLimits() 
{  
_NO_IMPLEMENTATION_;
// messCounter->ListLimits();                                  
}
//_____________________________________________________________________________
void StLoggerManager::Close()
{
 // Close the messenger streams
 // Close the QA-related appenders to flush its buffers
   if (fgQALogger) fgQALogger->closeNestedAppenders();
}
//_____________________________________________________________________________
void StLoggerManager::RemoveLimit(const char* str) 
{  SetLimit(str,-1);                                           }
//_____________________________________________________________________________
void StLoggerManager::SwitchOff(const char* str) 
{ SetLimit(str,0);                                             }
//_____________________________________________________________________________
void StLoggerManager::SwitchOn(const char* str) 
{  RemoveLimit(str);                                           }
//_____________________________________________________________________________
void StLoggerManager::FixOn(const char* str) 
{  SetLimit(str,-5);                                           }
//_____________________________________________________________________________
void StLoggerManager::NoLimits() 
{  
_NO_IMPLEMENTATION_;
// messCounter->NoLimits();                                    
}
//_____________________________________________________________________________
void StLoggerManager::MemoryOn() 
{  
_NO_IMPLEMENTATION_;
//   remember=1;                                                 
}
//_____________________________________________________________________________
void StLoggerManager::MemoryOff() 
{
_NO_IMPLEMENTATION_;
// remember=0;                                                 
}
//_____________________________________________________________________________
int StLoggerManager::ListTypes() 
{ 
_NO_IMPLEMENTATION_;   return 5;
//   return messTypeList->ListTypes();                           
}

//_____________________________________________________________________________
void StLoggerManager::SetLevel(Int_t level) 
{
   // Map STAR level to the logger level and set the logger level
   switch (level) {
      case kFatal:
         fLogger->setLevel(Level::FATAL);
         break;
      case kError:
         fLogger->setLevel(Level::ERROR);
         break;
      case kWarning:
         fLogger->setLevel(Level::WARN);
         break;
      case kInfo:
         fLogger->setLevel(Level::INFO);
         break;
      case kAll:
      case kDebug:
      case kDebug2: 
         fLogger->setLevel(Level::DEBUG);
         break;
      case kDefault:
         // restore the default level (defined the XML configuration if present)
         fLogger->setLevel(fDefaultLevel);
         break;
      default:
         fLogger->setLevel(Level::DEBUG);
         break;            
   };   
}
//_____________________________________________________________________________
Int_t StLoggerManager::GetLevel(Int_t) const 
{
   // Map the current logger level to the STAR one
#if 0   
   const LevelPtr &level = fLogger->getLevel();
        if (level == &Level::DEBUG)  return kDebug; 
   else if (level == &Level::FATAL)  return kFatal;  
   else if (level == &Level::ERROR)  return kError;
   else if (level == &Level::WARN )  return kWarning;
   else if (level == &Level::INFO )  return kInfo;
#endif   
   return kAll;
}
#if 0
//_____________________________________________________________________________
const char *GetName() 
{
  // Returns the name of the current logger
  CurrentMessager()
}
#endif
   
//_____________________________________________________________________________
  // Instantiate the (singleton) class upon loading
  //
// static StMessMgr* temp=StLoggerManager::Instance();
// StMessMgr& gMess = *(StMessMgr *)StLoggerManager::Instance();

//_____________________________________________________________________________
// $Id: StLoggerManager.cxx,v 1.28 2007/08/08 20:50:22 fine Exp $
// $Log: StLoggerManager.cxx,v $
// Revision 1.28  2007/08/08 20:50:22  fine
// Fix bug: some messages submitted via the old interface were lost
//
// Revision 1.27  2007/08/03 21:34:51  fine
// fix StStarLogger for Sl 4.4
//
// Revision 1.26  2007/02/13 22:07:26  perev
// Add the lost part of the ROOT message - location
//
// Revision 1.25  2007/01/30 20:48:57  fine
// Make the deault level for all loggers INFO
//
// Revision 1.24  2007/01/30 19:25:51  fine
// Set the deafult level for QA to b INFO
//
// Revision 1.23  2007/01/25 18:36:38  fine
// Acivate logger level StMaker level run-time adjustment
//
// Revision 1.22  2007/01/23 22:27:14  fine
// Set the dwefault logger level to WARN
//
// Revision 1.21  2006/07/01 01:19:17  fine
// Add new jiob tracking option code
//
// Revision 1.20  2006/06/05 00:21:40  fine
// class the new StMessMgr method to flush the logger buffers
//
// Revision 1.19  2006/05/19 21:42:30  fine
// remove the debug print outs
//
// Revision 1.18  2005/12/20 20:51:15  fine
// fix typo
//
// Revision 1.17  2005/10/10 20:24:17  fine
// Bug fix: add the brackets around the if statements
//
// Revision 1.16  2005/09/14 15:35:25  fine
// Clean up the code
//
// Revision 1.15  2005/08/21 20:20:18  perev
// fix typo in comment
//
// Revision 1.14  2005/08/19 21:01:05  fine
// Manage the ROOT messages with the StarLogger
//
// Revision 1.13  2005/08/05 19:39:14  fine
// Make use of the DOM Configurator instead of PropertyConfigurator
//
// Revision 1.12  2005/03/07 23:17:09  fine
// Fix a bug to allow mnay logger insatnces to print out without any mess
//
// Revision 1.11  2004/11/15 17:25:59  fine
// Make Star filer workign properly
//
// Revision 1.10  2004/11/13 00:28:16  fine
// teach StarOption filter to count the messages
//
// Revision 1.9  2004/11/05 20:24:43  fine
// Remove some redundant lines, clean up
//
// Revision 1.8  2004/11/05 20:24:00  fine
// Replace the obsolete StLoggerManager::Instance with tMessMgr::CurrentMessager()
//
// Revision 1.7  2004/11/03 16:39:32  fine
// add extra method to checl STAR QA info logger
//
// Revision 1.6  2004/11/03 01:33:22  fine
// add the implemantions of the new based methods and clean up
//
// Revision 1.5  2004/09/16 00:13:05  fine
// remove the implict StStarLogger object. We should give log4cxx a chance to complete the intialization
//
// Revision 1.4  2004/05/11 23:48:19  fine
// update the default logger pattern
//
// Revision 1.3  2004/05/11 23:21:58  fine
// add the default logger description
//
// Revision 1.2  2004/05/11 22:42:27  fine
// Add the default configurator
//
// Revision 1.1  2004/05/11 20:58:48  fine
// first implemantation of the abstarct STAR messeger via log4cxx package
//
