//StiDisplayManager.h
//M.L. Miller (Yale Software)
//04/01

//This  class manages the Sti display

#ifndef StiDisplayManager_HH
#define StiDisplayManager_HH

#include <string>
#include <map>

class TCanvas;
class TShape;
class TNode;
class StiDrawable;

class StiDisplayManager
{
public:

    typedef map<string, StiDrawable*> stidrawablemap;
    typedef stidrawablemap::value_type stiDrawableMapValType;
    
    virtual ~StiDisplayManager();

    //Singleton access
    static StiDisplayManager* instance();
    static void kill();

    //Action
    void addDrawable(StiDrawable*);
    
    void cd();
    void update();
    void draw();

    void setInvisible(); //All
    void setInvisible(const StiDrawable*);
    void setVisible(); //All
    void setVisible(const StiDrawable*);

    //Utility
    void print() const;
    
private:

    static StiDisplayManager* sinstance;

    //dx, dy, and dz define the volume within which objects are drawn.  The volume is a rectangle of lengths 2*dx, 2*dy, 2*dz
    enum StiCanvasSize {kXmin=200, kXmax=800, kYmin=100, kYmax=700};
    enum StiMainVolumeSize {kdx=200, kdy=200, kdz=240};
    
    StiDisplayManager();
    void setup();

    stidrawablemap mmap;
    
    TCanvas* mcanvas;
    TShape* mzone;
    TNode* mnode;

};

#endif
