//McKenzie Burch
//Rigging Dojo Maya API
//July 2020
//mainPlugin.cpp
//standard setup for event node

#include "stretchik.h"  //change include header for each node

#include <maya/MFnPlugin.h> //Maya class that redisters and deregisters plug-ins with Maya

MStatus initializePlugin(MObject obj){
  MStatus status;
  MFnPlugin fnplugin(obj, "McKenzie Burch", "1.0", "Any");

  //register a new dependency node with Maya
  status = fnplugin.registerNode("stretchIk", stretchIk::typeId, stretchIk::creator, stretchIk::initialize, stretchIk::kDependNode);

  if(status != MS::kSuccess)
    status.perror("Could not regiser the poseReader node");

  return status;
}

MStatus uninitializePlugin(MObject obj){
  MFnPlugin pluginFn;

  //deregister the given user defined dependency node type Maya
  pluginFn.deregisterNode(MTypeId(0x80004));

  return MS::kSuccess;
}
