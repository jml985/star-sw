/***************************************************************************
 *
 * $Id: StMcFtpcHit.cc,v 2.1 1999/11/19 19:06:32 calderon Exp $
 * $Log: StMcFtpcHit.cc,v $
 * Revision 2.1  1999/11/19 19:06:32  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.3  1999/09/23 21:25:50  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#include "StThreeVectorF.hh"

#include "StMcFtpcHit.hh"
#include "StMcTrack.hh"
#include "tables/St_g2t_ftp_hit_Table.h" 

static const char rcsid[] = "$Id: StMcFtpcHit.cc,v 2.1 1999/11/19 19:06:32 calderon Exp $";

StMemoryPool StMcFtpcHit::mPool(sizeof(StMcFtpcHit));

StMcFtpcHit::StMcFtpcHit(const StThreeVectorF& p,
			 const float de, const float ds,
			 StMcTrack* parent)  : StMcHit(p, de, ds, parent)
{ /* noop */ }

StMcFtpcHit::StMcFtpcHit(g2t_ftp_hit_st* pt)
{
  mdE = pt->de;
  mdS = pt->ds;
  // Decode position.
  mPosition.setX(pt->x[0]); 
  mPosition.setY(pt->x[1]);
  mPosition.setZ(pt->x[2]);
  mVolumeId = pt->volume_id;
}

StMcFtpcHit::~StMcFtpcHit() {/* noop */ }

unsigned long
StMcFtpcHit::plane() const
{
    //volume_id = 101 to 110 are the first FTPC (0-9 in StEvent)
    //volume_id = 201 to 210 are the second FTPC (10-19 in StEvent)
    return (mVolumeId/100 - 1)*10 + mVolumeId%100 - 1;
}
