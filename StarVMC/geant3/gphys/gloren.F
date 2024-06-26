*
* $Id: gloren.F,v 1.1.1.2 2009/02/01 17:10:31 fisyak Exp $
*
* $Log: gloren.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:31  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:48:38  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:37:54  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.2  2003/11/28 11:23:56  brun
* New version of geant321 with all geant3 routines renamed from G to G3
*
* Revision 1.1.1.1  2002/07/24 15:56:25  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:18:41  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:20  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:26  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.22  by  S.Giani
*-- Author :
      SUBROUTINE G3LOREN(BETA,PA,PB)
C.
C.    ******************************************************************
C.    *                                                                *
C     *       Routine to transform momentum and energy from the        *
C     *       Lorentz frame A to the Lorentz frame B                   *
C     *                                                                *
C     *       PA(1)                                                    *
C     *       PA(2)     Momentum components in frame A                 *
C     *       PA(3)                                                    *
C     *       PA(4)     Energy                                         *
C     *       PB(..)   same quantities in frame B                      *
C     *                                                                *
C     *       BETA(1)    Components of velocity of frame B             *
C     *       BETA(2)        as seen from frame A                      *
C     *       BETA(3)                                                  *
C     *       BETA(4)    1./SQRT(1.-BETA**2)                           *
C.    *                                                                *
C.    *    ==>Called by : G3DECAY,G3DECA3                              *
C.    *       Author    M.Hansroul  *********                          *
C.    *                                                                *
C.    ******************************************************************
C.
      DIMENSION BETA(4),PA(4),PB(4)
C.
C.    ------------------------------------------------------------------
C.
      BETPA  = BETA(1)*PA(1) + BETA(2)*PA(2) + BETA(3)*PA(3)
      BPGAM  = (BETPA * BETA(4)/(BETA(4) + 1.) - PA(4)) * BETA(4)
      PB(1) = PA(1) + BPGAM  * BETA(1)
      PB(2) = PA(2) + BPGAM  * BETA(2)
      PB(3) = PA(3) + BPGAM  * BETA(3)
      PB(4) =(PA(4) - BETPA) * BETA(4)
      END
 
