#ifndef EEmcEventHeader_h
#define EEmcEventHeader_h
/*********************************************************************
 * $Id: EEmcEventHeader.h,v 1.7 2003/12/01 20:19:28 zolnie Exp $
 *********************************************************************
 * $Log: EEmcEventHeader.h,v $
 * Revision 1.7  2003/12/01 20:19:28  zolnie
 * fixed a huge memory leak in EEfeeRawEvent
 *
 * Revision 1.5  2003/09/11 19:41:01  zolnie
 * updates for gcc3.2
 *
 * Revision 1.4  2003/06/16 16:03:54  zolnie
 * updated root version number
 *
 * Revision 1.3  2003/06/02 18:55:00  zolnie
 * added run number to the header
 *
 * Revision 1.2  2003/05/27 19:11:44  zolnie
 * added dE/dx info
 *
 * Revision 1.1  2003/05/20 19:22:58  zolnie
 * new additions for ..... :)
 *
 *********************************************************************/
#include <cstdio>
#include <ctime>
#include "TObject.h"


class EEmcEventHeader : public TObject {
 private:
  unsigned mEventNumber;       //
  unsigned mToken;             //
  time_t   mTimeStamp;         //(unix time, GMT) 
  time_t   mProcessingTime;    // auxiliary 
  unsigned mStatus;            // event status 
  int      mCommentLen;        //
  char    *mComment;           //[mCommentLen];
  unsigned mRunNumber;         //
  
 public:
  EEmcEventHeader();
  virtual ~EEmcEventHeader();
  void         print(FILE *f = stdout) const;
  void         clear();

  void         setEventNumber   ( unsigned en) { mEventNumber    = en; }
  void         setRunNumber     ( unsigned rn) { mRunNumber      = rn; }
  void         setToken         ( unsigned et) { mToken          = et; }
  void         setTimeStamp     ( time_t    t) { mTimeStamp      = t;  }
  void         setProcessingTime( time_t    t) { mProcessingTime = t;  }
  void         setStatus        ( unsigned st) { mStatus         = st; }
  void         setComment       ( const char *str); 


  unsigned     getEventNumber()    const { return mEventNumber;    }
  unsigned     getRunNumber  ()    const { return mRunNumber;      }
  unsigned     getToken()          const { return mToken;          }
  time_t       getTimeStamp()      const { return mTimeStamp;      }
  time_t       getProcessingTime() const { return mProcessingTime; }
  unsigned     getStatus        () const { return mStatus;         }
  const char * getComment ()       const { return mComment;        }
  
  ClassDef(EEmcEventHeader,4) 

};
#endif


