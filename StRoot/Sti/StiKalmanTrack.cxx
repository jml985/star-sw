//Std
#include <stdexcept>
#include <math.h>

//Sti
#include "StiKTNIterator.h"
#include "StiHit.h"
#include "StiDefaultMutableTreeNode.h"
#include "StiKalmanTrackNode.h"
#include "StiKalmanTrack.h"
#include "StiDetector.h"
#include "StiPlacement.h"
#include "StiKTNIterator.h"
#include "StPhysicalHelix.hh"
#include "StiGeometryTransform.h"

ostream& operator<<(ostream&, const StiHit&);

StiObjectFactoryInterface<StiKalmanTrackNode>* StiKalmanTrack::trackNodeFactory = 0;

/*! 
   Reset the class members to their default state.
   This method is called by the ctor of the class to initialize the
   members of the class to an "empty" or null track state. The
   method must also be called everytime an instance of this class is
   retrieved from its factory in order to set the first and last
   nodes to "null" thus guaranteeing that the track object is empty
   i.e. does not represent any track and is thus ready for a new
   search and reconstruction.  It is guaranteed that a call to
   reset() fully propogates up the inheritance tree.
 */
void StiKalmanTrack::reset()
{
	StiTrack::reset();
  firstNode = 0;
	lastNode = 0;
  trackingDirection = kOutsideIn;
  fittingDirection  = kOutsideIn;
}

void StiKalmanTrack::update()
{
  //cout<<"void StiKalmanTrack::update()"<<endl;
  return;
}
 

/*! 
	Set the factory used for the creation of kalman track nodes.
	\see StiKalmanTrackNodeFactory
*/
void StiKalmanTrack::setKalmanTrackNodeFactory(StiObjectFactoryInterface<StiKalmanTrackNode>* val)
{
  trackNodeFactory = val;
}

/*! 
   Calculates and returns the momentum and error of the track 
	 <p>
   This method calculates and returns in the two arrays provided as arguments the 
   3-momentum and error of the track in Star global coordinates. The 3-momentum 
   is calculated at the inner most point associated with the track. The inner-most 
   point may or may not be the main vertex of the event. Care should thus be exercised 
   while using this method. 
	 <p>
   The error is calculated (and returned) only if a non null array is passed as a second
   argument. It is thus possible to get the momentum without a lengthy calculation 
   of the error matrix. The error error matrix corresponds to a full covariance matrix.
   The definition of the error matrix is described in the introduction of this class
   definition. Note that the actual calculation of the momentum and associated error 
   is delegated to the track node class and uses the inner most node of the track.
 */
void StiKalmanTrack::getMomentum(double p[3], double e[6]) const
{
	// return the momentum of the track at the inner most node held by this track
    // which may (or not) be the primary vertex. 
    // this will need to be refined...
    getInnerMostHitNode()->getMomentum(p,e);
}

/*!
   Calculates and returns the transverse momentum of the track at the inner most node 
   held by this track which may or (or not) be the primary vertex. 
*/
double  StiKalmanTrack::getPt() const
{
    return getInnerMostHitNode()->getPt();
}

/*!
   Calculates and returns the track curvature at the inner most node held by this track.
	 <p>
	 Obtains the curvature from the inner most hit node associated with this track.
*/
double StiKalmanTrack::getCurvature() const
{
  return getInnerMostHitNode()->fP3;
}

/*!
	Returns the rapidity of the track if the mass is known.
	<p>
	<ol>
  <li>Obtains the momentum from the inner most hit node associated with the track.</li>
  <li>Obtains the mass of this track using the getMass() method. If the mass returned
	is negative, throws a runtime_error exception.</li>
	</ol>
	\throws runtime_error
	\return rapidity
 */
