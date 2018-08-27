#ifndef __CONTROLPARAMETERS_H
#define __CONTROLPARAMETERS_H

#include "Riostream.h"
#include "TObject.h"
#include "TString.h"
#include <stdint.h>

#include "colors.h"

class controlParameters : public TObject {

 public:
  Bool_t smearPosition, highEdifferent, eDependent;
  Bool_t useRealSegments;
  Float_t sigmaX[2], sigmaY[2], sigmaZ[2];
  Float_t Ax, Ay, Az, Bx, By, Bz;

  Bool_t ccResolution;
  TString ccResFile;
  
  Bool_t segResolution;
  TString segResFile;
  
  Bool_t combinePoints, sameSegOnly;
  Float_t coalescenceD;

  Bool_t moveLowE;
  Int_t minSegMult;
  Float_t maxE2Move;

  Float_t ASeg, BSeg;  

 public:
  controlParameters();
  ~controlParameters() { ; }
  void reset();
  Int_t getSettings(TString fileName);

 private:
  ClassDef(controlParameters, 1);

};


#endif
