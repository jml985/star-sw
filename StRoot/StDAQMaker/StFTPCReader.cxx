/***************************************************************************
 *
 * $Id: StFTPCReader.cxx,v 1.2 2000/06/12 15:04:02 perev Exp $
 *
 * Author: Holm Huemmler
 ***************************************************************************
 *
 * Description: Offline Wrapper for DAQ reader classes
 *
 ***************************************************************************
 *
 * $Log: StFTPCReader.cxx,v $
 * Revision 1.2  2000/06/12 15:04:02  perev
 * SVT + cleanup
 *
 * Revision 1.1  2000/01/24 14:39:33  perev
 * FTPC (HolmMade) is added
 *
 *
 **************************************************************************/
#include "StDAQReader.h"
#include "StFTPCReader.h"
#include "Stypes.h"
#include "StDaqLib/GENERIC/EventReader.hh"

//	non standard open,close,read
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Stypes.h"
//


typedef EventInfo DAQEventInfo;
//_____________________________________________________________________________
StFTPCReader::StFTPCReader(StDAQReader *daqr)
{
  simu=0;
  fDAQReader = daqr;
  fSector = -1999;
  fFTPCImpReader 	= 0;
  fZeroSuppressedReader = 0;
  fADCRawReader 	= 0;
  fPedestalReader 	= 0;
  fPedestalRMSReader 	= 0;
  fGainReader 		= 0;
  fCPPReader 		= 0;
  fBadChannelReader 	= 0;
  Update();
}
//_____________________________________________________________________________
StFTPCReader::StFTPCReader(unsigned short *fcl_ftpcsqndx, int nSeq,
			   char *fcl_ftpcadc, int nAdc)
{
  simu=1;
  m_ftpcsqndx=fcl_ftpcsqndx;
  m_ftpcadc=fcl_ftpcadc;
  m_numSqndx=nSeq;
  m_numAdc=nAdc;

  mSecReader=0;
  fSector = -1999;
  Update();
}
//_____________________________________________________________________________
void StFTPCReader::Update()
{
  setSector(-1);
}
//_____________________________________________________________________________
StFTPCReader::~StFTPCReader()
{
  if(simu)
    {
      delete mSecReader;
    }
  else
    {
      delete fFTPCImpReader;
    }
}
//_____________________________________________________________________________
void StFTPCReader::setSector(int sector)
{
  if (sector == fSector) return;

  if(simu)
    {
      
      delete mSecReader;
      mSecReader = 0;
      
      if (sector == -1) {
	fSector = -1999;
	return;
      }
      
      fSector = sector;
      
      mSecReader = new StFssSectorReader(fSector, m_ftpcsqndx, m_numSqndx, m_ftpcadc, m_numAdc);
      mSecReader->initialize();
    }
  else
    {
      delete fZeroSuppressedReader;
      delete fADCRawReader ;
      delete fPedestalReader ;
      delete fPedestalRMSReader;
      delete fGainReader;
      delete fCPPReader;
      delete fBadChannelReader;
      
      if (sector == -1) {
	delete fFTPCImpReader;
	fFTPCImpReader = ::getDetectorReader(fDAQReader->getEventReader(),fDAQReader->getFTPCVersion());
	fSector = -1999;
      }

      fZeroSuppressedReader = 0;
      fADCRawReader 	= 0;
      fPedestalReader 	= 0;
      fPedestalRMSReader 	= 0;
      fGainReader 		= 0;
      fCPPReader 		= 0;
      fBadChannelReader 	= 0;
      if (sector == -1) return;
      
      fSector = sector;
      
      fZeroSuppressedReader = fFTPCImpReader->getZeroSuppressedReader(fSector);
      fADCRawReader 	= fFTPCImpReader->getADCRawReader(fSector);
      fPedestalReader 	= fFTPCImpReader->getPedestalReader(fSector);
      fPedestalRMSReader 	= fFTPCImpReader->getPedestalRMSReader(fSector);
      fGainReader 		= fFTPCImpReader->getGainReader(fSector);
      fCPPReader 		= fFTPCImpReader->getCPPReader(fSector);
      fBadChannelReader 	= fFTPCImpReader->getBadChannelReader(fSector);
    }
}
//_____________________________________________________________________________
int StFTPCReader::getPadList(int Sector, int PadRow, unsigned char *&padList)
{
  setSector(Sector); 
  if(simu)
    {
      if (!mSecReader) 
	return -1;
      return mSecReader->getPadList(PadRow, &padList);  
    }
  else
    {
      if (!fZeroSuppressedReader) return -1;
      return fZeroSuppressedReader->getPadList(PadRow, &padList);  
    }
} 
//_____________________________________________________________________________
int StFTPCReader::getSequences(int Sector, int PadRow, int Pad, int *nSeq,
			       TPCSequence *&SeqData) 
{
  setSector(Sector);
  *nSeq = 0; SeqData = 0;
  int iret=0;
  Sequence *seq;

  if(simu)
    {
      if (!mSecReader) 
	return -1;
      iret = mSecReader->getSequences(PadRow,Pad,nSeq,&seq);
    }
  else
    {
      if (!fZeroSuppressedReader) 
	return -1;
      iret = fZeroSuppressedReader->getSequences(PadRow,Pad,nSeq,&seq);
    }
  assert (sizeof(TPCSequence)==sizeof(Sequence));
  SeqData = (TPCSequence*)seq;
  return iret;
}

