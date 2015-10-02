*
* $Id: gfrdt.F,v 1.1.1.2 2009/02/01 17:10:31 fisyak Exp $
*
* $Log: gfrdt.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:31  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:48:36  fisyak
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
* Revision 1.1.1.1  1995/10/24 10:21:25  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.21  by  S.Giani
*-- Author :
      SUBROUTINE G3FRDT(ZZ,ESHL,NSHL,NWORD,NRAD,PRB,ENER)
C.
C.    ******************************************************************
C.    *                                                                *
C     *       Fetch Radiative Decay probability Tables and the modes   *
C.    *       the radiative shells decay                               *
C.    *                                                                *
C.    *    ==>Called by : G3FSHDC                                      *
C.    *       Author    J. Chwastowski                                 *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gccuts.inc"
#include "geant321/gcpmxz.inc"
#include "geant321/gcphrd.inc"
      REAL ONEEV
      PARAMETER (ONEEV = 1.E-9)
      DIMENSION ESHL(*),PRB(*),ENER(*),NRAD(*)
      IZ = ZZ
      NWORD = 0
      DO 30 J = 1,NSHL
         NRAD(J) = 0
         ESHELL = ESHL(J)
         IF(ESHELL.GT.0.0) THEN
            IF(J.EQ.1) THEN
               NR = KSHLS
               NS = 0
            ELSEIF(J.EQ.2) THEN
               NR = L1SHLS
               NS = KSHLS
            ELSEIF(J.EQ.3) THEN
               NR = L2SHLS
               NS = KSHLS+L1SHLS
            ELSEIF(J.EQ.4) THEN
               NR = L3SHLS
               NS = KSHLS+L1SHLS+L2SHLS
            ENDIF
            NWS = NWORD+1
            QS = 0.0
            DO 10 I = 1,NR
               QS = QS+GPHRAT(NS+I,IZ)
               IF(GPHRAT(NS+I,IZ).GT.0.0) THEN
                  NWORD = NWORD+1
                  NRAD(J) = NRAD(J)+1
                  PRB(NWORD) = QS
                  ENER(NWORD) = ESHELL-ESHL(ISHLTR(NS+I))
                  ENER(NWORD) = ENER(NWORD)*ONEEV
                  IF(ENER(NWORD).LE.CUTGAM) ENER(NWORD) = -ENER(NWORD)
               ENDIF
   10       CONTINUE
            IF(QS.GT.0.0) THEN
               QS1 = 1./QS
               DO 20 I = NWS,NWORD
                  PRB(I) = PRB(I)*QS1
   20          CONTINUE
            ENDIF
         ENDIF
   30 CONTINUE
      END