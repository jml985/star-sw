/***************************************************************************
 *
 * $Id: StSvtHit.hh,v 1.5 1999/03/04 18:17:21 ullrich Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StSvtHit.hh,v $
 * Revision 1.5  1999/03/04 18:17:21  ullrich
 * Namespace std not used if ST_NO_NAMESPACES defined
 *
 * Revision 1.5  1999/03/04 18:17:21  ullrich
 * Namespace std not used if ST_NO_NAMESPACES defined
 *
 * Revision 1.4  1999/03/04 15:57:02  wenaus
 * add std namespace for Sun CC5 compatibility
 *
 * Revision 1.3  1999/01/26 16:33:17  wenaus
 * StXXXHit table constructors
 *
 * Revision 1.2  1999/01/15 22:53:55  wenaus
 * version with constructors for table-based loading
 *
 **************************************************************************/
#ifndef StSvtHit_hh
#define StSvtHit_hh

#include <vector>
#include "StEvent/StHit.hh"
#include "StEvent/StGlobalTrack.hh"
#include "StEvent/StTrackCollection.hh"
#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif

class StSvtHit : public StHit {
public:
    StSvtHit(const StThreeVector<float>&,
	     const StThreeVector<float>&,
	     float, unsigned char = 0);
    StSvtHit(dst_point_st* pt) : StHit(pt) {};
    
    StVecPtrGlobalTrack relatedTracks(const StTrackCollection&);
};

#endif