//_____________________________________________________________________________
int StFTPCReader::getRawADC(int Sector,int PadRow, int Pad, int &nArray,
                        unsigned char *&Array)
{
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      nArray = 0; Array=0;
      if (!fADCRawReader) return -1;
      return fADCRawReader->getSequences(PadRow,Pad,&nArray,&Array);
    }
}  
//_____________________________________________________________________________
int StFTPCReader::getPedestals(int Sector,int PadRow, int Pad, int &nArray,
                           unsigned char *&Array)
{  
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      nArray = 0; Array=0;
      if (!fPedestalReader) return -1;
      return fPedestalReader->getSequences(PadRow,Pad,&nArray,&Array);
    }
}  
//_____________________________________________________________________________
int StFTPCReader::getRMSPedestals(int Sector,int PadRow, int Pad, int &nArray,
                           unsigned char *&Array)
{  
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      nArray = 0; Array=0;
      if (!fPedestalRMSReader) return -1;
      return fPedestalRMSReader->getSequences(PadRow,Pad,&nArray,&Array);
    }  
}
//_____________________________________________________________________________
int StFTPCReader::getGain(int Sector, int PadRow, int Pad, TPCGain *&gain)
{  
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      gain = 0;
      if (!fGainReader) return -1; 
      struct Gain *gainqq;
      int iret = fGainReader->getGain(PadRow,Pad,&gainqq);
      assert(sizeof(TPCGain)==sizeof(struct Gain));
      gain = (TPCGain*)gainqq;
      return iret;
    }
}
//_____________________________________________________________________________
int StFTPCReader::getClusters(int Sector, int PadRow, int Pad, int &nClusters, 
			     TPCCluster *&clusters)
{  
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      nClusters=0; clusters=0;
      if (!fCPPReader) return -1;
      struct ASIC_Cluster *clustersqq;
      int iret = fCPPReader->getClusters(PadRow,Pad,&nClusters,&clustersqq);
      assert(sizeof(TPCCluster)==sizeof(struct ASIC_Cluster));
      clusters = (TPCCluster *)clustersqq;
      return iret;
    }
}  
//_____________________________________________________________________________
int StFTPCReader::IsBad(int Sector, int PadRow, int Pad)
{
  if(simu)
    {
      return -1;
    }
  else
    {
      setSector(Sector);
      if (!fBadChannelReader) return 1;
      return fBadChannelReader->IsBad(PadRow,Pad);
    }
}
  
  
