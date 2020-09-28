//stretchik.h

#ifndef stretchik_H
#define stretchik_H

#include <maya/MTypeId.h>  //Manage Maya Object type identifiers
#include <maya/MPxNode.h>  //Base class for user defined dependency nodes

class stretchIk : public MPxNode{
  public:
    stretchIk();
    static MStatus initialize();  //initialize node
    static void* creator();  //create node
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);  //implements core of the node

  public:
    //needed variables
    static MTypeId typeId;
    static MObject startMatrix;
    static MObject endMatrix;
    static MObject upInitLength;
    static MObject downInitLength;
    static MObject upScale;
    static MObject downScale;
    static MObject stretch;
    static MObject slide;
    static MObject globalScale;
    static MObject poleVectorMatrix;
    static MObject poleVecLock;

};

#endif
