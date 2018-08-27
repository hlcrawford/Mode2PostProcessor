#include "Globals.h"
#include "crystalGeometry.h"

/* Notes on crystal type numbering:

     In the real world, all quads have B-A-B-A crystal types from
     position 0 to 3.  

     In GEANT-land, this corresponds to all quads having what are
     known as 1-0-1-0 type crystals from positions 0 to 3. 

     Therefore, by definition, type A = 0 and type B = 1.          */

CrystalGeometry::CrystalGeometry() { ; }

/*******************************************************/

CrystalGeometry::~CrystalGeometry() { ; }

/*******************************************************/

void CrystalGeometry::BuildCrystal(Int_t type, TGeoVolume *top) {

  xtalType = type;

  TGeoMaterial *matGermanium = new TGeoMaterial("Ge", 72.64, 32, 5.323);
  TGeoMedium *Ge = new TGeoMedium("Ge", 2, matGermanium);
  
  if (type == 1) { /* Crystal type A */
    TGeoArb8 *halfAA = new TGeoArb8("halfAA", 45.0);
    halfAA->SetVertex(0, -26.601460, 14.309190);
    halfAA->SetVertex(1, 0.873900, 33.434270);
    halfAA->SetVertex(2, 29.035060, 18.806490);
    halfAA->SetVertex(3, 29.035060, -16.720150);
    halfAA->SetVertex(4, -41.812290, 22.456740);
    halfAA->SetVertex(5, -0.370500, 51.303560);
    halfAA->SetVertex(6, 43.813450, 28.353050);
    halfAA->SetVertex(7, 43.813450, -27.057040);
    TGeoArb8 *halfAB = new TGeoArb8("halfAB", 45.0);
    halfAB->SetVertex(0, -26.601460, 14.309190);
    halfAB->SetVertex(1, 29.035060, -16.720150);
    halfAB->SetVertex(2, -2.106240, -31.359840);
    halfAB->SetVertex(3, -26.601460, -14.309190);
    halfAB->SetVertex(4, -41.812290, 22.456740);
    halfAB->SetVertex(5, 43.813450, -27.057040);
    halfAB->SetVertex(6, -3.350650, -49.229140);
    halfAB->SetVertex(7, -41.812290, -22.456740);
  
    TGeoTranslation *t1A = new TGeoTranslation("t1A", 0, 0, +45.);
    t1A->RegisterYourself();
    TGeoRotation *r1A = new TGeoRotation("r1A", -31.79, 0, 0);
    r1A->RegisterYourself();
    TGeoCombiTrans *c1A = new TGeoCombiTrans("c1A", 0, 0, +45., r1A);
    c1A->RegisterYourself();
    
    TGeoPcon *capsuleA = new TGeoPcon("capsuleA", 0., 360., 4);
    capsuleA->DefineSection(0, 0., 0., 40.);
    capsuleA->DefineSection(1, 15., 0., 40.);
    capsuleA->DefineSection(2, 15., 5., 40.);
    capsuleA->DefineSection(3, 90., 5., 40.);
    
    crystal = new TGeoCompositeShape("crystalA",
				     "((halfAA:c1A+halfAB:c1A)*capsuleA:r1A)");
    compCrystal = new TGeoVolume("compCrystalA", crystal, Ge);
    compCrystal->SetLineColor(kRed);
    top->AddNode(compCrystal, 1);

  } else if (type == 0) {   
    TGeoArb8 *halfBA = new TGeoArb8("halfBA", 45.0);
    halfBA->SetVertex(0, -29.035060, 18.819560);
    halfBA->SetVertex(1, 0.326440, 34.622460);
    halfBA->SetVertex(2, 27.760980, 18.624430);
    halfBA->SetVertex(3, 27.246880, -16.340780);
    halfBA->SetVertex(4, -43.813450, 28.366120);
    halfBA->SetVertex(5, 0.497910, 52.215310);
    halfBA->SetVertex(6, 43.529680, 27.121990);
    halfBA->SetVertex(7, 42.755280, -25.546310);
    TGeoArb8 *halfBB = new TGeoArb8("halfBB", 45.0);
    halfBB->SetVertex(0, -29.035060, 18.819560);
    halfBB->SetVertex(1, 27.246880, -16.340780);
    halfBB->SetVertex(2, 1.233470, -33.020040);
    halfBB->SetVertex(3, -29.035060, -16.661720);
    halfBB->SetVertex(4, -43.813450, 28.366120);
    halfBB->SetVertex(5, 42.755280, -25.546310);
    halfBB->SetVertex(6, 1.932060, -51.721310);
    halfBB->SetVertex(7, -43.813450, -26.998620);
    
    TGeoTranslation *t1B = new TGeoTranslation("t1B", 0., 0, +45.);
    t1B->RegisterYourself();
    TGeoTranslation *t2B = new TGeoTranslation("t2B", 0., 0, 0);
    t2B->RegisterYourself();
    TGeoRotation *r1B = new TGeoRotation("r1B", -118.25, 0, 0);
    r1B->RegisterYourself();
    TGeoCombiTrans *c1B = new TGeoCombiTrans("c1B", 0., 0, +45., r1B);
    c1B->RegisterYourself();
    TGeoCombiTrans *c2B = new TGeoCombiTrans("c2B", 0., 0, 0, r1B);
    c2B->RegisterYourself();
    
    TGeoPcon *capsuleB = new TGeoPcon("capsuleB", 0., 360., 4);
    capsuleB->DefineSection(0, 0., 0., 40.);
    capsuleB->DefineSection(1, 15., 0., 40.);
    capsuleB->DefineSection(2, 15., 5., 40.);
    capsuleB->DefineSection(3, 90., 5., 40.);
    
    crystal = new TGeoCompositeShape("crystalB",
				     "(halfBA:c1B+halfBB:c1B)*capsuleB:c2B");
    compCrystal = new TGeoVolume("compCrystalB", crystal, Ge);
    compCrystal->SetLineColor(kBlue);
    top->AddNode(compCrystal, 1);
  }
}

