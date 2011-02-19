// $Id: QAhlist_EventQA_qa_shift.h,v 2.37 2011/02/19 02:22:18 genevb Exp $
// $Log: QAhlist_EventQA_qa_shift.h,v $
// Revision 2.37  2011/02/19 02:22:18  genevb
// Allow for specification of histogram usage by the required detector sets
//
// Revision 2.36  2011/02/07 20:25:56  genevb
// specify subsystems
//
// Revision 2.35  2010/01/04 19:44:52  genevb
// Remove V0s from QA shift set, revert to large multiplicity ranges
//
// Revision 2.34  2009/03/19 01:08:08  genevb
// Show both xy and rphi TPC hit hists
//
// Revision 2.33  2009/03/09 16:35:30  genevb
// Use small histogram ranges for pp qa_shift
//
// Revision 2.32  2008/02/05 22:51:50  genevb
// Correction on two histogram names for QA shift
//
// Revision 2.31  2007/05/02 20:46:40  genevb
// Additional SVT plots
//
// Revision 2.30  2007/04/12 20:39:48  genevb
// Cleanup (removal) of CalibVtx, Nfitpnt, Chisq1, Rich, histograms
//
// Revision 2.29  2007/04/07 04:40:29  genevb
// Remove fit pnts/tot; retitle log as log10
//
// Revision 2.28  2007/03/13 18:37:53  genevb
// simplified names
//
// Revision 2.27  2006/03/20 03:35:29  genevb
// FTPC: PtrkGood and PtrkPsi hists out, GtrkPadTime in
//
// Revision 2.26  2004/12/13 15:52:36  genevb
// Numerous updates: PMD, primtrk, FPD, QAShift lists
//
// Revision 2.25  2004/10/04 16:40:42  genevb
// FTPC radial histos
//
// Revision 2.24  2004/04/23 23:15:29  genevb
// Added signedDCA (Impact) plots for globals
//
// Revision 2.23  2004/03/17 00:35:14  genevb
// Left Mult off hist names in previous commit
//
// Revision 2.22  2004/03/15 23:32:14  genevb
// Add primary vertex check for event classes to QA Shift set
//
// Revision 2.21  2004/02/12 17:39:15  genevb
// Separate MinBias histos
//
// Revision 2.20  2004/02/12 05:03:03  genevb
// Year 4 AuAu changes. New SVT histos.
//
// Revision 2.19  2003/04/19 00:17:49  genevb
// Updated for dAu/pp running
//
// Revision 2.18  2003/01/02 19:39:47  genevb
// Comment out BBC/FPD histos; See diffs to previous version to restore
//
// Revision 2.17  2002/05/29 13:54:30  genevb
// Some changes to FTPC chisq histos
//
// Revision 2.16  2002/04/23 01:59:55  genevb
// Addition of BBC/FPD histos
//
// Revision 2.15  2002/02/12 18:41:59  genevb
// Additional FTPC histograms
//
// Revision 2.14  2002/01/26 03:04:07  genevb
// Fixed some problems with fcl histos
//
// Revision 2.13  2002/01/21 22:09:24  genevb
// Include some ftpc histograms from StFtpcClusterMaker
//
// Revision 2.12  2001/11/20 21:53:45  lansdell
// added x-y dist of hits, tpc east&west histos
//
// Revision 2.11  2001/08/29 20:45:15  genevb
// Trigger word histos
//
// Revision 2.10  2001/08/27 21:15:15  genevb
// fixed a typo
//
// Revision 2.9  2001/07/31 23:21:42  lansdell
// added last point, hit-helix histos
//
// Revision 2.8  2001/06/27 23:57:50  lansdell
// added geant-reco primvtx position histos to qa_shift list
//
// Revision 2.7  2001/05/29 23:23:05  lansdell
// removed impact param plots for FTPC from qa_shift list
//
// Revision 2.6  2001/05/25 16:31:20  lansdell
// more updates to qa shift histograms
//
// Revision 2.5  2001/05/24 01:48:13  lansdell
// qa_shift histograms updated
//
// Revision 2.4  2001/05/23 00:14:52  lansdell
// more changes for qa_shift histograms
//
// Revision 2.3  2001/05/16 20:57:02  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.2  2001/04/24 22:53:50  lansdell
// Removed redundant radial position of first hit histograms
//
// Revision 2.1  2000/08/25 16:04:09  genevb
// Introduction of files
//
//
///////////////////////////////////////////////////////////////////////
// Names of histograms to be plotted for dir=EventQA, analType=qa_shift
///////////////////////////////////////////////////////////////////////
// Note: Editing this file means that StAnalysisUtilities/StHistUtil
// must be recompiled
// See StHistUtil::DetectorIn() for format of detector requirement

  "QaMultClass",
  "QaTrigWord",
  "QaTrigBits",
  ":ftpc:fcl_chargestepW",
  ":ftpc:fcl_chargestepE",
  ":ftpc:fcl_radialW",
  ":ftpc:fcl_radialE",

  ":tpx;tpc:QaPointTpc",
  ":svt:QaPointSvt",
  ":svt:QaPointXYSvt",
  ":svt:QaPointSvtLaser",
  ":ftpc:QaPointFtpc",
  ":tpx;tpc:QaPointRPTpcE",
  ":tpx;tpc:QaPointRPTpcW",
  ":ftpc:QaPointXYFtpcE",
  ":ftpc:QaPointXYFtpcW",
  ":ftpc:QaPointPadTimeFtpcW",
  ":ftpc:QaPointPadTimeFtpcE",
  ":tpx;tpc:QaPointZhits",
  ":tpx;tpc:QaPointPhiT",
  ":svt:QaPointZhitsS",
  ":svt:QaPointPhiS",
  ":tpx;tpc:QaPointPadrowT",
  ":svt:QaPointBarrelS",
  ":ftpc:QaPointPlaneF",
  ":tpx;tpc:QaGtrkZfTS",
  ":tpx;tpc:QaGtrkPhifTS",
  ":tpx;tpc:QaGtrkZfT",
  ":tpx;tpc:QaGtrkPhifT",
  ":ftpc:QaGtrkXfYfFE",
  ":ftpc:QaGtrkXfYfFW",
  ":tpx;tpc:QaGtrkPadfTEW",
  ":tpx;tpc:QaGtrkRTS",
  ":tpx;tpc:QaGtrkRnmfTTS",
  ":tpx;tpc:QaGtrkPsiTTS",
  ":tpx;tpc:QaGtrkPtTTS",
  ":tpx;tpc:QaGtrkEtaTTS",
  ":ftpc:QaGtrkPsiF",
  ":ftpc:QaGtrkPtF",
  ":ftpc:QaGtrkEtaF",
  ":ftpc:QaGtrkPF",
  ":tpx;tpc:QaGtrkChisq0T",
  ":tpx;tpc:QaGtrkFlag",
  ":tpx;tpc:QaGtrkGood",
  ":tpx;tpc:QaGtrkNPntFitTTS",
  ":ftpc:QaGtrkNPntF",
  ":tpx;tpc:QaGtrkGoodTTS",
  ":ftpc:QaGtrkGoodF",
  ":tpx;tpc:QaGtrkFitPntLTTS",
  ":tpx;tpc:QaGtrkSImpactT",
  ":tpx;tpc:QaGtrkImpactTTS",
  ":tpx;tpc:QaGtrkImpactrTTS",
  ":ftpc:QaGtrkImpactrF",
  ":tpx;tpc:QaGtrkDetId",
  ":tpx;tpc:QaGtrkTanlzf",
  ":tpx;tpc:QaGtrkTanlzfTS",
  ":tpx;tpc:QaPtrkPsiTTS",
  ":tpx;tpc:QaPtrkPtTTS",
  ":tpx;tpc:QaPtrkEtaTTS",
  ":ftpc:QaPtrkPtF",
  ":ftpc:QaPtrkEtaF",
  ":tpx;tpc:QaPtrkMeanPtTTS",
  ":tpx;tpc:QaPtrkMeanEtaTTS",
  ":ftpc:QaPtrkMeanPtF",
  ":ftpc:QaPtrkMeanEtaF",
  ":tpx;tpc:QaPtrkGood",
  ":tpx;tpc:QaPtrkGoodTTS",
  ":tpx;tpc:QaPtrkChisq0TTS",
  ":tpx;tpc:QaPtrkFlag",
  ":tpx;tpc:QaPtrkGlob",
  ":tpx;tpc:QaPtrkFitPntLTTS",
  ":svt:QaPtrkSvtLoc",
  ":tpx;tpc:QaNullPrimVtxMult",
  ":tpx;tpc:QaVtxPrXY",
  ":tpx;tpc:QaVtxPrZ",
  ":ftpc:QaVtxFtpcETpcXY",
  ":ftpc:QaVtxFtpcETpcZ",
  ":ftpc:QaVtxFtpcWTpcXY",
  ":ftpc:QaVtxFtpcWTpcZ",
  ":tpx;tpc:QaGtrkDcaBeamZ1",
  ":tpx;tpc:QaGtrkDcaBeamZ2",
  ":tpx;tpc:QaGtrkZdcaZf",
  ":tpx;tpc:QaGtrkZdcaPsi",
  ":tpx;tpc:QaGtrkZdcaTanl",
  ":tpx;tpc:QaPidGlobtrkDstdedxPVsDedx",
  ":tpx;tpc:QaDedxBBTTS",
  ":tpx;tpc:QaEvsumTotChg",
  ":ftpc:QaEvsumTotChgF",
  ":tpx;tpc:QaGtrkRZf0",
  ":tpx,svt;tpc,svt:QaGtrkRZf0TS",
  ":tpx,svt:QaPtrkRZf0",
  ":tpx,svt;tpc,svt:QaPtrkRZf0TS",
  ":emc:bemcClNum",
  ":emc:bemcClEnergy",
  ":emc:bemcEta",
  ":emc:bemcPhi",
  ":bsmd:bsmdeClNum",
  ":bsmd:bsmdeEta",
  ":bsmd:bsmdpClNum",
  ":bsmd:bsmdpPhi",
  ":emc:EmcCat4_Point_Energy",
  ":emc:EmcCat4_Point_Eta",
  ":emc:EmcCat4_Point_Phi",
  ":emc:EmcCat4_Sigma_Eta",
  ":emc:EmcCat4_Sigma_Phi",
  ":emc:EmcCat4_Delta_Eta",
  ":emc:EmcCat4_Delta_Phi",
  ":emc:EmcCat4_Points_Multiplicity",
  ":emc:EmcCat4_Track_Momenta",
  " Point Flag",
  "QaGRpvtxDx",
  "QaGRpvtxDy",
  "QaGRpvtxDz",
  "QaBbcAdcES",
  "QaBbcAdcEL",
  "QaBbcAdcWS",
  "QaBbcAdcWL",
  "QaBbcTdcES",
  "QaBbcTdcEL",
  "QaBbcTdcWS",
  "QaBbcTdcWL",
  ":fpd:QaFpdTop0",
  ":fpd:QaFpdTop1",
  ":fpd:QaFpdBottom0",
  ":fpd:QaFpdBottom1",
  ":fpd:QaFpdSouth0",
  ":fpd:QaFpdSouth1",
  ":fpd:QaFpdNorth0",
  ":fpd:QaFpdNorth1",
  ":fpd:QaFpdSums0",
  ":fpd:QaFpdSums1",
  ":fpd:QaFpdSums2",
  ":fpd:QaFpdSums3",
  ":fpd:QaFpdSums4",
  ":fpd:QaFpdSums5",
  ":fpd:QaFpdSums6",
  ":fpd:QaFpdSums7"
