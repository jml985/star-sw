/***************************************************************************
 *
 * $Id: StDevice.hh,v 1.3 1999/09/23 21:25:20 calderon Exp $
   Generic device class.  Each (generic) StSubDetector object
   has a set of StDevices (like the TPC StSubDetector has 24
   sector StDevices).  Each StDevice object has a set of StDevRows
   (like the TPC sector StDevice has 45 padrows, each one is a StDevRow)

 * $Log: StDevice.hh,v $
 * Revision 1.3  1999/09/23 21:25:20  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
 


#ifndef StDevice_HH
#define StDevice_HH

#include <vector>
#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif

//class StDevRow;
#include "StDevRow.hh"

class StDevice{

private:
#ifndef ST_NO_TEMPLATE_DEF_ARGS
    vector<StDevRow*> mRows;
#else
     vector<StDevRow*, allocator<StDevRow*> > mRows;
#endif
    unsigned int mNRows;
    
public:
    StDevice(const unsigned int nRows);
    ~StDevice();
    
    StDevRow*         row(const int irow){return mRows[irow];};
    unsigned int      numOfRows() { return mNRows; };
};


#endif
