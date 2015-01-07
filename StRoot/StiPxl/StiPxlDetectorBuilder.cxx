/* $Id: StiPxlDetectorBuilder.cxx,v 1.78.2.17 2015/01/07 19:45:33 smirnovd Exp $ */

#include <assert.h>
#include <sstream>
#include <string>

#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiDetector.h"
#include "Sti/Base/Factory.h"
#include "Sti/StiToolkit.h"
#include "Sti/StiNeverActiveFunctor.h"
#include "StiPxl/StiPxlDetectorBuilder.h"
#include "StiPxl/StiPxlIsActiveFunctor.h"
#include "StiPxl/StiPxlHitErrorCalculator.h"
#include "tables/St_HitError_Table.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEventTypes.h"
#include "StPxlDbMaker/StPxlDb.h"
#include "StPxlDbMaker/StPxlDbMaker.h"
#include "StPxlUtil/StPxlConstants.h"
#include "StBFChain/StBFChain.h"



/**
 * Parameterized hit error calculator.  Given a track (dip, cross, pt, etc)
 * returns average error once you actually want to do tracking, the results
 * depend strongly on the numbers below.
 *
   numbering should be the following :
   hardware : sector ladder   ITTF : layer  ladder
   1      1                          1      0
   1      2                          1      1
   1      3                          1      2
   1      4                          0      0

   2      1                          1      3
   2      2                          1      4
   2      3                          1      5
   2      4                          0      1
   (...)
   10     1                          1     27
   10     2                          1     28
   10     3                          1     29
   10     4                          0     9
 */
StiPxlDetectorBuilder::StiPxlDetectorBuilder(bool active, bool buildIdealGeom) :
   StiDetectorBuilder("Pixel", active), mBuildIdealGeom(buildIdealGeom), mPxlDb(0)
{
   setGroupId(kPxlId);
}


/** Build the pixel detector components. */
void StiPxlDetectorBuilder::buildDetectors(StMaker &source)
{
   LOG_INFO << "StiPxlDetectorBuilder::buildDetectors() -I- Started" << endm;

   SetCurrentDetectorBuilder(this);

   // Access the (survey) geometry if requested by the user
   if (!mBuildIdealGeom) {
      TObjectSet *pxlDbDataSet = (TObjectSet*) source.GetDataSet("pxl_db");

      if (!pxlDbDataSet) {
         LOG_ERROR << "StiPxlDetectorBuilder::buildDetectors: PXL geometry was requested from "
            "DB but no StPxlDb object found. Check for pxlDb option in BFC chain" << endm;
         exit(EXIT_FAILURE);
      }

      mPxlDb = (StPxlDb*) pxlDbDataSet->GetObject();
      assert(mPxlDb);

      LOG_INFO << "StiPxlDetectorBuilder::buildDetectors: Will build PXL geometry from DB tables" << endm;
   }

   // Gas material must be defined. Here we use air properties
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("AIR");

   _gasMat = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                    : add(new StiMaterial("AIR", 7.3, 14.61, 0.001205, 30420.));

   if (StiVMCToolKit::GetVMC()) {
      useVMCGeometry();
      buildInactiveVolumes();
   }
}


