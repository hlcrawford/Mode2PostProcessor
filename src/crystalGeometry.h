#ifndef __CRYSTALGEOMETRY_H
#define __CRYSTALGEOMETRY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <signal.h>

#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoPcon.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"

#include "TFile.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TMath.h"
#include "TVector3.h"
#include "TRandom3.h"

#include "colors.h"

/*! Class containing details of crystal geometry, including physical 
  crystal boundaries, basis / segmentation details and functions to 
  test position of interaction points and modify based on geometry */

class CrystalGeometry : public TObject {
 public:
  Int_t xtalType;
  TGeoCompositeShape *crystal;
  TGeoVolume *compCrystal;
  Float_t zBound[50][50][6];
  Float_t phiBound[6];

  TH1F *zDist;
  TH2F *xyHigh[6];
  TH1F *phiDist[36];
  TH3F *xyzDist[36];
  TH2F *rzDist[36];
  //TH3F *xyz;
  
 public:
  CrystalGeometry();
  ~CrystalGeometry();
  //! Function to generate crystal geometry, for 'A' or 'B' type crystals
  /*! \param type An integer argument -- 0 for 'B' crystal geometry or 1 for 'A' crystal geometry
      \return No return -- function populates class members geoManager, top, crystal and compCrystal 
  */
  void BuildCrystal(Int_t type, TGeoVolume *top);
  //! Function to test if point (xx, yy, zz) is inside crystal physical volume
  /*! \param xx double value of x coordinate of point being tested
      \param yy double value of y coordinate of point being tested
      \param zz double value of z coordinate of point being tested
      \return Returns boolean -- 1 if point is inside physical volume, 0 otherwise
  */
  Bool_t IsInside(Double_t xx, Double_t yy, Double_t zz);
  //! Function to return distance of a point (xx, yy, zz) from outer crystal boundary
  /*! \param xx double value of x coordinate of point being tested
      \param yy double value of y coordinate of point being tested
      \param zz double value of z coordinate of point being tested
      \return Returns double value corresponding to the distance to the closest point on the surface of the crystal volume
  */
  Double_t DistanceToEdge(Double_t xx, Double_t yy, Double_t zz);
  //! Function to read in basis points for the given crystal
  /*! \param basisFilename the TString value of the filename containing the basis information
      \return Returns int value
  */
  Int_t GetBasisPoints(TString basisFilename);
  Int_t WriteBasisSpectra(TString fileName);
  TVector3 GetNewInteractionPosition(TRandom3 *gRandom, Int_t segmentNum);
  Int_t GetSegmentNumber(Double_t xx, Double_t yy, Double_t zz);

 private:
  ClassDef(CrystalGeometry,  1);
};

#endif
