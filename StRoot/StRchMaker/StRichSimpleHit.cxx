/***************************************************************************
 *
 * $Id: StRichSimpleHit.cxx,v 2.0 2000/08/09 16:22:12 gans Exp $
 *
 * Author: bl
 ***************************************************************************
 *
 * Description: Implementation of Hit definition
 *
 ***************************************************************************
 *
 * $Log: StRichSimpleHit.cxx,v $
 * Revision 2.0  2000/08/09 16:22:12  gans
 * Cosmetic Changes. Naming convention for TDrawable objects
 *
 * Revision 2.1  2000/09/29 19:01:24  lasiuk
 * enumerated types added for flags
 * c'tor includes cp from persistent hit
 * number of pads added as a member
 *
 * Revision 2.0  2000/08/09 16:22:12  gans
 * Cosmetic Changes. Naming convention for TDrawable objects
 *
 * Revision 1.3  2000/05/23 16:55:55  lasiuk
 * Incorporate new MC info
 * add clone() where necessary
 * accomodate name changes
 *
 * Revision 1.2  2000/05/18 11:42:39  lasiuk
 * mods for pre StEvent writing
 *
 * Revision 1.1  2000/05/18 11:34:14  lasiuk
 * iRename revision
 *
 **************************************************************************/

    : mLocal(xl), mSigma(dx)

StRichSimpleHit::StRichSimpleHit() {/* nopt */}

StRichSimpleHit::StRichSimpleHit(const StThreeVector<double>& xl, const StThreeVector<double>& dx)
    : mLocal(xl), mSigma(dx), mFlags(0)
StRichSimpleHit::StRichSimpleHit(const StRichHit*)
      mSigma(hit->positionError().x(), hit->positionError().y(), hit->positionError().z()),
      mCharge(hit->charge()),
      mMaxAmplitude(hit->maxAmplitude()),
      mClusterNumber(hit->clusterNumber()),
      mFlags(hit->reservedLong())
{
    cout << "StRichSimpleHit::StRichSimpleHit(const StRichHit*)" << endl;
}
#endif

StRichSimpleHit::~StRichSimpleHit() {/* nopt */}

    for(int ii=0; ii<numberOfBits; ii++) {
	unsigned long mask = pow(2.,ii);
	cout << ((mFlags & mask) == 0 ? 0 : 1);
    }
    cout << endl;
}