/*******************************************************/

Bool_t CrystalGeometry::IsInside(Double_t xx, Double_t yy, Double_t zz) {
  Double_t point[3];
  point[0] = xx; point[1] = yy; point[2] = zz;
  return crystal->Contains(point);
}

/*******************************************************/

Double_t CrystalGeometry::DistanceToEdge(Double_t xx, Double_t yy, Double_t zz) {
  Double_t point[3];
  point[0] = xx; point[1] = yy; point[2] = zz;
  return crystal->Safety(point, IsInside(xx, yy, zz));
}

/*******************************************************/

Int_t CrystalGeometry::GetBasisPoints(TString basisFilename) {
  
  Bool_t DEBUG = 0;

  char header[256];
  int siz = 0;
  int i, j, k, m;

  FILE *basisFile;
  basis_point *basis;
  if (!(basis = (basis_point*)malloc(sizeof(basis_point)*GRID_PTS))) {
    std::cerr << "ERROR: Could not malloc basis!" << std::endl;
    exit(-1);
  }
  if (!(basisFile = fopen(basisFilename.Data(), "r"))) {
    std::cout << ALERTTEXT;
    printf("GetBasisPoints: ERROR: Could not open basis file \"%s\".", 
	   basisFilename.Data());
    std::cout << RESET_COLOR;
    exit(-1);
  } else {
    printf("Reading basis file \"%s\".", basisFilename.Data());
  }
  fread(header, 256, 1, basisFile);
  
  siz = fread(basis, sizeof(basis_point), GRID_PTS, basisFile);
  if (siz != GRID_PTS) { 
    std::cerr << "ERROR: Couldn't read basis information properly." << std::endl;
    fclose(basisFile);
    return 1;
  }
  fclose(basisFile);
  
  Float_t basisX[GRID_PTS] = {0}; Float_t basisY[GRID_PTS] = {0};
  Float_t basisZ[GRID_PTS] = {0};
  Int_t basisSeg[GRID_PTS] = {0};
  Float_t basisR[GRID_PTS] = {0};
  Float_t basisPhi[GRID_PTS] = {0};
  
  Int_t numOutside = 0;
  Int_t totalNum = 0;
  Float_t distAverage = 0;

  for (i=0; i<GRID_PTS; i++) {
    basisX[i] = basis[i].x;
    basisY[i] = basis[i].y;
    basisZ[i] = basis[i].z;
    basisSeg[i] = basis[i].iseg;
    basisR[i] = TMath::Sqrt(basis[i].x*basis[i].x + basis[i].y*basis[i].y);
    basisPhi[i] = TMath::ATan2(basis[i].y,basis[i].x);
    
    if (!IsInside(basis[i].x, basis[i].y, basis[i].z) ){ 
      if (DEBUG) {
	std::cout << ALERTTEXT;
	printf("Basis %d: (%0.2f, %0.2f, %0.2f): %d\n", xtalType, basis[i].x,
	       basis[i].y ,basis[i].z, IsInside(basis[i].x, basis[i].y, basis[i].z));
	printf("                       Distance: %0.2f\n",
	       DistanceToEdge(basis[i].x, basis[i].y, basis[i].z));
	std::cout << RESET_COLOR;  fflush(stdout);
      }
      numOutside++;
      distAverage += DistanceToEdge(basis[i].x, basis[i].y, basis[i].z);
    }
    totalNum++;
  }

  printf(" --> Basis points in volume? %d / %d = %0.2f %%\n", (totalNum - numOutside), 
	 totalNum, 100.*(Float_t)(totalNum - numOutside)/(Float_t)(totalNum));
  printf("     Average distance from volume surface: %0.2f\n", 
	 distAverage/(Float_t)(numOutside));

  char str[300];    
  sprintf(str, "zDist_%d", xtalType);
  zDist = new TH1F(str, str, 1000,0,100);
  sprintf(str, "xyz_%d", xtalType);
  //xyz = new TH3F(str, str, 100, -50, 50, 100, -50, 50, 100, 0, 100);
  
  Float_t basisXseg[GRID_PTS] = {0};
  Float_t basisYseg[GRID_PTS] = {0};
  Float_t basisZseg[GRID_PTS] = {0};
  Float_t basisRseg[GRID_PTS] = {0};
  Float_t basisPhiseg[GRID_PTS] = {0};

  for (i=0; i<50; i++) {
    for (j=0; j<50; j++) {
      for (k=0; k<6; k++) {
  	zBound[i][j][k] = -1.;
      }
    }
  }
 

  for (i=0; i<36; i++) {
    m = 0;
    for (j=0; j<GRID_PTS; j++) {
      if ((int)(basis[j].iseg) == i) {
	basisXseg[m] = basisX[j];
	basisYseg[m] = basisY[j];
	basisZseg[m] = basisZ[j];
	basisRseg[m] = basisR[j];
	basisPhiseg[m] = basisPhi[j];
	if (basisZ[j] > zBound[((int)(basisX[j]+50))/2][((int)(basisY[j]+50))/2][(int)(i/6)]) {
	  zBound[((int)(basisX[j]+50))/2][((int)(basisY[j]+50))/2][(int)(i/6)] = basisZ[j];
	}
	zDist->Fill(basisZ[j]);
	// xyz->Fill(basisX[j], basisY[j], basisZ[j]);
	m++;
      }
    }
    
    char str[300], str2[300];    
    sprintf(str, "phiDist%d_%d", i, xtalType);
    phiDist[i] = new TH1F(str, str, 800,-4,4);
    sprintf(str, "xyzDist%d_%d", i, xtalType);
    sprintf(str2, "rzDist%d_%d", i, xtalType);
    if (xtalType == 1 && (i == 9 || i == 15)) {
      xyzDist[i] = new TH3F(str, str, 500, -50, 50, 500, -50, 50, 500, 0, 100);
      rzDist[i] = new TH2F(str2, str2, 500, 0, 100, 500, 0, 50);
    } else {
      xyzDist[i] = new TH3F(str, str, 100, -50, 50, 100, -50, 50, 100, 0, 100);
      rzDist[i] = new TH2F(str2, str2, 100, 0, 100, 100, 0, 50);
    }
    
    for (int j=0; j<m; j++) {
      xyzDist[i]->Fill(basisXseg[j], basisYseg[j], basisZseg[j]);
      rzDist[i]->Fill(basisZseg[j], TMath::Sqrt(basisXseg[j]*basisXseg[j] + basisYseg[j]*basisYseg[j]));
      phiDist[i]->Fill(basisPhiseg[j]);
    }
  }
  
  TH1F *temp;
  int num = 0;
  Double_t crossing = 0;
  /* Figure out the phiBounds, and fill array */
  for (j=0; j<6; j++) {
    for (m=0; m<6; m++) {
      crossing = -100;
      if (j < 5) {
	temp = (TH1F*)phiDist[j+(m*6)];
	temp->Add(phiDist[j+(m*6)+1], -1);
	for (Int_t i=0; i<temp->GetNbinsX(); i++) {
	  if (temp->GetBinContent(i) < 0 && temp->GetBinContent(i+1) > 0) {
	    //printf("%d-%d Crossing at %0.2f...\n", j, j+1, temp->GetBinCenter(i));
	    crossing = temp->GetBinCenter(i);
	  }
	}
      } else if (j==5) {
	temp = (TH1F*)phiDist[j+(m*6)];
	temp->Add(phiDist[0+(m*6)], -1);
	for (Int_t i=0; i<temp->GetNbinsX(); i++) {
	  if (temp->GetBinContent(i) < 0 && temp->GetBinContent(i+1) > 0) {
	    //printf("%d-%d Crossing at %0.2f...\n", j, 0, temp->GetBinCenter(i));
	    crossing = temp->GetBinCenter(i);
	  }
	} 
      }

      if (crossing != -100) {
	if (crossing < 3) { crossing += 2*TMath::Pi(); }
	phiBound[j] += crossing;
	num++;
      }
    }
    phiBound[j] /= num;
    if (phiBound[j] > TMath::Pi()) { phiBound[j] -= 2*TMath::Pi(); }
    num = 0;
  }

  /* Patch the zBound plots -- check neighbours, and if they aren't at a limit, adjust 
     each point so that it doesn't vary more than say 5% from interpolation between 
     neighbours. */
  for(int tries=0; tries<3; tries++) {
    for (i=0; i<50; i++) {
      for (j=0; j<50; j++) {
	for (k=0; k<6; k++) {
	  if ((i-1) >= 0 && (i+1) < 50 && (j-1) >= 0 && (j+1) < 50) {
	    if ((zBound[i-1][j][k] != -1 && zBound[i+1][j][k] != -1) || 
	    	(zBound[i][j-1][k] != -1 && zBound[i][j+1][k] != -1)) {
	      
	      if (zBound[i][j][k] < zBound[i+1][j][k] && 
		  zBound[i][j][k] < zBound[i-1][j][k]) {
		if (TMath::Abs(zBound[i][j][k] - 
			((zBound[i+1][j][k] + zBound[i-1][j][k])/2))/
		    ((zBound[i+1][j][k] + zBound[i-1][j][k])/2) > 0.05) {
		  zBound[i][j][k] = ((zBound[i+1][j][k] + zBound[i-1][j][k])/2);
		}
	      }
	      if (zBound[i][j][k] < zBound[i][j+1][k] && 
		  zBound[i][j][k] < zBound[i][j-1][k]) {
		if (TMath::Abs(zBound[i][j][k] - 
			((zBound[i][j+1][k] + zBound[i][j-1][k])/2))/
		    ((zBound[i][j+1][k] + zBound[i][j-1][k])/2) > 0.05) {
		  zBound[i][j][k] = ((zBound[i][j+1][k] + zBound[i][j-1][k])/2);
		}
	      }
	      
	    } else if ( (i-2) >= 0 && (i+2) < 50 && (j-2) >= 0 && (j+2) < 50) {
	      if (((zBound[i-2][j][k] != -1 && zBound[i+2][j][k] != -1) || 
		   (zBound[i][j-2][k] != -1 && zBound[i][j+2][k] != -1))) {
		
		if (zBound[i][j][k] < zBound[i+2][j][k] && 
		    zBound[i][j][k] < zBound[i-2][j][k]) {
		  if (TMath::Abs(zBound[i][j][k] - 
			  ((zBound[i+2][j][k] + zBound[i-2][j][k])/2))/
		      ((zBound[i+2][j][k] + zBound[i-2][j][k])/2) > 0.05) {
		    zBound[i][j][k] = ((zBound[i+2][j][k] + zBound[i-2][j][k])/2);
		  }
		}
		if (zBound[i][j][k] < zBound[i][j+2][k] && 
		    zBound[i][j][k] < zBound[i][j-2][k]) {
		  if (TMath::Abs(zBound[i][j][k] - 
			  ((zBound[i][j+2][k] + zBound[i][j-2][k])/2))/
		      ((zBound[i][j+2][k] + zBound[i][j-2][k])/2) > 0.05) {
		    zBound[i][j][k] = ((zBound[i][j+2][k] + zBound[i][j-2][k])/2);
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  /* Extend z definitions (as an average) all the way to out to avoid weird gaps... */
  for (int tries=0; tries<100; tries++) {
    for (k=0; k<6; k++) {
      for (i=0; i<50; i++) {
	for (j=0; j<50; j++) {
	  if (zBound[i][j][k] == -1) {
	    if ((zBound[i+1][j][k] != -1 && i+1 < 50) || (zBound[i][j+1][k] != -1 && j+1 < 50) ||
		(zBound[i][j-1][k] != -1 && j-1 > 0) || (zBound[i-1][j][k] != -1 && i-1 > 0)) {
	      Double_t averageValue = 0; Int_t numInAv = 0;
	      
	      if (zBound[i+1][j][k] > 0 && i+1 < 50) { averageValue += zBound[i+1][j][k]; numInAv++; }
	      if (zBound[i-1][j][k] > 0 && i-1 > 0) { averageValue += zBound[i-1][j][k]; numInAv++; }
	      if (zBound[i][j+1][k] > 0 && j+1 < 50) { averageValue += zBound[i][j+1][k]; numInAv++; }
	      if (zBound[i][j-1][k] > 0 && j-1 > 0) { averageValue += zBound[i][j-1][k]; numInAv++; }
	      if (numInAv > 0) {
		averageValue /= numInAv;
		zBound[i][j][k] = averageValue;
		//printf("Setting %d %d %d to %0.2f\n", i, j, k, averageValue);
	      }
	    }
	  }
	}
      }
    }
  }
  
  for (i=0; i<6; i++) {
    char str[300];    
    sprintf(str, "xy%dH_%d", i, xtalType);
    xyHigh[i] = new TH2F(str, str, 50,-50,50,50,-50,50);
    for (int j=0; j<50; j++) {
      for (int k=0; k<50; k++) {
	xyHigh[i]->Fill(((j*2)-49), ((k*2)-49), zBound[j][k][i]);
      }
    }
  }  

  return 0;
  
  temp->Delete();
}

Int_t CrystalGeometry::WriteBasisSpectra(TString fileName) {
  
  TFile *basisOUT = new TFile(fileName.Data(), "RECREATE");
  zDist->Write();
  // xyz->Write();
  crystal->Write();
  compCrystal->Write();
  for (Int_t i=0; i<6; i++) { xyHigh[i]->Write(); }
  for (Int_t i=0; i<36; i++) { phiDist[i]->Write(); xyzDist[i]->Write(); rzDist[i]->Write(); }
  basisOUT->Write();
  basisOUT->Close();
  return 0;
}

TVector3 CrystalGeometry::GetNewInteractionPosition(TRandom3 *gRandom, 
						    Int_t segmentNum) {
  TVector3 xyzNew;
  Double_t x, y, z;
  xyzDist[segmentNum]->GetRandom3(x, y, z);
  xyzNew.SetXYZ(x,y,z);
  return xyzNew;
}
					 

Int_t CrystalGeometry::GetSegmentNumber(Double_t xx, Double_t yy, Double_t zz) {

  Bool_t DEBUG = 0;
  Float_t phi = TMath::ATan2(yy, xx);
  Int_t wedge = -1;  Int_t layer = -1;
  //printf("phiBounds: \n");
  for (Int_t i=0; i<6; i++) {
    //printf("%d-%d: %0.2f  |  ", i, i+1, phiBound[i]);
  }

  //printf("\n Phi Value: %0.3f\n", phi);
  if ((phiBound[2]*phiBound[3]) > 0 && phi <= phiBound[2] && phi > phiBound[3]) {
    wedge = 3;
  } else if ((phiBound[2]*phiBound[3]) < 0 && (phi <= phiBound[2] || phi > phiBound[3])) {
    wedge = 3;
  } else if ((phiBound[1]*phiBound[2]) > 0 && phi <= phiBound[1] && phi > phiBound[2]) {
    wedge = 2;
  } else if ((phiBound[1]*phiBound[2]) < 0 && (phi <= phiBound[1] || phi > phiBound[2])) {
    wedge = 2;
  } else if ((phiBound[0]*phiBound[1]) > 0 && phi <= phiBound[0] && phi > phiBound[1]) {
    wedge = 1;
  } else if ((phiBound[0]*phiBound[1]) < 0 && (phi <= phiBound[0] || phi > phiBound[1])) {
    wedge = 1;
  } else if ((phiBound[5]*phiBound[0]) > 0 && phi <= phiBound[5] && phi > phiBound[0]) {
    wedge = 0;
  } else if ((phiBound[5]*phiBound[0]) < 0 && (phi <= phiBound[5] || phi > phiBound[0])) {
    wedge = 0;
  } else if ((phiBound[5]*phiBound[4]) > 0 && phi <= phiBound[4] && phi > phiBound[5]) {
    wedge = 5;
  } else if ((phiBound[5]*phiBound[4]) < 0 && (phi <= phiBound[4] || phi > phiBound[5])) {
    wedge = 5;
  } else if ((phiBound[3]*phiBound[4]) > 0 && phi <= phiBound[3] && phi > phiBound[4]) {
    wedge = 4;
  } else if ((phiBound[3]*phiBound[4]) < 0 && (phi <= phiBound[3] || phi > phiBound[4])) {
    wedge = 4;
  }

  //printf("Wedge ID: %d\n", wedge);
  
  Int_t xIndex = ((Int_t)(xx + 50))/2;
  Int_t yIndex = ((Int_t)(yy + 50))/2;

  if (zz <= zBound[xIndex][yIndex][0]) {
    layer = 0;
  } else if (zz <= zBound[xIndex][yIndex][1] &&
	     zz > zBound[xIndex][yIndex][0]) {
    layer = 1;
  } else if (zz <= zBound[xIndex][yIndex][2] &&
	     zz > zBound[xIndex][yIndex][1]) {
    layer = 2;
  } else if (zz <= zBound[xIndex][yIndex][3] &&
	     zz > zBound[xIndex][yIndex][2]) {
    layer = 3; 
  } else if (zz <= zBound[xIndex][yIndex][4] &&
	     zz > zBound[xIndex][yIndex][3]) {
    layer = 4;
  } else if (zz > zBound[xIndex][yIndex][4]) {
    layer = 5;
  }

  //printf("layer : %d\n", layer);

  if ((layer < 0 || wedge < 0) && DEBUG) {
    printf(DMAGENTA "Messed up point: layer = %d, wedge = %d\n", layer, wedge);
    printf(" (x, y, z) = %0.2f %0.2f %0.2f\n", xx, yy, zz);

    printf("\n --- Layer determination details ---\n");
    printf("Z bounds: \n");
    for (Int_t i=0; i<6; i++) {
      printf("%d: %0.2f | ", i, zBound[xIndex][yIndex][i]);
    }
    printf("Point xIndex: %d, yIndex: %d, zz: %0.2f\n" RESET_COLOR, xIndex, yIndex, zz);

  } 
  Int_t segment = (layer*6) + wedge;
  
  return layer;
}
