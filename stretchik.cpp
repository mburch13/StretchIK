//stretchik.cpp

#include "stretchik.h"

#include <maya/MGlobal.h>  //static class provviding common API global functions
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>  //typed attribute function set
#include <maya/MFnNumericAttribute.h>  //numeric attribute function set
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnCompoundAttribute.h>  //compound attribute function set
#include <maya/MFloatPoint.h>  //implementation of a point
#include <maya/MFnMatrixAttribute.h>  //matrix attribute function set
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <cmath>  //c++ math library


MTypeId stretchIk::typeId(0x80015);  //define value for typeId

//needed attributes
MObject stretchIk::startMatrix;
MObject stretchIk::endMatrix;
MObject stretchIk::upInitLength;
MObject stretchIk::downInitLength;
MObject stretchIk::upScale;
MObject stretchIk::downScale;
MObject stretchIk::stretch;
MObject stretchIk::slide;
MObject stretchIk::globalScale;
MObject stretchIk::poleVectorMatrix;
MObject stretchIk::poleVecLock;

stretchIk::stretchIk(){

}

void* stretchIk::creator(){
  return new stretchIk();
}

MStatus stretchIk::initialize(){
  MFnNumericAttribute numFn;
  MFnMatrixAttribute matrixFn;
  MStatus stat;

  //matricies
  startMatrix = matrixFn.create("startMatrix", "stm");  //holds world matrix of the start reference node of the ik (transform matching position of ik root)
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating stretchIk start matrix");  //report error if fails
  matrixFn.setKeyable(true);
  matrixFn.setStorable(true);
  addAttribute(startMatrix);

  endMatrix = matrixFn.create("endMatrix", "enm"); //holds world matrix of the end reference node of the ik (transform matching position of last chain bone
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating stretchIk end matrix");  //report error if fails
  matrixFn.setKeyable(true);
  matrixFn.setStorable(true);
  addAttribute(endMatrix);

  poleVectorMatrix = matrixFn.create("poleVectorMatrix", "pvm");
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating stretchIk pole vector matrix");  //report error if fails
  matrixFn.setKeyable(true);
  matrixFn.setStorable(true);
  addAttribute(poleVectorMatrix);

  upInitLength = numFn.create("upInitLength", "uil", MFnNumericData::kDouble, 0, &stat); //initial length of the second bone
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating stretchIk up initial length value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(upInitLength);

  downInitLength = numFn.create("downInitLength", "dil", MFnNumericData::kDouble, 0, &stat);  //initial length of the first bone
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating stretchIk down initial length curve");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(downInitLength);

  globalScale = numFn.create("globalScale", "gls", MFnNumericData::kDouble, 0, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk global scale value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  numFn.setMin(0.001);
  addAttribute(globalScale);

  poleVecLock = numFn.create("poleVecLock", "pvl", MFnNumericData::kDouble, 0, &stat); //attribute to lock the second bode to the pole vecotr
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk pole vector lock");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  numFn.setMin(0.0);
  numFn.setMax(1.0);
  addAttribute(poleVecLock);

  stretch = numFn.create("stretch", "str", MFnNumericData::kDouble, 0, &stat); //attribute to control the blending of the strech variables
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk stretch value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  numFn.setMin(0.0);
  numFn.setMax(1.0);
  addAttribute(stretch);

  slide = numFn.create("slide", "sld", MFnNumericData::kDouble, 0, &stat);  //attribute that contorls the slide amound for the second bone
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk slide value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  numFn.setMin(-1.0);
  numFn.setMax(1.0);
  addAttribute(slide);

  upScale = numFn.create("upScale", "ups", MFnNumericData::kDouble, 1, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk up scale value");  //report error if fails
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(upScale);

  downScale = numFn.create("downScale", "dws", MFnNumericData::kDouble, 1, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating stretchIk down scale value");  //report error if fails
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(downScale);


  //Maya will see when input is changed will force the node to evaluate the new output
  attributeAffects(startMatrix, upScale);
  attributeAffects(endMatrix, upScale);
  attributeAffects(stretch, upScale);
  attributeAffects(slide, upScale);
  attributeAffects(poleVecLock, upScale);
  attributeAffects(poleVectorMatrix, upScale);
  attributeAffects(upInitLength, upScale);

  attributeAffects(startMatrix, downScale);
  attributeAffects(endMatrix, downScale);
  attributeAffects(stretch, downScale);
  attributeAffects(slide, downScale);
  attributeAffects(poleVecLock, downScale);
  attributeAffects(poleVectorMatrix, downScale);
  attributeAffects(globalScale, downScale);
  attributeAffects(downInitLength, downScale);

  return MS::kSuccess;
}

MStatus stretchIk::compute(const MPlug& plug, MDataBlock& dataBlock){
  //trigger only when needed
  if((plug == downScale) || (plug == upScale)){

    //get input matricies
    MMatrix startMatrixV = dataBlock.inputValue(startMatrix).asMatrix();
    MMatrix endMatrixV = dataBlock.inputValue(endMatrix).asMatrix();
    MMatrix poleVectorMatrixV = dataBlock.inputValue(poleVectorMatrix).asMatrix();

    //get initail length
    double upInitLengthV = dataBlock.inputValue(upInitLength).asDouble();
    double downInitLengthV = dataBlock.inputValue(downInitLength).asDouble();

    //get parameter input
    double stretchV = dataBlock.inputValue(stretch).asDouble();
    double slideV = dataBlock.inputValue(slide).asDouble();
    double lockV = dataBlock.inputValue(poleVecLock).asDouble();
    double globalScaleV = dataBlock.inputValue(globalScale).asDouble();

    //compute total chain length (original)
    double chainLength = upInitLengthV + downInitLengthV;

    //compute the bone vectors
    MVector startVec (startMatrixV[3][0], startMatrixV[3][1], startMatrixV[3][2]);
    MVector endVec (endMatrixV[3][0], endMatrixV[3][1], endMatrixV[3][2]);
    MVector currentLengthVec = endVec - startVec;

    //compute length
    chainLength*=globalScaleV ;
    double currentLength = currentLengthVec.length();

    //create output variables
    double upScaleOut = upInitLengthV;
    double downScaleOut = downInitLengthV;

    //compute stretch
    double delta, ratio;
    if (stretchV > 0.001){
      delta = currentLength/chainLength;
      if(delta > 1){
        delta = ((delta-1)*stretchV)+1;
      }
      else{
        delta = 1;
      }

      upScaleOut*=delta;
      downScaleOut*=delta;
    }

    //compute slide
    if(slideV >= 0){
      ratio = chainLength/(upInitLengthV * globalScaleV);
      delta = (ratio - 1)*slideV;
      upScaleOut *= (delta+1);
      downScaleOut *= (1-slideV);
    }
    else{
      ratio = chainLength/(downInitLengthV * globalScaleV);
      delta = (ratio - 1)* -slideV;
      downScaleOut *= (delta+1);
      upScaleOut *= (1+slideV);
    }

    //compute pole vector lock
    if(lockV > 0.001){
      MVector polePos (poleVectorMatrixV[3][0], poleVectorMatrixV[3][1], poleVectorMatrixV[3][2]);

      //compute the length of the vector needed to snap
      MVector startPole = polePos - startVec;
      MVector poleEnd = endVec - polePos;
      double startPoleLen = startPole.length()/globalScaleV;
      double poleEndLen = poleEnd.length()/globalScaleV;

      //linear interpolate to two values
      upScaleOut = (upScaleOut * (1-lockV)) + (startPoleLen*lockV);
      downScaleOut = (downScaleOut * (1-lockV)) + (poleEndLen*lockV);
    }
    //set output values
    dataBlock.outputValue(downScale).set(downScaleOut);
    dataBlock.outputValue(downScale).setClean();  //clean mease has been evaluated correctly

    dataBlock.outputValue(upScale).set(upScaleOut);
    dataBlock.outputValue(upScale).setClean();  //clean mease has been evaluated correctly
  }

  return MS::kSuccess;
}
