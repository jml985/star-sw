/*
 *  paw_interface.h  --
 *	Declare interface to PAW.
 *
 *  Original: 12-Jan-1996 15:58
 *
 *  Author:   Maarten Ballintijn <Maarten.Ballintijn@cern.ch>
 *
 *  $Id: paw_interface.h,v 1.1 1999/02/16 15:45:10 fisyak Exp $
 *
 *  $Log: paw_interface.h,v $
 *  Revision 1.1  1999/02/16 15:45:10  fisyak
 *  add kuip stuff
 *
 *  Revision 1.11  1996/08/30 10:04:27  lecointe
 *  Restored Gouraud Shading in NT/PLOT
 *
 *  Revision 1.10  1996/08/21 12:55:33  lecointe
 *  Restore the spider plot in ntuple/scan
 *
 *  Revision 1.9  1996/05/24 09:16:06  dinofm
 *  Bug fixed when a FORTRAN selection function operates on a PIAF residing
 *  ntuple. The source file is sent to PIAF and compiled if '.f77' extension
 *  has been used.
 *
 *  Revision 1.8  1996/05/15 13:11:37  maartenb
 *  - Fix the CSELECT command.
 *
 *  Revision 1.7  1996/05/06 13:34:19  dinofm
 *  Code modified to take care of empty histograms detection on slave(s).
 *
 *  Revision 1.6  1996/04/23 18:38:08  maartenb
 *  - Add RCS keywords
 *
 *
 */

#ifndef CERN_PAW_INTERFACE
#define CERN_PAW_INTERFACE

#include	"cfortran.h"


#define FTNPRN(CHMESS) \
	CCALLSFSUB1(FTNPRN,ftnprn,STRING,CHMESS)


#define GETTP(THETA,PHI) \
	CCALLSFSUB2(GETTP,gettp,PFLOAT,PFLOAT,THETA,PHI)


#define HFIND(IDD,CHROUT) \
	CCALLSFSUB2(HFIND,hfind,INT,STRING,IDD,CHROUT)


#define PACSEL \
	CCALLSFSUB0(PACSEL,pacsel)


#define	GETNBINS(NX,NY,NZ) \
	CCALLSFSUB3(GETNBINS,getnbins,PINT,PINT,PINT,NX,NY,NZ)

#define	PAHLOG(LOGX,LOGY,LOGZ) \
	CCALLSFSUB3(PAHLOG,pahlog,PLOGICAL,PLOGICAL,PLOGICAL,LOGX,LOGY,LOGZ)


#define PAPLOT(ID,CHOPT,CHCASE,NUM,ICRANG,ICX1,ICX2,ICY1,ICY2) \
	CCALLSFSUB9(PAPLOT,paplot,INT,STRING,STRING,INT,INT,INT,INT,INT,INT,\
		ID,CHOPT,CHCASE,NUM,ICRANG,ICX1,ICX2,ICY1,ICY2)

#define PASPI(ICHEVT, NVARS, CNAMES, RZONE, IVART, IZONE, CURRENT, LOW, HIGH, AVG,SPIDER_TYPE) \
	CCALLSFSUB11(PASPI,paspi, \
		    INT,INT,STRING,FLOAT,INTV,INT,FLOATV,FLOATV,FLOATV,FLOATV,INT, \
		    ICHEVT, NVARS, CNAMES, RZONE, IVART, IZONE, CURRENT, LOW, HIGH,AVG,SPIDER_TYPE)

#define PADRISO(NX,NY,NZ,X,Y,Z,VALUE,S) \
        CCALLSFSUB8(PADRISO,padriso,INT,INT,INT,FLOATV,FLOATV,FLOATV,FLOATV,FLOATV,\
		   NX,NY,NZ,X,Y,Z,VALUE,S) 


#define PAUTIT(CHTIT)	\
	CCALLSFSUB1(PAUTIT,pautit,STRING,CHTIT)


#define PAWCS \
	CCALLSFSUB0(PAWCS,pawcs)


#define PAWILD(CFIRST,CSECON,ILFIRS,IRESUL) \
	CCALLSFSUB4(PAWILD,pawild,STRING,STRING,INT,PINT,CFIRST,CSECON,ILFIRS,IRESUL)


#define	PAWLOC(NP,XP,YP,NTPRI,IWKID,CHOPT) \
	CCALLSFSUB6(PAWLOC,pawloc,PINT,FLOATV,FLOATV,INT,INT,STRING,NP,XP,YP,NTPRI,IWKID,CHOPT)


PROTOCCALLSFFUN5(INT,PCHEVT,pchevt,STRING,INT,INT,INT,INT)

#define	PCHEVT(NAME,LEN,ID,NEVT,IOP)	\
	CCALLSFFUN5(PCHEVT,pchevt,STRING,INT,INT,INT,INT,NAME,LEN,ID,NEVT,IOP)


#define	PCHNCD(PATH,IERR)	\
	CCALLSFSUB2(PCHNCD,pchncd,STRING,PINT,PATH,IERR)


#define	PCNEXT(IDN,NCHROW,NDIM,NROW,IEND)	\
	CCALLSFSUB5(PCNEXT,pcnext,INT,PINT,PINT,PINT,PINT,\
		IDN,NCHROW,NDIM,NROW,IEND)	\


#define	PFHOUT(IDH,ISTAT)	\
	CCALLSFSUB2(PFHOUT,pfhout,INT,PINT,IDH,ISTAT)


#define	PFKUIP(CHCMD,ISTAT)	\
	CCALLSFSUB2(PFKUIP,pfkuip,STRING,PINT,CHCMD,ISTAT)


#define	PFSOCK(ISLAV)	\
	CCALLSFSUB1(PFSOCK,pfsock,INT,ISLAV)


#define	PFPING(ISLAV,IACT,ISTAT)	\
	CCALLSFSUB3(PFPING,pfping,INT,INT,PINT,ISLAV,IACT,ISTAT)


#define	PFPUSH(ISTAT)	\
	CCALLSFSUB1(PFPUSH,pfpush,PINT,ISTAT)


#define	PFMINMAX(RMIN,RMAX)	\
	CCALLSFSUB2(PFMINMAX,pfminmax,PFLOAT,PFLOAT,RMIN,RMAX)


#define	PFLABELS(CBUF,LENBUF)	\
	CCALLSFSUB2(PFLABELS,pflabels,PSTRING,PINT,CBUF,LENBUF)


#define	PFEMPTY(CBUF)	\
	CCALLSFSUB1(PFEMPTY,pfempty,STRING,CBUF)


#define	PFCSEX(LUNIN,CHFILE,ISTAT)	\
	CCALLSFSUB3(PFCSEX,pfcsex,INT,STRING,PINT,LUNIN,CHFILE,ISTAT)


#endif	/*	CERN_PAW_INTERFACE	*/