/** Builds the sensors of the pixel detector. */
void StiPxlDetectorBuilder::useVMCGeometry()
{
   LOG_INFO << "StiPxlDetectorBuilder::useVMCGeometry() -I- Use VMC geometry" << endm;

   // Define silicon material used in manual construction of sensitive layers in this builder
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("SILICON");

   StiMaterial* silicon = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                                 : add(new StiMaterial("SILICON", 14, 28.0855, 2.33, 9.36) );

   // Build active sti volumes for pixel sensors
   // Use the "middle" sensor on the ladder to extract alignment corrections from DB
   int iSensor = floor(kNumberOfPxlSensorsPerLadder/2);

   for (int iSector = 1; iSector <= kNumberOfPxlSectors; ++iSector)
   {
      for (int iLadder = 1; iLadder <= kNumberOfPxlLaddersPerSector; ++iLadder)
      {
         std::ostringstream geoPath;
         geoPath << "/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1/PXLA_" << iSector << "/LADR_" << iLadder << "/PXSI_1/PLAC_1";

         bool isAvail = gGeoManager->cd(geoPath.str().c_str());

         if (!isAvail) {
            Warning("useVMCGeometry()", "Cannot find path to PLAC (pixel sensitive) node. Skipping to next ladder...");
            continue;
         }

         TGeoVolume* sensorVol = gGeoManager->GetCurrentNode()->GetVolume();
         TGeoMatrix* sensorMatrix = 0;

         if (mBuildIdealGeom) {
            sensorMatrix = gGeoManager->MakePhysicalNode(geoPath.str().c_str())->GetMatrix();
         } else {
            sensorMatrix = (TGeoMatrix*) mPxlDb->geoHMatrixSensorOnGlobal(iSector, iLadder, iSensor);
         }

         if (!sensorMatrix) {
            Warning("useVMCGeometry()", "Could not get PXL sensor position matrix. Skipping to next ladder...");
            continue;
         }

         // Build global rotation for the sensor
         TGeoRotation sensorRot(*sensorMatrix);

         TGeoBBox *sensorBBox = (TGeoBBox*) sensorVol->GetShape();

         // Split the ladder in two halves
         for (int iLadderHalf = 1; iLadderHalf <= 2; iLadderHalf++) {
            // Convert center of the half sensor geobox to coordinates in the global coordinate system
            double sensorXyzLocal[3]  = {};
            double sensorXyzGlobal[3] = {};

            // Shift the halves by a quater width
            sensorXyzLocal[0] = iLadderHalf == 1 ? -sensorBBox->GetDX()/2 : sensorBBox->GetDX()/2;

            sensorMatrix->LocalToMaster(sensorXyzLocal, sensorXyzGlobal);

            TVector3 sensorVec(sensorXyzGlobal);

            // Create new Sti shape based on the sensor geometry
            std::string halfLadderName(geoPath.str() + (iLadderHalf == 1 ? "_HALF1" : "_HALF2") );
            double sensorLength = kNumberOfPxlSensorsPerLadder * (sensorBBox->GetDZ() + 0.02); // halfDepth + 0.02 ~= (dead edge + sensor gap)/2
            StiShape *stiShape = new StiPlanarShape(halfLadderName.c_str(), sensorLength, 2*sensorBBox->GetDY(), sensorBBox->GetDX()/2);

            add(stiShape);

            Double_t phi  = sensorVec.Phi();
            Double_t phiD = sensorRot.GetPhiRotation() / 180 * M_PI;
            Double_t r    = sensorVec.Perp(); // Ignore the z component if any
            double normVecMag = fabs(r * sin(phi - phiD));
            TVector3 normVec(cos(phiD + M_PI_2), sin(phiD + M_PI_2), 0);

            if (normVec.Dot(sensorVec) < 0) normVec *= -normVecMag;
            else                            normVec *=  normVecMag;

            // Volume positioning
            StiPlacement *pPlacement = new StiPlacement();

            pPlacement->setZcenter(0);
            pPlacement->setLayerRadius(r);
            pPlacement->setLayerAngle(phi);
            pPlacement->setRegion(StiPlacement::kMidRapidity);

            double centerOrient = sensorVec.Phi() - normVec.Phi();
            pPlacement->setNormalRep(normVec.Phi(), normVecMag, r * sin(centerOrient));

            // Build final detector object
            StiDetector *stiDetector = getDetectorFactory()->getInstance();
            StiIsActiveFunctor* isActive = _active ?  new StiPxlIsActiveFunctor :
               static_cast<StiIsActiveFunctor*>(new StiNeverActiveFunctor);

            stiDetector->setProperties(halfLadderName, isActive, stiShape, pPlacement, getGasMat(), silicon);
            stiDetector->setHitErrorCalculator(StiPxlHitErrorCalculator::instance());

            // Convert geo sensor id to Sti indices. We do not check the validity
            // of returned values because the input is valid a priori in this case
            int stiRow, stiSensor;
            convertSensor2StiId(iSector, iLadder, iLadderHalf, stiRow, stiSensor);

            // Add created sensitive PXL layer to Sti
            add(stiRow, stiSensor, stiDetector);
         }
      }
   }
}


/**
 * Returns the active StiDetector corresponding to a sensitive layer in PXL. The
 * StiDetector is normally created by this StiDetectorBuilder and identified by
 * its sector, ladder, and sesortHalf id-s. An active volume can have hits
 * associated with it. The ladder id is expected to follow the human friendly
 * numbering scheme, i.e.
 *
 * <pre>
 * 1 <= sector <= kNumberOfPxlSectors
 * 1 <= ladder <= kNumberOfPxlLaddersPerSector
 * 1 <= sensorHalf <= 2
 * </pre>
 */
