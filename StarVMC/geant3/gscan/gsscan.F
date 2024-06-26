*
* $Id: gsscan.F,v 1.1.1.2 2009/02/01 17:10:32 fisyak Exp $
*
* $Log: gsscan.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:32  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:48:48  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:37:56  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.2  2003/11/28 11:23:56  brun
* New version of geant321 with all geant3 routines renamed from G to G3
*
* Revision 1.1.1.1  2002/07/24 15:56:26  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:18:41  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:20  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:36  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.31  by  S.Giani
*-- Author :
      SUBROUTINE G3SSCAN(ISL,ID)
*
************************************************************************
*             Enter current point in the LSTEMP/LSCAN structure        *
*                                                                      *
*           Layout of the LSCAN data structure                         *
*           ==================================                         *
*                                                                      *
*                          |                                           *
*                          | LSCAN                                     *
*                          |                                           *
*  -nphi      -iphi    -1  V                                           *
* +-----------+---+--------+----------------------------------------   *
* |           | . |        | (1..5)   nphi,nteta,modtet,nslist,free    *
* |           | . |        | (6..10)  phimin,phimax,tetmin,tetmax      *
* |           | . |        | (11..)   islist                           *
* +-----------+---+--------+----------------------------------------   *
*               |                                                      *
*               | LPHI                                                 *
*               |                                                      *
*               V  nteta                                               *
*               +--------------------------------------------------    *
*               |   IDES  |  List of (W1,W2)  |                        *
*               +--------------------------------------------------    *
*                                                                      *
*                                                                      *
*   where  IDES  [ 1:16] = ISCUR pointer in LPHI to first (W1,W2)      *
*                [17:32] = NW Number of pairs (W1,W2) -1               *
*                                                                      *
*           W1   [ 1:17] = IDET                                        *
*                [18:22] = ISL number in ISLIST-1                      *
*                [23:32] = SABS*FACTL                                  *
*                                                                      *
*           W2   [ 1:17] = RIN*FACTR                                   *
*                [18:32] = SX0*FACTX0                                  *
*                                                                      *
*           SX0          = Sum of rad lengths up to current R boundary *
*                                                                      *
*           SABS         = Sum of abs lengths up to current R boundary *
*                                                                      *
*           RIN          = SQRT(X**2+Y**2+Z**2) at current R boundary  *
*                                                                      *
*           FACTX0       = multiplication factor (default=100)         *
*                                                                      *
*           FACTL        = multiplication factor (default=1000)        *
*                                                                      *
*           FACTR        = multiplication factor (default=100)         *
*                                                                      *
*     Bits are numbered from 1 to 32 and the most significative        *
*     bit is bit number 2 (or 1 for unsigned integers)                 *
*                                                                      *
*                                                                      *
*          ==>Called by : <USER>,GUKINE                                *
*             Author    R.Brun  *********                              *
************************************************************************
*
#include "geant321/gcbank.inc"
#include "geant321/gcscal.inc"
#include "geant321/gcflag.inc"
#include "geant321/gctrak.inc"
#include "geant321/gcscan.inc"
C.
C.    ------------------------------------------------------------------
C.
      ISX0=FACTX0*SX0
      ISAB=FACTL*SABS
      IRIN=FACTR*SLENG
      IF(IRIN.LE.0)GO TO 999
      IDES=IQ(LSTEMP+ITETA)
      IF(IDES.EQ.0)THEN
         IQ(LSTEMP+ITETA)=ISCUR
         NW=0
      ELSE
         NW=IBITS(IDES,16,16)+1
         IDOLD=IBITS(IQ(LSTEMP+ISCUR-2),0,18)
         IF(IDOLD.EQ.ID)GO TO 999
         CALL MVBITS(NW,0,16,IQ(LSTEMP+ITETA),16)
      ENDIF
      IW1=ID
      CALL MVBITS(ISL,0,5,IW1,17)
      CALL MVBITS(ISAB,0,10,IW1,22)
      IW2=IRIN
      CALL MVBITS(ISX0,0,15,IW2,17)
      IF(ISCUR+5.GT.IQ(LSTEMP-1))THEN
         CALL MZPUSH(IXCONS,LSTEMP,0,1000,'I')
      ENDIF
      IQ(LSTEMP+ISCUR)=IW1
      IQ(LSTEMP+ISCUR+1)=IW2
      ISCUR=ISCUR+2
      IF(IDEBUG.NE.0)THEN
         IF(ISWIT(3).GT.ITETA)THEN
            NAME=IQ(JVOLUM+1)
            IF(ISL.NE.0)NAME=ISLIST(ISL)
            IF(NW.EQ.0)PRINT 10000,IPHI,ITETA
            PRINT 10100,NW+1,NAME,ID,IRIN,ISX0,ISAB
10000       FORMAT(' IPHI =',I3,' ITETA =',I3,
     +             ' NW   NAME      ID      IRIN     ISX0     ISAB')
10100       FORMAT(21X,I3,3X,A4,2X,I6,2X,I8,2X,I7,2X,I7)
         ENDIF
      ENDIF
  999 END
