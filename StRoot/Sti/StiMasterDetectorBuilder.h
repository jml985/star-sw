#ifndef StiMasterDetectorBuilder_H
#define StiMasterDetectorBuilder_H

#include "Sti/Base/Vectorized.h"
#include "StiDetectorBuilder.h"

class StiMasterDetectorBuilder : public StiDetectorBuilder, public Vectorized<StiDetectorBuilder>
{
public:
    StiMasterDetectorBuilder();
    virtual ~StiMasterDetectorBuilder(); 
    virtual bool hasMore() const;
    virtual StiDetector * next();
    virtual void reset();
    virtual void build();
    virtual void add(StiDetectorBuilder *builder);

//VP  to supress "hide" warnings
    virtual StiMaterial * add(StiMaterial *material)
            {return StiDetectorBuilder::add(material);}
    virtual StiShape    * add(StiShape    *shape)
            {return StiDetectorBuilder::add(shape);}
    virtual StiDetector * add(StiDetector *detector)
            {return StiDetectorBuilder::add(detector);}
    virtual StiDetector * add(unsigned int row, unsigned int sector, StiDetector *detector)
            {return StiDetectorBuilder::add(row,sector,detector);}
};

#endif 