const StiDetector* StiPxlDetectorBuilder::getActiveDetector(int sector, int ladder, int sensorHalf) const
{
   int stiRow, stiSensor;
   convertSensor2StiId(sector, ladder, sensorHalf, stiRow, stiSensor);

   return stiRow < 0 ? 0 : getDetector(stiRow, stiSensor);
}


/** Creates inactive sti volumes for the pixel support material. */
void StiPxlDetectorBuilder::buildInactiveVolumes()
{
   // Build average inactive volumes
   const VolumeMap_t pxlVolumes[] = {
      {"DTUH", "Dtube part of pixel support",  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},

      // Planar components of the pixel sector support .../PSUP_\d/...
      {"PSHA", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSHC", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSHE", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSHG", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSAL", "Long tube in half pixel support",  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSAK", "Short tube in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSCL", "Plane in half pixel support",      "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSCK", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSAB", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSAE", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},
      {"PSMD", "Detail in half pixel support",     "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1", "", ""},

      // The following are the largest planar components of the central pixel
      // sector support .../PXLA_\d/...
      {"PXRB", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      {"PXTR", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      {"PXTM", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      {"PXTL", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      {"PXLB", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      {"PXIB", "Pixel sector support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},

      // Inactive material close to silicon layers
      { "DRIV", "Driver Board",         "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      { "GLUA", "Glu layer A",          "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      { "GLUB", "Glu layer B",          "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      { "GLUC", "Glu layer C",          "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      { "ALCA", "Aluminium cable",      "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},
      { "CFBK", "Carbon Fiber Backing", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1", "", ""},

      // These are the components of the pixel support tube (PSTM)
      { "APTS1", "Tube shell", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSTM_1/APTS_1", "", ""},
      { "PITN1", "Pixel insertion TubeNaked", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSTM_1/PITN_1", "", ""}
   };

   int nPxlVolumes = sizeof(pxlVolumes) / sizeof(VolumeMap_t);
   LOG_DEBUG << " # of volume(s) : " << nPxlVolumes << endm;

   for (int i = 0; i < nPxlVolumes; i++) {

      if (! gGeoManager->cd(pxlVolumes[i].path) ) {
         Warning("buildInactiveVolumes()", "Cannot find path to %s node. Skipping to next node...", pxlVolumes[i].name);
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "\n\n" << endm
                << "Current node: " << i << " of " << nPxlVolumes << ", path: " << pxlVolumes[i].path << endm
                << "Number of daughters: " << geoNode->GetNdaughters() << ", weight: " << geoNode->GetVolume()->Weight(0.01, "a") << endm;

      StiVMCToolKit::LoopOverNodes(geoNode, pxlVolumes[i].path, pxlVolumes[i].name, MakeAverageVolume);
   }
}


/**
 * Convert natural sensor id (sector/ladder/sensorHalf) to Sti indices stiRow
 * and stiSensor. If the input values are not within valid ranges unphisical
 * (i.e. negative) Sti indices returned.
 *
 * The numbering is:
 *
 * <pre>
 * sector ladder sensorHalf -> stiRow stiSensor
 *
 * 1      1      1          -> 0      0
 * 1      1      2          -> 1      0
 * 1      2      1          -> 2      0
 * 1      2      2          -> 3      0
 * 1      3      1          -> 2      1
 * 1      3      2          -> 3      1
 * 1      4      1          -> 2      2
 * 1      4      2          -> 3      2
 *
 * 2      1      1          -> 0      1
 * 2      1      2          -> 1      1
 * 2      2      1          -> 2      3
 * 2      2      2          -> 3      3
 * 2      3      1          -> 2      4
 * 2      3      2          -> 3      4
 * 2      4      1          -> 2      5
 * 2      4      2          -> 3      5
 *
 * ...
 *
 * </pre>
 */
void StiPxlDetectorBuilder::convertSensor2StiId(int sector, int ladder, int sensorHalf, int& stiRow, int& stiSensor)
{
   // Check validity of input values
   if (sector < 1 || sector > kNumberOfPxlSectors ||
       ladder < 1 || ladder > kNumberOfPxlLaddersPerSector ||
       sensorHalf < 1 || sensorHalf > 2)
   {
      stiRow = stiSensor = -1;
      return;
   }

   if (ladder == 1) {
      stiRow = sensorHalf == 1 ? 0 : 1;
      stiSensor = (sector - 1);
   } else { // ladder = 2, 3, 4
      stiRow = sensorHalf == 1 ? 2 : 3;
      stiSensor = (sector - 1) * (kNumberOfPxlLaddersPerSector - 1) + (ladder - 2);
   }
}
