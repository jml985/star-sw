*
* $Id: goscin.F,v 1.1.1.2 2009/02/01 17:10:33 fisyak Exp $
*
* $Log: goscin.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:33  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:48:49  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:37:56  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.2  2003/11/28 11:23:56  brun
* New version of geant321 with all geant3 routines renamed from G to G3
*
* Revision 1.1.1.1  2002/07/24 15:56:27  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:18:41  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:21  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:38  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.25  by  S.Giani
*-- Author :
      FUNCTION G3OSCIN(EIN1EV,EIN2EV)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *     Input energy in eV, Sandia tables in keV                   *
C.    *                                                                *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcjloc.inc"
#include "geant321/gconsp.inc"
#include "geant321/gcmate.inc"
#include "geant321/gc10ev.inc"
#if !defined(CERNLIB_SINGLE)
      DOUBLE PRECISION ONE,RES,ZERO,EIN1,EIN2,REST,EBEG,EEND
      DOUBLE PRECISION E1IN,E2IN,TWO,THREE,C1,C2,C3,C4
#endif
      PARAMETER (ZERO=0,ONE=1,TWO=2,THREE=3)
C.
C.    ------------------------------------------------------------------
C.
      REST   = ZERO
      EIN1   = EIN1EV*1E-3
      EIN2   = EIN2EV*1E-3
C Use Sandia data
      JPHXS  = LQ(JPHOT-1)
      NZ     = Q(JPHXS+1)
      JWEIGH = JPHXS+1+2*NZ
      DO 30 JZ=1,NZ
         RES    = ZERO
         EBEG   = TENEV
         WEIGHT = Q(JWEIGH+JZ)
         JPHFN  = LQ(JPHXS-JZ)
         IPOINT = JPHFN+1
         IMAX   = Q(IPOINT)
         IPOINT = IPOINT-4
         DO 10 I = 1,IMAX
            IPOINT = IPOINT+5
            EEND   = Q(IPOINT)
            IF(EIN2.GT.EBEG.AND.EIN1.LT.EEND) THEN
               E1IN  = ONE/MAX(EBEG,EIN1)
               E2IN  = ONE/MIN(EEND,EIN2)
               J     = IPOINT+1
               C1    = -Q(J  )
               C2    = -Q(J+1)
               C3    = -Q(J+2)/TWO
               C4    = -Q(J+3)/THREE
               RES   = RES
     +         +C1*LOG(E2IN)+E2IN*(C2+E2IN*(C3+E2IN*C4))
     +         -C1*LOG(E1IN)-E1IN*(C2+E1IN*(C3+E1IN*C4))
            ENDIF
            EBEG = EEND
   10    CONTINUE
   20    REST = REST+WEIGHT*RES
   30 CONTINUE
C RES value is in cm**2/(g keV)
      G3OSCIN = REST*1E-3*A/AVO
C Now in Megabarns/eV
C
      END