double  StiKalmanTrack::getRapidity()       const 
{
  // returns the rapidity of the particle at the inner most node held by this track
  // which may (or not) be the primary vertex. 
  double p[3];
  StiKalmanTrackNode *  inner = getInnerMostHitNode();
  inner->getMomentum(p,0);
  double mass = getMass();
  if (mass<0)
    throw runtime_error("StiKalmanTrack::getRapidity() - particle mass unknown");
  double e = sqrt(mass*mass+p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
	if (e<=p[2])
		throw runtime_error("StiKalmanTrack::getRapidity() - Error: e<=pz");
	return 0.5*log(e+p[2]/e-p[2]);
}

/*!
	Returns the pseudo-rapidity of the track.
	<p>
	<ol>
  <li>Obtains the helix pitch angle from the inner most hit node associated with the track.</li>
  <li>Calculate/return the pseudo-rapidity using the pitch angle.</li>
	</ol>
	\return pseudo-rapidity
 */
double  StiKalmanTrack::getPseudoRapidity() const
{
  // Return pseudo rapidity of the particle at the inner most node held by this track
  // which may (or not) be the primary vertex. 
  return -log(tan(M_PI/4.-(getInnerMostHitNode()->getTanL()/2.)));
}

/*! 
	Returns the azimuthal angle of the track determined at the inner most point of the track
	hich may or may not be a vertex.
	\return phi in radian
*/
double  StiKalmanTrack::getPhi()            const 
{
  double p[3];
  getInnerMostHitNode()->getMomentum(p,0);
  return atan2(p[1],p[0]);
}

/*!
	Return tan(lambda) of the particle at the inner most node held by this track
	which may (or not) be the primary vertex. 
	\return tan(lambda)
*/
double  StiKalmanTrack::getTanL()           const 
{
  return getInnerMostHitNode()->getTanL();
}


/*!
	Add a hit to this track.
	<p>
  If the current lastNode is non null, 
	<ol>
	<li>Insert the given hit in a StiKalmanTrackNode instance.</li>
	<li>Add the new node as a child to the current last node.</li>
	<li>Make the new node the last node of this track.</li>
	</ol>
	else
	<ol>
	<li>Insert the given hit in a StiKalmanTrackNode instance.</li>
	</ol>
*/
StiKalmanTrackNode * StiKalmanTrack::addHit(StiHit *h)
{
  if (lastNode!=0)
    {
      StiKalmanTrackNode * n = trackNodeFactory->getObject();
      n->reset();
      n->setHit(h);
      n->fX = h->x();
      lastNode->add(n);
			lastNode = n;
      return n;
    }
  else 
    {
      firstNode  = trackNodeFactory->getObject(); 
      firstNode->reset();
      firstNode->setHit(h);
      firstNode->fX = h->x();
			lastNode = firstNode;
      return firstNode;
    }
}

double StiKalmanTrack::getTpcDedx() const
{
  return 0.; // to be changed...
}

double StiKalmanTrack::getSvtDedx() const
{
  return 0.; // to be changed...
}

StiKalmanTrackNode * StiKalmanTrack::insertHit(StiHit *hInserted, StiHit * targetParent)
{
  // Add a hit to this track right after the given hit
  // Note that this method is slow because it needs to 
  // find the parent hit first...
  // Note that if the targetParent hit is null, it is assumed
  // the intent is to add the hit before the firstNode
  // It is further assumed that the targetParent has at most
  // one child.
  
  StiKalmanTrackNode * n = trackNodeFactory->getObject();
  n->reset();
  n->setHit(hInserted);
  if (targetParent==0)
    {
      if (firstNode!=0)
	{
	  n->add(firstNode);
	  firstNode = n;
	}
      else
	  firstNode = n;
    }
  else
    {
      StiKalmanTrackNode * pn = findHit(targetParent);
      if (pn==0)
	  throw logic_error("SKT::insertHit() - ERROR - Attempted hit insertion after hit which does not belong to this track");
      else
	{
	  StiKalmanTrackNode * cn = static_cast<StiKalmanTrackNode *> (pn->getFirstChild());
	  if (cn!=0)
	    {
	      pn->remove(cn);
	      n->add(cn);
	      pn->add(n);
	    }
	  else
	    {
	      // hit added as lastChild
	      pn->add(n);
	    }
	}
    }
  return n;
}

void StiKalmanTrack::removeHit(StiHit *h)
{
  // remove the given hit (and node) from this track
  // It is assume that the hit has at most one child
  
  StiKalmanTrackNode * n = findHit(h);
  if (n!=0)
    {
      // the hit belongs to this track, let's remove it
      StiKalmanTrackNode * cn = static_cast<StiKalmanTrackNode *> (n->getFirstChild());
      
      if (cn==0)
	{
	  // no child, this is the last hit
	  StiKalmanTrackNode * pn = static_cast<StiKalmanTrackNode *> (n->getParent());
	  if (pn==0)
	    firstNode = 0;
	  else
	    pn->remove(n);
	}
      else
	{
	  // child exist
	  StiKalmanTrackNode * pn = static_cast<StiKalmanTrackNode *> (n->getParent());
	  if (pn==0)
	    {
	      // no parent, this is the first hit
	      cn->setParent(0); 
	      firstNode = cn;
	    }
	  else
	    {
	      pn->remove(n);
	      pn->add(cn);
	    }
	}
    }
  else
    throw logic_error("StiKalmanTrack::removeHit() - Error - Given hit does not belong to this track");
}

StiKalmanTrackNode * StiKalmanTrack::findHit(StiHit * h)
{
  if (firstNode==0)
    return 0;
  else
    {
      if (h==firstNode->getHit())
	  return firstNode;
      StiKalmanTrackNode * n = firstNode;
      while (n->getChildCount()>0)
	{
	  n = static_cast<StiKalmanTrackNode *> (n->getFirstChild());
	  if (h==n->getHit())
	      return firstNode;
	}
    }
  return 0;
}

/*! Remove all hits from this track.
	<p>
	Remove all references to hits from this track be setting the firstNode and lastNode 
	pointers to "0".
	<h3>Note</h3>
	<ol>
	<li>No need to destroy any object since the memory for the nodes and hits is owned 
	by the factory that supply these.</li>
	</ol>
*/
void StiKalmanTrack::removeAllHits()
{
    firstNode = 0;
		lastNode  = 0;
}

/*! Initialization of this kalman track from external parameters.
	<p>
	This track object is initialized on the basis of parameters determined externally. The
parameters consist of the track curvature, the tangent of pitch angle, the origin of 
the helix, and a vector of hits already associated with the track.

<h3>Arguments:</h3>
<TABLE BORDER="0" CELLPADDING="2" CELLSPACING="0" WIDTH="100%">
  <TR> <TD WIDTH="10%">curvature</TD> <TD WIDTH="90%">1/radius of the tack.</TD>  </TR>
  <TR> <TD WIDTH="10%">tanl</TD>      <TD WIDTH="90%">tan(pitch angle)</TD> </TR>
  <TR> <TD WIDTH="10%">origin</TD>    <TD WIDTH="90%">origin of the track in global coordinates.</TD> </TR>
  <TR> <TD WIDTH="10%">v</TD>         <TD WIDTH="90%">vector of hits associated with this track.</TD> </TR>
</TABLE>
<h3>Internal Track Representation: </h3>

<TABLE BORDER="0" CELLPADDING="2" CELLSPACING="0" WIDTH="80%">
  <TR>
    <TD WIDTH="10%">x</TD>
    <TD WIDTH="10%">fX</TD>
    <TD WIDTH="80%">independent variable</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">state[0]</TD>
    <TD WIDTH="10%">fP0</TD>
    <TD WIDTH="80%">y; ordinate at &quot;x&quot;</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">state[1]</TD>
    <TD WIDTH="10%">fP1</TD>
    <TD WIDTH="80%">z; position along beam axis at &quot;x&quot;</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">state[2]</TD>
    <TD WIDTH="10%">fP2</TD>
    <TD WIDTH="80%">eta=C*x0; C == curvature, x0==position of helix center.</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">state[3]</TD>
    <TD WIDTH="10%">fP3</TD>
    <TD WIDTH="80%">C (local) curvature of the track</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">state[4]</TD>
    <TD WIDTH="10%">fP4</TD>
    <TD WIDTH="80%">tan(l)</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[0]</TD>
    <TD WIDTH="10%">fC00</TD>
    <TD WIDTH="80%">Error Matrix - Symmetric e.g. fC20=fC02</TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[1]</TD>
    <TD WIDTH="10%">fC10</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[2]</TD>
    <TD WIDTH="10%">fC11</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[3]</TD>
    <TD WIDTH="10%">fC20</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[4]</TD>
    <TD WIDTH="10%">fC21</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[5]</TD>
    <TD WIDTH="10%">fC22</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[6]</TD>
    <TD WIDTH="10%">fC30</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[7]</TD>
    <TD WIDTH="10%">fC31</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[8]</TD>
    <TD WIDTH="10%">fC32</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[9]</TD>
    <TD WIDTH="10%">fC33</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[10]</TD>
    <TD WIDTH="10%">fC40</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[11]</TD>
    <TD WIDTH="10%">fC41</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[12]</TD>
    <TD WIDTH="10%">fC42</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[13]</TD>
    <TD WIDTH="10%">fC43</TD>
    <TD WIDTH="80%"></TD>
  </TR>
  <TR>
    <TD WIDTH="10%">error[14]</TD>
    <TD WIDTH="10%">fC44</TD>
    <TD WIDTH="80%"></TD>
  </TR>
</TABLE>


<h3>Algorithm:</h3>
<ol>
<li>Verify that a valid node factory exists.</li>
<LI>Use local arrays state and error to add and set all nodes of this track.</LI>
<LI>Use the same curvature, and tanl for all nodes as supplied in argument list.</li>
<li>Use Unit matrix for error matrix.</li>
<li>Loop over all hits of the input hit vector and create a track node for each.</LI>
<li>Paramters of the track node are set according to the y,z of the hits added.</LI>
<li>Hits given are transformed in the local coordinates of their detector.
</ol>
<h3>Notes:</h3>
<OL>
<LI>Throws a logic_error exception if no track node factory is available.</li>
<LI>Throws a logic_error exception if the factory
  is not a castable to a factory of StiKalmanTrackNode.</li>
<li>Throws a logic error exception if hits do not have a valid pointer to a detector object.</li>
</OL>
*/
void StiKalmanTrack::initialize(double curvature,
				double tanl,
				const StThreeVectorD& origin,
				const hitvector & v)
{
    if (!trackNodeFactory) 
			throw logic_error("StiKalmanTrack::initialize()\tERROR:\tNo Kalman Track Node Factory-Abort");
    StiObjectFactoryInterface<StiKalmanTrackNode>* fac = trackNodeFactory;
    if (!fac) 
			throw logic_error("StiKalmanTrack::initialize(). ERROR:\tFactory cast failed-Abort");
    //StThreeVectorD stiOrigin;
    double alpha,alphaP,eta;  
    hitvector::const_iterator it;
    double state[5];  
    double error[15];
    // These are constant for all hits
    state[3]=curvature;
    state[4]=tanl;
    // For the time being set a diagonal error matrx
    error[0] = 1.;  
    error[1] = 0.; error[2] = 1.;  
    error[3] = 0.; error[4] = 0.; error[5] = 1.;  
    error[6] = 0.; error[7] = 0.; error[8] = 0.;  error[9]  = 1.;  
    error[10]= 0.; error[11] = 0.;error[12] = 0.; error[13] = 0.;  error[14] = 1.;
    // do the transfer here
    StiKalmanTrackNode * node  = 0;
    StiKalmanTrackNode * pNode = 0;
    eta = 0.;
    for (it=v.begin(); it!=v.end(); ++it)
      {
				const StiDetector* layer = (*it)->detector();
				if (!layer) 
					throw logic_error("StiKalmanTrack::initialize() ERROR:\t Hit has null detector.");
				alpha = layer->getPlacement()->getNormalRefAngle();
				node = fac->getObject();
				if (node==0)
					throw logic_error("StiKalmanTrack::initialize() ERROR:\t Null node returned by Node factory");
				node->reset();
				if (pNode==0)
					alphaP = -99999.; // no parent, set crazy value
				else
					alphaP = pNode->fAlpha; // value of the parent
				if (alphaP!=alpha)
					{
						StThreeVectorD temp = origin;
						temp.rotateZ(-alpha);
						eta = curvature*temp.x();
					}
				state[0] = (*it)->y(); 
				state[1] = (*it)->z(); 
				state[2] = eta;
				node->set((*it), alpha, (*it)->x(), state,error, 0., 0.);
				if (pNode==0) 
					firstNode = node;
				else
						pNode->add(node);
				pNode = node;
				lastNode = node;
      }
}

StiKalmanTrackNode * StiKalmanTrack::getNodeNear(double x) const
{
    if (firstNode==0)  // node in this track, return a null state and error
	return 0;
    //cout << "StiKalmanTrack::getNodeNear(" << x << ") called" << endl;
    //cout << *firstNode << endl;
    StiDefaultMutableTreeNodeVector* nodes  = firstNode->breadthFirstEnumeration();
    double minDist  = 1.E10;
    double xx, diff;
    StiKalmanTrackNode * bestNode = firstNode;
    StiDefaultMutableTreeNodeIterator it;
    for (it=nodes->begin(); it!=nodes->end(); it++)
	{
	    StiKalmanTrackNode * node = static_cast<StiKalmanTrackNode *>(*it);
	    xx = node->fX;
	    diff = xx-x; if (diff<0) diff = -diff;
	    //cout << "===> x/diff:" << xx << "\t" << diff << endl;
	    if (diff<minDist) 
		{
		    minDist = diff;
		    bestNode = node;
		}
	}
    delete nodes;
    return bestNode;
}

StThreeVector<double>
StiKalmanTrack::getHitPositionNear(double x) const
{
	/*
		Returns the hit position associated with the node nearest to the given "x" value.
	*/
	StiKalmanTrackNode * node = getNodeNear(x);
	if (node==0)
		throw logic_error("StiKalmanTrack::getHitPositionNear(double x) - ERROR - node==0");
	StiHit * hit = node->getHit();
	if (hit==0)
		throw runtime_error("StiKalmanTrack::getHitPositionNear(double x) - ERROR - hit==0");
	StThreeVectorF pos = hit->globalPosition();
	return StThreeVector<double>( pos.x(), pos.y(), pos.z() );
}

StThreeVector<double> StiKalmanTrack::getPointNear(double x) const
{
	// returns point in local coordinates
	StiKalmanTrackNode * node = getNodeNear(x);
	if (node==0)
		throw logic_error("StiKalmanTrack::getPointNear(double x) - ERROR - node==0");
	return StThreeVector<double>(node->fX, node->fP0, node->fP1);
}

StThreeVector<double> StiKalmanTrack::getGlobalPointNear(double x) const
{
	// returns point in local coordinates
	double xx,yy,zz;
	StiKalmanTrackNode * node = getNodeNear(x);
	if (node==0)
		throw logic_error("StiKalmanTrack::getGlobalPointNear(double x) - ERROR - node==0");
	xx = node->fX;
	yy = node->fP0;
	zz = node->fP1;
	double alpha = node->fAlpha;
	double ca = cos(alpha);
	double sa = sin(alpha);
	return (StThreeVector<double>(ca*xx-sa*yy, sa*xx+ca*yy, zz));
}


StThreeVector<double> StiKalmanTrack::getGlobalPointAt(double x) const
{
  // returns a point along the track in global coordinates at radius "x"
  // Note that if the track does not pass at or near the given x value
  // the null vector is returned.
  //
  // Algorithm: One first look for the node closest to "x". A "fresh"
  // node is obtained from the factory, and its state is set to that
  // of the found node. Then the fresh node is "propagated" to the 
  // desired "x" position. A null vector (i.e. [0,0,0]) is returned
  // if anything fails...

    double xx,yy,zz;
    StiKalmanTrackNode * nearNode = getNodeNear(x);
    if (nearNode==0)
			throw logic_error("StiKalmanTrack::getGlobalPointAt(double x) - ERROR - nearNode==0");
    StiObjectFactoryInterface<StiKalmanTrackNode> * f 
			= static_cast<StiObjectFactoryInterface<StiKalmanTrackNode>*>(trackNodeFactory);
		if (f==0)
			throw logic_error("StiKalmanTrack::getGlobalPointAt(double x) - ERROR - no factory f==0");
    StiKalmanTrackNode * n = f->getObject();
    if (n==0)
			throw logic_error("StiKalmanTrack::getGlobalPointAt(double x) - ERROR - n==0");
    n->reset();
    n->setState(nearNode);
		n->propagate(x);
		xx = n->fX;
	  yy = n->fP0;
	  zz = n->fP1;
	  double alpha = n->fAlpha;
	  double ca = cos(alpha);
	  double sa = sin(alpha);
	  return (StThreeVector<double>(ca*xx-sa*yy, sa*xx+ca*yy, zz));
}


StThreeVector<double> StiKalmanTrack::getMomentumNear(double x)
{
    StiKalmanTrackNode * node = getNodeNear(x);
    double p[3];
    double e[6];
    node->getMomentum(p,e);
    StThreeVector<double> p3(p[0],p[1],p[2]);
    p3.rotateZ(node->fAlpha);
    return p3;
}

StThreeVector<double> StiKalmanTrack::getMomentumAtOrigin() const
{
    double px,py,pz;
    px = 0;
    py = 0;
    pz = 0;
    StiKalmanTrackNode * inner = getInnerMostNode();
    if (inner==0)
      throw logic_error("StiKalmanTrack::getMomentumAtOrigin() - ERROR - No node");
    inner->propagate(0.);
    double p[3];
    double e[6];
    inner->getMomentum(p,e);
    StThreeVector<double> p3(p[0],p[1],p[2]);
    p3.rotateZ(inner->fAlpha);
    return p3;
}

/*! Return the mass hypothesis used in the resconstruction of this track.
*/
double  StiKalmanTrack::getMass() const   
{
  return m;
}

/*! Return the track sign
   <h3>Notes</h3> 
   <ol>
   <li>Use the last node and the field.</li>
   </ol>
*/
int StiKalmanTrack::getCharge() const
{
	return  (lastNode->fP3*StiKalmanTrackNode::getFieldConstant()>0)?-1:1;
}

/*! Return the track chi2
   <h3>Notes</h3> 
   <ol>
   <li>Use the chi2 held by the last hit node used in the fit.</li>
   </ol>
*/
double  StiKalmanTrack::getChi2() const
{
	if (fittingDirection==kOutsideIn)
		{
			if (trackingDirection==kOutsideIn)
				return lastNode->fChi2;
			else
				return firstNode->fChi2;
		}
	else // insideOut
		{
			if (trackingDirection==kOutsideIn)
				return firstNode->fChi2;
			else
				return lastNode->fChi2;			
		}
}

/*! Calculate and return the distance of closest approach to given hit
   <h3>Notes</h3> 
   <ol>
   <li>No implementation.</li>
   <li>Returns 0</li>
   </ol>
*/
double  StiKalmanTrack::getDca(StiHit * h)    const
{
	return 0;
}

/*! Calculate and return the distance of closest approach to given track
   <h3>Notes</h3> 
   <ol>
   <li>No implementation.</li>
   <li>Returns 0</li>
   </ol>
*/
double  StiKalmanTrack::getDca(StiTrack *t)   const
{
  return 0;
}

/*! Calculate and return the distance of closest approach to given track - 2D calc
   <h3>Notes</h3> 
   <ol>
   <li>No implementation.</li>
   <li>Returns 0</li>
   </ol>
*/
double  StiKalmanTrack::getDca2(StiTrack *t)   const
{
    return 0;
}

/*! Calculate and return the distance of closest approach to given track - 3D calc
   <h3>Notes</h3> 
   <ol>
   <li>No implementation.</li>
   <li>Returns 0</li>
   </ol>
*/
double  StiKalmanTrack::getDca3(StiTrack *t)   const
{
    return 0;
}

/*! 
	Calculate and return the number of hits on this track. 
   <h3>Notes</h3> 
   <ol>
   <li>Iterate through all nodes of this track.</li>
   <li>Count number of hits.</li>
   </ol>
	 \return number of hits.
*/
int StiKalmanTrack::getPointCount() const
{
	int nPts = 0;
	if (firstNode)
		{
		  StiKTNBidirectionalIterator it;
		  for (it=begin();it!=end();it++)
		    {
		      if ((*it).getHit())
						nPts++;
		    }
		}
	return nPts;
}

/*! Calculate and return the maximum possible number of hits on this track. 
  <h3>Notes</h3> 
   <ol>
   <li>Iterate through all nodes of this track.</li>
   <li>Count active layers.</li>
   <li>Use the (y,z) position of the node to determine whether point is on
       active region of the detector i.e. RDO were functional.</li>
   </ol>
	 \return maximum number of points
*/
int StiKalmanTrack::getMaxPointCount() const
{
  int nPts = 0;
  if (firstNode)
    {
      StiKTNBidirectionalIterator it;
      for (it=begin();it!=end();it++)
				{
					const StiDetector * detector = (*it).getDetector();
					if (detector)
						{
							if (detector->isActive((*it).fP0,(*it).fP1))
								nPts++;
						}
					else
						nPts++; // vertex have no detector...
				}
    }
  return nPts;
}


/*! Return the number of gaps (active layers with no hits) along this track.
  <h3>Notes</h3> 
   <ol>
   <li>A gap consists of one or multiple contiguous active layers through which this track
       passes.</li>
   <li>There can be gaps on the inside or the outside of the track if no hits are found there.</li>
   </ol>
	 \returns number of gaps.
*/
int    StiKalmanTrack::getGapCount()    const  
{
  int gaps = 0;
  if (firstNode)
    {
      StiKTNBidirectionalIterator it;
      bool inGap = false;
      for (it=begin();it!=end();it++)
				{
					const StiDetector * detector = (*it).getDetector();
					if (detector && detector->isActive())
						{
							if ((*it).getHit())
								{
									if (inGap) 
										inGap = false;
								}
							else
								{
									if (!inGap)
										{
											inGap = true;
											gaps++;
										}										
								}
						}
				}
    }
  return gaps;
}

/*! Return the number of hits (points) used in the fit of this track.
  <h3>Notes</h3> 
   <ol>
   <li>Currently no difference is made between points on the track and fit points 
       on the track.</li>
   <li>Call "getPointCount()" to get the count.</li>
   </ol>
	 \return number of hits on this track.
*/
int StiKalmanTrack::getFitPointCount()    const  
{
	return getPointCount();
}

/*! Convenience method used to return a track node iterator initialized to the track first node.
	\return Bidirectional Itertator of KalmanTrackNodes 
	\throws runtime_error 	
*/
StiKTNBidirectionalIterator StiKalmanTrack::begin() const 
{
	if (!firstNode)
		throw runtime_error("StiKalmanTrack::begin() - ERROR - firstNode==0");
  return StiKTNBidirectionalIterator(firstNode);
}

/*! Convenience method used to return a track node iterator initialized to the track last node.
	\return Bidirectional Itertator of KalmanTrackNodes 
	\throws runtime_error 	
*/
StiKTNBidirectionalIterator StiKalmanTrack::end() const 
{
	if (!firstNode)
		throw runtime_error("StiKalmanTrack::end() - ERROR - firstNode==0");
	return StiKTNBidirectionalIterator(lastNode);
}

/*! Calculate and return the track length.
  <h3>Note</h3> 
   <ol>
   <li>Using helix track model in global reference frame.</li>
   <li>Using only inner most and outer most hits associated with this track.</li>
   </ol>
	 \return tracklength
	 \throws runtime_error
*/
double StiKalmanTrack::getTrackLength() const
{
  double length = 0;
	//double x1,y1,z1,a1,c1,t1,e1,x2,y2,z2,a2,c2,t2,e2;
	//double dx,dy,dz,cos1,cos2,xp,yp,cda,sda,da,d;
	//double x0,y0;

	StPhysicalHelix helix(StThreeVector<double>(0,0,0),StThreeVector<double>(0,0,0), 0, 0);
	(*StiGeometryTransform::instance())(getInnerMostHitNode(), &helix);
	const StThreeVectorF &p = getOuterMostHitNode()->getHit()->globalPosition();
	StThreeVector<double> point(p.x(), p.y(), p.z());
	length = helix.pathLength(point);
	return length;
}
	/*
		StiKTNBidirectionalIterator it;
		x1=(*first).fX; y1=(*first).fP0; z1=(*first).fP1; 
		e1=(*first).fP2; a1=(*first).fAlpha; c1=(*first).fP3; t1=(*first).fP4;
		cos1=c1*x1-e1;
		x2=(*last).fX; y2=(*last).fP0; z2=(*last).fP1; 
		e2=(*last).fP2; a2=(*last).fAlpha; c2=(*last).fP3; t2=(*last).fP4;
		if (a2!=a1)
		{//rotate (x0,y0) first as we need x2,y2 in the original frame
		if (a1<0) a1+=2*M_PI;
		if (a2<0) a2+=2*M_PI;
		da=a1-a2; cda=cos(da); sda=sin(da);
		x0=e2/c2; //center of circle
		d=c2*x2-e2;
		y0=y2+sqrt(1-d*d)/c2;
		cout << "(x0,y0)="<<x0<<"\t"<<y0<<endl;
		xp=cda*x0-sda*y0;
		e2=c2*xp;
		// now rotate (x2,y2)
		cout << "(x2,y2)="<<x2<<"\t"<<y2<<endl;
		xp=cda*x2-sda*y2;
		yp=sda*x2+cda*y2;
		x2=xp;y2=yp;
		cout << "(x2',y2')="<<x2<<"\t"<<y2<<endl;
		}
		cos2=c2*x2-e2;
		if (c1<1e-12 || (fabs(cos1)>1.) || (fabs(cos2)>1.) )
		{	// straight track case
		cout <<"SL:";
		dx = x2-x1;	dy = y2-y1;	dz = z2-z1;
		length += sqrt(dx*dx+dy*dy+dz*dz);
		}
		else
		{	// helix case
		cout <<"H:";
		length += fabs(acos(cos1)-acos(cos2))*sqrt(1+t1*t1)/c1;
		}
		
		//ignore this patch of code...
		x1=x2;y1=y2;z1=z2;e1=e2;c1=c2;t1=t2,cos1=cos2; //a does not change...
		it = first;
		x1=(*it).fX; y1=(*it).fP0; z1=(*it).fP1; e1=(*it).fP2; a1=(*it).fAlpha; c1=(*it).fP3; t1=(*it).fP4;
		cos1=c1*x1-e1;
		while (it!=last)
		{
		cout << " a1:"<<a1<<" c1:"<<c1<<" cos1:"<<cos1;
		it++;
		x2=(*it).fX; y2=(*it).fP0; z2=(*it).fP1; e2=(*it).fP2; a2=(*it).fAlpha; c2=(*it).fP3; t2=(*it).fP4;
		if (a2!=a1)
		{//rotate (x0,y0) first as we need x2,y2 in the original frame
		da=a2-a1; cda=cos(da); sda=sin(da);
		cout << " rotby:" << da*180/3.1415;
		x0=e2/c2; //center of circle
		d=c2*x2-e2;
		y0=y2+sqrt(1-d*d)/c2;
		xp=cda*x0-sda*y0;
		// now rotate (x2,y2)
		e2=c2*xp;
		xp=cda*x2-sda*y2;
		yp=sda*x2+cda*y2;
		x2=xp;y2=yp;
		}
		cos2=c2*x2-e2;
		if (c1<1e-12 || (fabs(cos1)>1.) || (fabs(cos2)>1.) )
		{	// straight track case
		dx = x2-x1;	dy = y2-y1;	dz = z2-z1;
		length += sqrt(dx*dx+dy*dy+dz*dz);
		}
		else
		{	// helix case
		length += fabs(acos(cos1)-acos(cos2))*sqrt(1+t1*t1)/c1;
		}
		x1=x2;y1=y2;z1=z2;e1=e2;c1=c2;t1=t2,cos1=cos2; //a does not change...
		cout << "\n : " << length<<endl;
		}
	*/

/*! Accessor method to get the dca.
  <h3>Note</h3> 
	<ol>
	<li>Not implemented</li>
	</ol>
*/
double StiKalmanTrack::getPrimaryDca() const
{
  return 0;
}


/*! Accessor method returns the outer most node associated with the track.
   <h3>Notes</h3>
   <ol>
   <li>Node returned depends on the direction of tracking. </li>
   <li>Return firstNode if tracking was done outside-in, lastNode otherwise.</li>
   <li>No check done to determine whether returned value is non null.</li>
   </ol>
	 \return outer most node on this track
*/
StiKalmanTrackNode * StiKalmanTrack::getOuterMostNode()  const 
{ 
  if (trackingDirection==kOutsideIn)
    return firstNode;
  else
    return lastNode;
}

/*! Accessor method returns the inner most node associated with the track.
   <h3>Notes</h3>
   <ol>
   <li>Node returned depends on the direction of tracking. </li>
   <li>Return firstNode if tracking was done inside-out, lastNode otherwise.</li>
   <li>No check done to determine whether returned value is non null.</li>
   </ol>
	 \return outer most node on this track
*/
StiKalmanTrackNode * StiKalmanTrack::getInnerMostNode()   const 
{ 
  if (trackingDirection==kInsideOut)
    return firstNode;
  else
    return lastNode;
}

/*! Return the inner most hit associated with this track.
   <h3>Notes</h3>
   <ol>
   <li>Throws logic_error exception if firstNode or lastNode are not defined, or if track has no hit.</li>
   <li>Loop through all nodes from end() to begin() (or vice versa if tracking 
       direction is outside-in) and search for node with hit. Return first hit found.</li>
   </ol>
	 \return inner most hit node on this track
	 \throws logic_error
*/
StiKalmanTrackNode * StiKalmanTrack::getOuterMostHitNode()  const
{
  if (firstNode==0 || lastNode==0)
    throw logic_error("StiKalmanTrack::getOuterMostHitNode() - ERROR - firstNode||lastNode==0");
	StiKTNBidirectionalIterator it;
	
  if (trackingDirection==kOutsideIn)
    {
			for (it=begin();it!=end();it++)
				{
					if ((*it).getHit())
						return &*it;
				}
		}
	else
		{	
			for (it=end();it!=begin();it--)
				{
					if ((*it).getHit())
						return &*it;
				}
    }
  throw logic_error("StiKalmanTrack::getOuterMostHitNode() - ERROR - Track has no hit");
}


/*! Return the inner most hit associated with this track.
   <h3>Notes</h3>
   <ol>
   <li>Throws logic_error exception if firstNode or lastNode are not defined, or if track has no hit.</li>
   <li>Loop through all nodes from begin() to end() (or vice versa if tracking 
       direction is outside-in) and search for node with hit. Return first hit found.</li>
   </ol>
	 \return outer most hit node on this track
*/
StiKalmanTrackNode * StiKalmanTrack::getInnerMostHitNode()   const
{
  if (firstNode==0 || lastNode==0)
    throw logic_error("StiKalmanTrack::getInnerMostHitNode() - ERROR - firstNode||lastNode==0");
  StiKTNBidirectionalIterator it;
  
  if (trackingDirection==kInsideOut)
    {
      for (it=begin();it!=end();it++)
	{
	  if ((*it).getHit())
	    return &*it;
	}
    }
  else
    {	
      for (it=end();it!=begin();it--)
	{
	  if ((*it).getHit())
	    return &*it;
	}
    }
  throw logic_error("StiKalmanTrack::getInnerMostHitNode() - ERROR - Track has no hit");
}

/*! Return true if inner most hit associated with this track is main vertex.
   <h3>Algorithm</h3>
   <ol>
   <li>Find the inner most hit node associated with this tracks.</li>
   <li>Return true if "x" of inner most hit is less than 2 cm.
   </ol>
	 \return true if "x" of inner most hit is less than 2 cm.
*/
bool  StiKalmanTrack::isPrimary() const
{
	StiKalmanTrackNode * node = getInnerMostHitNode();
	return (node->fX<2.)?true:false;
}

/*! Swap the track node sequence inside-out
   <h3>Algorithm</h3>
   <ol>
   <li>Loop through the node sequence starting with the firstNode and invert the parent child relationships.</li>
   <li>Include removal of all children for each node.</li>
   <li>Include change of parent</li>
   <li>Set parent of last node as "0" to complete swap.</li>
   <li>Change the "trackingDirection" flag to reflect the swap.
   </ol>
 */
void StiKalmanTrack::swap()
{
	StiKalmanTrackNode * parent = 0;
	StiKalmanTrackNode * child  = 0;
	StiKalmanTrackNode * grandChild = 0;
	
	parent = firstNode;
	firstNode = lastNode;
	lastNode = parent; 
	if (parent && parent->getChildCount()>0)
		{
			child  = dynamic_cast<StiKalmanTrackNode *>(parent->getFirstChild());
			parent->removeAllChildren();			
			while (child)
				{
					if (child->getChildCount()>0)
						{
							grandChild = dynamic_cast<StiKalmanTrackNode *>(child->getFirstChild());
							child->removeAllChildren();
						}
					else
						grandChild = 0;
					child->addChild(parent);
					parent = child;
					child = grandChild;
				}
			// last parent has no parent
			parent->setParent(0);
		}
	if (trackingDirection==kOutsideIn)
		trackingDirection = kInsideOut;
	else
		trackingDirection = kOutsideIn;
}

///return hits;
//vector<StHit*> StiKalmanTrack::stHits() const
vector<StMeasuredPoint*> StiKalmanTrack::stHits() const
{
	StiKalmanTrackNode* leaf = getLastNode();
	StiKTNForwardIterator it(leaf);
	StiKTNForwardIterator end = it.end();
	//vector<StHit*> hits;
	vector<StMeasuredPoint*> hits;
	while (it!=end) {
		const StiKalmanTrackNode& node = *it;
		StiHit* hit = node.getHit();
		if (hit) {
			StMeasuredPoint * stHit = const_cast<StMeasuredPoint*>( hit->stHit() );
			if (stHit)
				hits.push_back(stHit);
		}
		++it;
	}
	return hits;
}

