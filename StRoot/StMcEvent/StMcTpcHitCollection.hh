/***************************************************************************
 *
 * $Id: StMcTpcHitCollection.hh,v 1.2 1999/09/23 21:25:53 calderon Exp $
 * $Log: StMcTpcHitCollection.hh,v $
 * Revision 1.2  1999/09/23 21:25:53  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#ifndef StMcTpcHitCollection_hh
#define StMcTpcHitCollection_hh


#include <vector>
#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif
class StMcTpcHit;

#ifdef ST_NO_TEMPLATE_DEF_ARGS
typedef vector<StMcTpcHit*, allocator<StMcTpcHit*> >            StMcTpcHitCollection;
typedef vector<StMcTpcHit*, allocator<StMcTpcHit*> >::iterator  StMcTpcHitIterator;
#else
typedef vector<StMcTpcHit*>            StMcTpcHitCollection;
typedef vector<StMcTpcHit*>::iterator  StMcTpcHitIterator;
#endif

#endif
