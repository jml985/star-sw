#ifndef StiKalmanTrack_H
#define StiKalmanTrack_H 1

//SCL
#include "StThreeVector.hh"
#include "StThreeVectorF.hh"

//STD
#include <math.h>

//Sti
#include "StiObjectFactoryInterface.h"
#include "StiTrack.h"
#include "StiKalmanTrackNode.h"
#include "StiHitContainer.h"

class StiKalmanTrack : public StiTrack 
{
 public:
  
  // constructor/destructor/copy/etc
  
  StiKalmanTrack() 
    {
      reset();
    };
  virtual ~StiKalmanTrack()
    {
    };

    static void setKalmanTrackNodeFactory(StiObjectFactoryInterface<StiKalmanTrackNode>*);

    //Action method for polymorphic graphical behavior
    virtual void update();
    
  // Action methods
  // Implementation of virtua methods inherited
  // from StiTrack
  virtual void    reset();
  virtual void    getMomentum(double p[3], double e[6]) const ;
  virtual double  getPt()             const                   ;   // transverse momentum
  virtual double  getRapidity()       const                   ;   // rapidity
  virtual double  getPseudoRapidity() const                   ;   // pseudo rapidity
  virtual double  getPhi()            const                   ;   // azimuthal angle
  virtual double  getTanL()           const                   ;   // tan(lambda)
  virtual double  getDca(StiHit *h=0)    const;   // distance of closest approach to given point/hit
  virtual double  getDca2(StiTrack *t)   const;   // distance of closest approach to given track - 2D calc
  virtual double  getDca3(StiTrack *t)   const;   // distance of closest approach to given track - 3D calc
  virtual double  getMass()              const;   // mass when pid known
  virtual int     getCharge()            const;   // charge of the particle
  virtual double  getChi2()              const;   // chi2 of fit
  virtual int     getFitPointCount()   const;  // number of points used in fit
  // number of total number of points 
  // currently assigned to the track
  virtual int     getPointCount()      const;  
  virtual int     getStatus()          const;  // status of track
  
  virtual void  setCharge(int v);
  virtual void  setChi2(double v);        
  virtual void  setFitPointCount(int v);   
  virtual void  setPointCount(int v);      
  virtual void  setStatus(int v);        

  // Methods of this class
  StiKalmanTrackNode * getFirstNode()  const { return firstNode; };
  StiKalmanTrackNode * getLastNode()   const { return lastNode;  };
  void setFirstNode(StiKalmanTrackNode * n) {firstNode = n;};
  void setLastNode(StiKalmanTrackNode * n)  {lastNode  = n;};
  
  double  getSvtDedx()         const { return svtDedx;};
  double  getTpcDedx()         const { return tpcDedx;};
  void   setSvtDedx(double dedx) { svtDedx = dedx; };
  void   setTpcDedx(double dedx) { tpcDedx = dedx; };
  
  // convenience methods for adding/retrieving points
  StiKalmanTrackNode * addHit(StiHit *h);
  StiKalmanTrackNode * insertHit(StiHit *hInserted, StiHit * targetParent);
  void removeHit(StiHit *h);
  void removeAllHits();
  int  getHitCount();
  StiHit * getHit(int index);
  StiKalmanTrackNode * findHit(StiHit * h);
  
  void initialize(double curvature,
		  double tanl,
		  const StThreeVectorD& origin,
		  const hitvector &);
  StiKalmanTrackNode *  getNodeNear(double x) const;
  StThreeVector<double> getPointNear(double x) const;
  StThreeVector<double> getGlobalPointNear(double x) const;
    
    virtual StThreeVector<double> getMomentumAtOrigin();
    virtual StThreeVector<double> getMomentumNear(double x);
    virtual StThreeVector<double> getHitPositionNear(double x) const;

protected:
    
  static StiObjectFactoryInterface<StiKalmanTrackNode> * trackNodeFactory;

  StiKalmanTrackNode * firstNode;
  StiKalmanTrackNode * lastNode;
  double svtDedx;
  double tpcDedx;
};

#endif

