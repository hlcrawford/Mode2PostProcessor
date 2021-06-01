#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <signal.h>

/* ROOT includes */
#include "TROOT.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TFile.h"
#include "TMath.h"
#include "TVector3.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TStyle.h"
#include "TRandom.h"
#include "TRandom3.h"

#include "colors.h"

#include "Globals.h"
#include "crystalGeometry.h"
#include "ControlParameters.h"

using namespace std;

#define DEBUG 0

/*************** Function prototypes ***************/

void PrintHelpInformation(char *argv[]);

int GetGRETINA(FILE *inf, Int_t lengthRemaining);
void SmearInteractionPoint(CrystalGeometry *crystal, TRandom3 *gRandom, Int_t intPointNum, Int_t sigmaX, 
			   Int_t sigmaY, Int_t sigmaZ, Int_t keepSameSeg);
void CombineInteractionPoints(Int_t intPointNum1, Int_t intPointNum2);
int GetBasisPts(Int_t crystal, TString basisFilename);
void AddCCResolution(TRandom3 *gRandom, Int_t crystalNum);
void AddIntPtResolution(TRandom3 *gRandom, Int_t crystalNum, Float_t ASeg, Float_t BSeg);
void SetSegmentNumbers(CrystalGeometry *crystal);
void AddInteractionPoint(TRandom3 *gRandom, Int_t xtal, Int_t segmentNum, 
			 Int_t eLimitLow, Int_t eLimitHigh);
void DeleteInteractionPoint(Int_t intPointNum);
void ConsolidateToSegmentCenters(Int_t crystal);
void PrintIntPts(Int_t num);
void CheckIntPts();
void BubbleSortIntPts();
Int_t ReadResolutions(TString resFilename);

long long int totalIntPts; 
long long int numRemoved; 
long long int numSegReassigned;
long long int numSegLayer;
long long int numSegWedge;
  
/*************** CTRL-C handling stuff ***************/

Int_t gotsignal;
void breakhandler(int dummy) {
  std::cout << "Got break signal.  Aborting cleanly..." << std::endl;
  gotsignal = 1;
}

/*************** Main data distortion function ***************/

int main(int argc, char *argv[]) {

  TRandom3 *gRandom = new TRandom3();
  gRandom->SetSeed(time(NULL));

  /* Some CTRL-C interrupt handling stuff... */
  gotsignal = 0; 
  signal(SIGINT, breakhandler);

  /* When insufficient arguments, print help information */
  if ( (argc<5) ) { PrintHelpInformation(argv); exit(1); }

  TString fileNameIn, fileNameOut, fileNameSet;
  Int_t inputFile = 0, outputFile = 0, setFile = 0;

  /* Initialize control variables -- what are we doing? */
  Int_t i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-fIn") == 0) {
      fileNameIn = argv[i+1]; i++; i++;
      inputFile = 1;
    } else if (strcmp(argv[i], "-fOut") == 0) {
      fileNameOut = argv[i+1]; i++; i++;
      outputFile = 1;
    } else if (strcmp(argv[i], "-fSet") == 0) {
      setFile = 1;
      fileNameSet = argv[i+1]; i++; i++;
    } else {
      cout << ALERTTEXT; 
      printf("Main(): Error -- unrecognized input flag: <%s>\n", argv[i]);
      cout << RESET_COLOR;  fflush(stdout);
      exit(-1);
    }
  }

  if (!inputFile || !outputFile) { 
    cerr << "Error -- missing argument for ";
    if (!inputFile && !outputFile) {
      cerr << "both input and output files. " << endl;
    } else if (!inputFile) {
      cerr << "input file. " << endl;
    } else if (!outputFile) {
      cerr << "output file. " << endl;
    }
    PrintHelpInformation(argv); exit(1);
  }

  FILE *inf, *outf;
 
  if (fileNameIn.Contains(".gz")) {
    fileNameIn = "zcat " + fileNameIn;
    inf = popen(fileNameIn.Data(), "r");
  } else {
    inf = fopen(fileNameIn.Data(), "r");
  }
  if (!inf) {
    printf("Cannot open: %s \n", fileNameIn.Data());
    exit(-1);
  } else {
    printf("Opened: %s for reading \n", fileNameIn.Data());
  }
  outf = fopen(fileNameOut.Data(), "w");

  controlParameters ctrl;
  ctrl.reset();
  if (setFile) {
    ctrl.getSettings(fileNameSet.Data());
  }

  //TString resFilename = "resolutions.dat";
  TString resFilename = ctrl.ccResFile;
  ReadResolutions(resFilename);

  /* Segment energy resolution parameters, for now */
  ctrl.ASeg = 0.3;  ctrl.BSeg = 0.24;

  /* Other data types - buffer for copying */
  UShort_t temp[8192];

  TGeoManager *geom = new TGeoManager("Crystals", "GRETINA Crystals"); 
  geom->SetVerboseLevel(0);
  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
  TGeoVolume *top = geom->MakeBox("TOP", Vacuum, 1000., 1000., 1000.);
  geom->SetTopVolume(top);

  CrystalGeometry *crystalGeo[2];
  crystalGeo[0] = new CrystalGeometry();
  crystalGeo[0]->BuildCrystal(0, top);
  crystalGeo[1] = new CrystalGeometry();
  crystalGeo[1]->BuildCrystal(1, top);
  geom->CloseGeometry();
  geom->SetVisLevel(4);
  top->Raytrace();

  crystalGeo[0]->GetBasisPoints("basis_rawB10.dat");
  crystalGeo[1]->GetBasisPoints("basis_rawA10.dat");
  crystalGeo[0]->WriteBasisSpectra("BasisB.root");
  crystalGeo[1]->WriteBasisSpectra("BasisA.root");

  Int_t siz = 0;
  Int_t lengthRemaining = 0;
  long long int remaining = 0;
  long long int bytes_read = 0; int MBread = 0;
  long long int bytes_read_since_last_time = 0;

  totalIntPts = 0;
  numRemoved = 0;
  numSegReassigned = 0;
  numSegLayer = 0;
  numSegWedge = 0;
  
  siz = fread(&gHeader, sizeof(struct globalHeader), 1, inf);
  lengthRemaining = gHeader.length;

  while ( siz && !gotsignal) {

    if (gHeader.type == DECOMP) {

      remaining = 1;

      while (remaining == 1) {

	remaining = GetGRETINA(inf, lengthRemaining);
	bytes_read += sizeof(struct mode2Struct);
	bytes_read_since_last_time += sizeof(struct mode2Struct);
	lengthRemaining -= sizeof(struct mode2Struct);
	
	/* Now, we'll actually mess with the data */
	int done = 0;

	/* Check CC threshold to see if we want to keep this crystal actually... */
	if (g2.tot_e > 50.0) {

	  //--------------- Clean up raw event first 
	  
	  /* Zero out all interaction points we don't expect to have... */
	  if (g2.num <= 16) {
	    for (Int_t m=g2.num; m<16; m++) {
	      if (g2.intpts[m].x != 0 || g2.intpts[m].y != 0 || g2.intpts[m].z != 0 ||
		  g2.intpts[m].e != 0 || g2.intpts[m].seg != 0 || g2.intpts[m].seg_energy != 0) {
		if (DEBUG) { 
		  printf(DGREEN "DEBUG: Unexpected data in intpts array! Expected: %d\n", g2.num);
		  PrintIntPts(15); 
		  cout << RESET_COLOR << endl; 
		}
		g2.intpts[m].x = 0;
		g2.intpts[m].y = 0;
		g2.intpts[m].z = 0;
		g2.intpts[m].e = 0;
		g2.intpts[m].seg = 0;
		g2.intpts[m].seg_energy = 0;
	      }
	    }
	  } else {
	    printf(DMAGENTA "Error - more than 16 interaction points in initial data.\n");
	    printf("Truncating to only 16...\n" RESET_COLOR);
	    g2.num = 16;
	    CheckIntPts();
	  }
	  
	  totalIntPts += g2.num;
	  
	  BubbleSortIntPts();
	  
	  if (DEBUG) {
	    printf(DCYAN "DEBUG: Initial status after bubble sort\n" RESET_COLOR);
	    PrintIntPts(g2.num);
	  }

	  //--------------- Figure out crystal, and do energy resolution stuff (simple for now)
	  
	  /* Figure out the crystal type... */
	  Int_t ct = g2.crystal_id%2; /* crystal_id from 0 to 119, BType ct=0, AType ct=1 */
	  if (DEBUG) {
	    printf(DCYAN "DEBUG: Hole number: %d\n", g2.crystal_id/4);
	    printf("DEBUG: Crystal %d, type %d\n" RESET_COLOR, g2.crystal_id, ct);
	  }
	  
	  /* Just smear CC energy resolution. */
	  if (ctrl.ccResolution) {
	    AddCCResolution(gRandom, g2.crystal_id);
	    if (DEBUG) {
	      printf(DCYAN "DEBUG: Did CC resolution smearing\n" RESET_COLOR);
	      PrintIntPts(g2.num);
	    }
	  }
	  
	  if (ctrl.segResolution && g2.num > 0)  {
	    AddIntPtResolution(gRandom, g2.crystal_id, ctrl.ASeg, ctrl.BSeg);
	    if (DEBUG) {
	      printf(DCYAN "DEBUG: Did interaction point resolution smearing\n" RESET_COLOR);
	      PrintIntPts(g2.num);
	    }
	  }

	  //--------------- Clean up interaction points that are outside the crystal physical limits
	  
	  for (Int_t m=0; m<g2.num; m++) {
	    if (!crystalGeo[(ct)]->IsInside(g2.intpts[m].x, g2.intpts[m].y, g2.intpts[m].z)) {
	      if  (crystalGeo[(ct)]->DistanceToEdge(g2.intpts[m].x, g2.intpts[m].y, g2.intpts[m].z) > 0.2) {
	   	numRemoved++;
	   	if (DEBUG) {
	   	  printf(DCYAN "-- Point outside crystal removed...\n");
	   	  printf("   Data %d: (%0.3f, %0.3f, %0.3f): %d:  %0.3f\n" RESET_COLOR, ct, g2.intpts[m].x, g2.intpts[m].y, 
	   		 g2.intpts[m].z, !crystalGeo[(ct)]->IsInside(g2.intpts[m].x, g2.intpts[m].y, g2.intpts[m].z),
	   		 crystalGeo[(ct)]->DistanceToEdge(g2.intpts[m].x, g2.intpts[m].y, g2.intpts[m].z));
		}
	   	DeleteInteractionPoint(m); 
	   	m = -1;
	      }
	    }
	  }

	  /* Reset the segment numbers according to basis segment limits */
	  /* This needs to be checked -- running on data seems to change segments, which is weird */
	  if (ctrl.useRealSegments) {
	    SetSegmentNumbers(crystalGeo[ct]);
	  }
	    
	  if (DEBUG) {
	    printf(DCYAN "DEBUG: After segment reassignments...\n" RESET_COLOR);
	    PrintIntPts(g2.num);
	  }

	  //--------------- Now add position resolutions...
	  
	  /* Add x,y and z Gaussian position resolutions */  
	  
	  if (ctrl.smearPosition && g2.num > 0) {
	    if (!ctrl.highEdifferent && !ctrl.eDependent) { /* Uniform position smearing */
	      for (Int_t intNum=0; intNum < g2.num; intNum++) {
	   	Float_t temp[3] = {g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z};
	   	//while(!crystalGeo[(ct)]->IsInside(g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z)) { 
		//g2.intpts[intNum].x = temp[0];
		//g2.intpts[intNum].y = temp[1];
		//g2.intpts[intNum].z = temp[2];
		//printf("Smearing...\n");
	   	  SmearInteractionPoint(crystalGeo[ct], gRandom, intNum, ctrl.sigmaX[0], ctrl.sigmaY[0], ctrl.sigmaZ[0], 0);
		  //}
	      } 
	    } else if (ctrl.highEdifferent && !ctrl.eDependent) { /* Treat the high E point differently in terms of sigma */
	      for (Int_t intNum=0; intNum < g2.num; intNum++) {
	   	Float_t temp[3] = {g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z};
	   	while(!crystalGeo[(ct)]->IsInside(g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z)) { 
	   	  g2.intpts[intNum].x = temp[0];
	   	  g2.intpts[intNum].y = temp[1];
	   	  g2.intpts[intNum].z = temp[2];
	   	  if (intNum == 0) {
	   	    SmearInteractionPoint(crystalGeo[ct], gRandom, intNum, ctrl.sigmaX[0], ctrl.sigmaY[0], ctrl.sigmaZ[0], 0);
	   	  } else {
	   	    SmearInteractionPoint(crystalGeo[ct], gRandom, intNum, ctrl.sigmaX[1], ctrl.sigmaY[1], ctrl.sigmaZ[1], 0);
	   	  }
	   	}
	      } 
	    } else if (ctrl.eDependent) {
	      
	      /* This follows equation (10) of Soderstrom et al., NIMA 638, 96 (2011), from AGATA:
	   	 -->  sigma(E) = A*(Sqrt(1/E)) + B, 
	   	 where we calculate A and B based on sigmas at 100keV 
	   	 and 1MeV in the input file                                     	      */
	      
	      for (Int_t intNum=0; intNum < g2.num; intNum++) {
	   	Float_t temp[3] = {g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z};
	   	g2.intpts[intNum].x = -100;
	   	while(!crystalGeo[ct]->IsInside(g2.intpts[intNum].x, g2.intpts[intNum].y, g2.intpts[intNum].z) && 
	   	      g2.intpts[intNum].e > 0.0) {
	   	  g2.intpts[intNum].x = temp[0];
	   	  g2.intpts[intNum].y = temp[1];
	   	  g2.intpts[intNum].z = temp[2];
	  	  
	   	  if (DEBUG) {
	   	    printf(DCYAN "Position sigmas for E = %0.3f: %0.3f, %0.3f, %0.3f\n" RESET_COLOR,
	   		   g2.intpts[intNum].e, ctrl.Ax*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.Bx,
	   		   ctrl.Ay*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.By,
	   		   ctrl.Az*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.Bz);
		  }
	   	  SmearInteractionPoint(crystalGeo[ct], gRandom, intNum, 
	   				ctrl.Ax*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.Bx,
	   				ctrl.Ay*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.By,
	   				ctrl.Az*(TMath::Sqrt(1/g2.intpts[intNum].e)) + ctrl.Bz, 0);
	   	}
	      } 
	    }
	    if (DEBUG) {
	      printf(DCYAN "DEBUG: Finished with smearPosition loop \n" RESET_COLOR);
	      PrintIntPts(g2.num);
	    }
	  }

	  //--------------- Combine any closely spaced interaction points
	  
	  /* Combine closely spaced interaction points (< 2mm separation) */
	  if (ctrl.combinePoints && g2.num > 1) {
	    for (Int_t int1=0; int1<g2.num; int1++) {
	      if (g2.num > 1) {
	   	for (Int_t int2=int1+1; int2<g2.num; int2++) {
		  
	   	  TVector3 separation((g2.intpts[int1].x - g2.intpts[int2].x),
	   			      (g2.intpts[int1].y - g2.intpts[int2].y),
	   			      (g2.intpts[int1].z - g2.intpts[int2].z));
	  	  
	   	  if (DEBUG) {
	   	    printf(DCYAN "DEBUG: %d-%d separation = %0.3f\n" RESET_COLOR, int1, int2, separation.Mag());
	   	  }

		  if (separation.Mag() < ctrl.coalescenceD) { 
		    if (ctrl.sameSegOnly && (g2.intpts[int1].seg == g2.intpts[int2].seg)) {
		      CombineInteractionPoints(int1, int2);
	   	      if (DEBUG) {
	   		printf(DBLUE "DEBUG: Did CombinePoints\n" RESET_COLOR);
			PrintIntPts(g2.num);
	   	      }
	   	      int1 = 0; int2 = int1;		 
	   	    } else if (!ctrl.sameSegOnly) {
		      CombineInteractionPoints(int1, int2);
	   	      /* Possibly different segments -- check where it ends up. */
		      if (ctrl.useRealSegments) {
			SetSegmentNumbers(crystalGeo[ct]);
		      }
	   	      if (DEBUG) {
	   		printf(DBLUE "DEBUG: Did CombinePoints\n" RESET_COLOR);
	   		PrintIntPts(g2.num);
	   	      }
	   	      int1 = 0; int2 = int1; 
	   	    }
	   	  }
		  
	   	}
	      }
	    }
	    if (DEBUG) {
	      printf(DCYAN "DEBUG: Finished with combinePoints loop \n" RESET_COLOR);
	      PrintIntPts(g2.num);
	    }
	  }

	  //--------------- Quite an artificial thing, but for high mult. segments, move low E to the edge
	  
	  /* Move low-energy interactions in segments with multiple
	     interaction points to the edge of the segments. */
	  if (ctrl.moveLowE && g2.num > 0) {
	    if (g2.num > ctrl.minSegMult) {
	      Int_t low=-1; Double_t lowE=12000;
	      for (Int_t intNum=0; intNum < g2.num; intNum++) {
	   	if (g2.intpts[intNum].e < lowE) {
	   	  lowE = g2.intpts[intNum].e; 
	   	  low = intNum;
	   	}
	      }
	      if (lowE < ctrl.maxE2Move) {
	   	TVector3 edgePos = crystalGeo[ct]->GetNewInteractionPosition(gRandom, g2.intpts[low].seg);
	   	g2.intpts[low].x = edgePos.X();
	   	g2.intpts[low].y = edgePos.Y();
	   	g2.intpts[low].z = edgePos.Z();
	      }
	    }
	    if (DEBUG) {
	      printf(DCYAN "DEBUG: Finished with moveLowE loop\n" RESET_COLOR);
	      PrintIntPts(g2.num);
	    }
	  }
	  
	  CheckIntPts();
	  
	  if (DEBUG) { 
	    printf(DCYAN "DEBUG: Finished!  Here's what will go out.\n" RESET_COLOR);
	    PrintIntPts(g2.num);
	    printf(DCYAN "************************\n\n" RESET_COLOR);
	  }
	  
	  fwrite(&gHeader, 1, sizeof(struct globalHeader), outf);
	  fwrite(&g2, 1, sizeof(struct mode2Struct), outf);
	}
	
      }

    } else if (gHeader.type != DECOMP) { /* Blind copy anything not Mode2 */

      Int_t siz = fread(temp, 1, gHeader.length, inf);
      if (siz != gHeader.length) {
	std::cerr << "CopyData failed in bytes read." << std::endl;
      }

      bytes_read += gHeader.length;
      bytes_read_since_last_time += gHeader.length;
      fwrite(&gHeader, 1, sizeof(struct globalHeader), outf);
      fwrite(temp, 1, gHeader.length, outf);           
    }
    
    if (bytes_read_since_last_time > 2*1024*1024) {
      std::cout << "Processing " <<  bytes_read/(1024*1024) << " MB" << "\r";
      bytes_read_since_last_time = 0;
      MBread += 2;
    }

    siz = fread(&gHeader, sizeof(struct globalHeader), 1, inf);
    lengthRemaining = gHeader.length;

  } /* End of "while we still have data and not interrupt signal" */

    printf("Total: %lld \n", totalIntPts);
    printf("Deleted: %lld (%0.2f%%)\n", numRemoved, (Float_t)(numRemoved)*100./(Float_t)(totalIntPts));
    printf("Reassigned: %lld (%0.2f%%)\n", numSegReassigned, (Float_t)(numSegReassigned)*100./(Float_t)(totalIntPts));
    printf("  Changed by 1 layer: %lld\n", numSegLayer);
    printf("  Changed by 1 wedge: %lld\n", numSegWedge);

    printf("Filesize read: %lld MB\n", bytes_read/(1024*1024));
    
  return(1);

}

int GetGRETINA(FILE *inf, Int_t lengthRemaining) {
  
  Int_t siz = 0;
  
  if (gHeader.type == DECOMP) {
    /* We will assume the presence of global headers,
       and mode2 struct version 0xabcd5678. */
    siz = fread(&g2, sizeof(struct mode2Struct), 1, inf);
    
    if (siz != 1) {
      cout << ALERTTEXT;
      printf("GetGRETINA(): Failed in bytes read.\n");
      cout << RESET_COLOR;  fflush(stdout);
      raise(SIGINT);
    } 
    
    if (lengthRemaining > (int)sizeof(struct mode2Struct)) { return 1; }
    else { return 0; }
  }
  
  return 0;
  
}

/*************** IP manipulation functions ***************/

void SetSegmentNumbers(CrystalGeometry *crystal) {
  for (int i=0; i<MAX_INTPTS; i++) {
    if (g2.intpts[i].e > 0) {
      Int_t newSeg = crystal->GetSegmentNumber(g2.intpts[i].x, g2.intpts[i].y, g2.intpts[i].z);
      newSeg *=6;
      newSeg += g2.intpts[i].seg%6;
      if (g2.intpts[i].seg != newSeg) {
	if (DEBUG) { 
	  cout << "New segment for " << i << ": was " << g2.intpts[i].seg 
	       << ", now " << newSeg << endl;
	}
	if (TMath::Abs(newSeg - g2.intpts[i].seg) == 6) { numSegLayer++; }
	else if (TMath::Abs(newSeg - g2.intpts[i].seg) == 1 || TMath::Abs(newSeg - g2.intpts[i].seg) == 5) { numSegWedge++; }
	  
	g2.intpts[i].seg = newSeg;
	numSegReassigned++;

      }
    }
  }
  
  for (Int_t m=0; m<g2.num; m++) {
    g2.intpts[m].seg_energy = g2.intpts[m].e;
    for (Int_t n=0; n<g2.num; n++) {
      if (n != m && g2.intpts[m].seg == g2.intpts[n].seg) {
	g2.intpts[m].seg_energy += g2.intpts[n].e;
      }
    }
  }

  BubbleSortIntPts();

};

void AddCCResolution(TRandom3 *gRandom, Int_t crystalNum){
  //Float_t sigma = resA[(crystalNum-1)]*TMath::Power(g2.tot_e, resB[(crystalNum-1)]);
  //Float_t sigma = g2.tot_e*0.01/2.35;
  Float_t sigma = std::sqrt(resA[crystalNum-1]+resB[crystalNum-1]*g2.tot_e);
  g2.tot_e += gRandom->Gaus(0, sigma);
};

void AddIntPtResolution(TRandom3 *gRandom, Int_t crystalNum, Float_t ASeg, Float_t BSeg){
  for (Int_t i=0; i<MAX_INTPTS; i++) {
    if (g2.intpts[i].e > 0) {
      Float_t sigma = ASeg*TMath::Power(g2.intpts[i].e, BSeg);
      g2.intpts[i].e += gRandom->Gaus(0, sigma);
    }
  }

  BubbleSortIntPts();

};

void SmearInteractionPoint(CrystalGeometry *crystal, TRandom3 *gRandom, Int_t intPointNum, Int_t sigmaX,
			   Int_t sigmaY, Int_t sigmaZ, Int_t keepSameSeg = 1) {

  Double_t xOrig = g2.intpts[intPointNum].x;
  Double_t yOrig = g2.intpts[intPointNum].y;
  Double_t zOrig = g2.intpts[intPointNum].z;
  
  g2.intpts[intPointNum].x += gRandom->Gaus(0, sigmaX);
  g2.intpts[intPointNum].y += gRandom->Gaus(0, sigmaY);
  g2.intpts[intPointNum].z += gRandom->Gaus(0, sigmaZ);

  //printf("%f %f %f --> %f %f %f\n", xOrig, yOrig, zOrig, g2.intpts[intPointNum].x, g2.intpts[intPointNum].y, g2.intpts[intPointNum].z);
  
  if (keepSameSeg) { 
  /* Check we've maintained the same segment ID --> shouldn't smear out of the
     segment.  So just pull back in toward the original point along the displacement
     vector until we're back inside. */
    Int_t newSeg = crystal->GetSegmentNumber(g2.intpts[intPointNum].x, g2.intpts[intPointNum].y, g2.intpts[intPointNum].z);
    
    if (newSeg != g2.intpts[intPointNum].seg) {
      TVector3 *radial = new TVector3();
      radial->SetXYZ(g2.intpts[intPointNum].x - xOrig, g2.intpts[intPointNum].y - yOrig, g2.intpts[intPointNum].z - zOrig);
      
      Int_t GOOD = 0;
      
      while (!GOOD) {
	radial->SetMag(radial->Mag()*0.95);
	g2.intpts[intPointNum].x = xOrig + radial->X();
	g2.intpts[intPointNum].y = yOrig + radial->Y();
	g2.intpts[intPointNum].z = zOrig + radial->Z();
	newSeg = crystal->GetSegmentNumber(g2.intpts[intPointNum].x, g2.intpts[intPointNum].y, g2.intpts[intPointNum].z);
	if (newSeg == g2.intpts[intPointNum].seg) { GOOD = 1; }
      } 
    }
  }
 
}

void BubbleSortIntPts() {
  for (int i=1; i<MAX_INTPTS; i++) {
    if (g2.intpts[i].e > g2.intpts[i-1].e) {
      ip *temp = new ip();
      temp->e = g2.intpts[i-1].e;
      temp->x = g2.intpts[i-1].x;
      temp->y = g2.intpts[i-1].y;
      temp->z = g2.intpts[i-1].z;
      temp->seg = g2.intpts[i-1].seg;
      temp->seg_energy = g2.intpts[i-1].seg_energy;
      g2.intpts[i-1].e = g2.intpts[i].e;
      g2.intpts[i-1].x = g2.intpts[i].x;
      g2.intpts[i-1].y = g2.intpts[i].y;
      g2.intpts[i-1].z = g2.intpts[i].z;
      g2.intpts[i-1].seg = g2.intpts[i].seg;
      g2.intpts[i-1].seg_energy = g2.intpts[i].seg_energy;
      g2.intpts[i].e = temp->e;
      g2.intpts[i].x = temp->x;
      g2.intpts[i].y = temp->y;
      g2.intpts[i].z = temp->z;
      g2.intpts[i].seg = temp->seg;
      g2.intpts[i].seg_energy = temp->seg_energy;
      i = 0;
    }
  }    
}

void DeleteInteractionPoint(Int_t intPointNum) {

  g2.intpts[intPointNum].e = 0.0;
  g2.intpts[intPointNum].x = 0.0;
  g2.intpts[intPointNum].y = 0.0;
  g2.intpts[intPointNum].z = 0.0;
  g2.intpts[intPointNum].seg = 0.0;
  g2.intpts[intPointNum].seg_energy = 0.0;

  g2.num -= 1;

  BubbleSortIntPts();

};

void InsertInteractionPoint(ip *ipNew) {

  g2.intpts[g2.num].e = ipNew->e;
  g2.intpts[g2.num].x = ipNew->x;
  g2.intpts[g2.num].y = ipNew->y;
  g2.intpts[g2.num].z = ipNew->z;
  g2.intpts[g2.num].seg = ipNew->seg;
  g2.intpts[g2.num].seg_energy = ipNew->seg_energy;

  g2.num++;

};

void SplitInteractionPoint(Int_t intPointNum, Double_t averageSep, 
			   Double_t sigmaSep, Double_t highFractionE) {
  
  ip *ipNew = new ip();
  ip *ipNew2 = new ip();
  
  ipNew->e = g2.intpts[intPointNum].e * highFractionE;
  ipNew2->e = g2.intpts[intPointNum].e * (1 - highFractionE);

};

void CombineInteractionPoints(Int_t intPointNum1, Int_t intPointNum2) {

  ip *ipNew = new ip();
  ipNew->e = g2.intpts[intPointNum1].e + g2.intpts[intPointNum2].e;
  ipNew->x = (1/ipNew->e)*(g2.intpts[intPointNum1].x*g2.intpts[intPointNum1].e +
		g2.intpts[intPointNum2].x*g2.intpts[intPointNum2].e);
  ipNew->y = (1/ipNew->e)*(g2.intpts[intPointNum1].y*g2.intpts[intPointNum1].e +
		g2.intpts[intPointNum2].y*g2.intpts[intPointNum2].e);
  ipNew->z = (1/ipNew->e)*(g2.intpts[intPointNum1].z*g2.intpts[intPointNum1].e +
		g2.intpts[intPointNum2].z*g2.intpts[intPointNum2].e);
  ipNew->seg = g2.intpts[intPointNum1].seg;
  ipNew->seg_energy = g2.intpts[intPointNum1].seg_energy;
  
  if (DEBUG) {
    cout << "Combining " << intPointNum1 << " and " << intPointNum2 << endl;
    cout << endl;
  }

  DeleteInteractionPoint(intPointNum1);
  DeleteInteractionPoint(intPointNum2);
  InsertInteractionPoint(ipNew);

  BubbleSortIntPts();
 
};

void SeparateInteractionPoints(Int_t intPointNum1, Int_t intPointNum2, Float_t factor) {

  TVector3 v;
  v.SetXYZ(g2.intpts[intPointNum1].x - g2.intpts[intPointNum2].x,
	   g2.intpts[intPointNum1].y - g2.intpts[intPointNum2].y,
	   g2.intpts[intPointNum1].z - g2.intpts[intPointNum2].z);
  
  Double_t x = v.X();  Double_t y = v.Y();  Double_t z = v.Z();
  Double_t r = v.Mag();
  
  /* Change the separation between interaction points */
  r *= factor;

  v.SetMag(r);

  /* Calculate change in coordinates */
  Double_t deltaX = x - v.X();
  Double_t deltaY = y - v.Y();
  Double_t deltaZ = z - v.Z();
  
  /* Shift points, preserving center of gravity between them */
  Double_t eSum = g2.intpts[intPointNum1].e + g2.intpts[intPointNum2].e;
  g2.intpts[intPointNum1].x -= (g2.intpts[intPointNum2].e/eSum)*deltaX;
  g2.intpts[intPointNum1].y -= (g2.intpts[intPointNum2].e/eSum)*deltaY;
  g2.intpts[intPointNum1].z -= (g2.intpts[intPointNum2].e/eSum)*deltaZ;
  g2.intpts[intPointNum2].x += (g2.intpts[intPointNum1].e/eSum)*deltaX;
  g2.intpts[intPointNum2].y += (g2.intpts[intPointNum1].e/eSum)*deltaY;
  g2.intpts[intPointNum2].z += (g2.intpts[intPointNum1].e/eSum)*deltaZ;

};

void AddInteractionPoint(TVector3 pos, TRandom3 *gRandom, Int_t xtal, Int_t segmentNum, 
			 Int_t eLimitLow, Int_t eLimitHigh) {

  ip *ipNew = new ip();

  ipNew->e = (gRandom->Rndm()*(eLimitHigh - eLimitLow)) + eLimitLow;
  ipNew->x = pos.X(); ipNew->y = pos.Y(); ipNew->z = pos.Z();
  ipNew->seg = segmentNum;

  /* Adjust other interaction point energies so that total is still good. */
  for (int j=0; j<MAX_INTPTS; j++) {
    g2.intpts[j].e *= ((g2.tot_e - ipNew->e)/(g2.tot_e));
  }
  g2.tot_e -= ipNew->e;

  InsertInteractionPoint(ipNew);

  BubbleSortIntPts();

};

void CheckIntPts() {

  BubbleSortIntPts();

  Float_t segE[36] = {0};

  for (Int_t i=0; i<g2.num; i++) {
    segE[g2.intpts[i].seg] += g2.intpts[i].e;
  }

  for (Int_t i=0; i<g2.num; i++) {
    g2.intpts[i].seg_energy = segE[g2.intpts[i].seg];
  }

}

void ConsolidateToSegmentCenters(Int_t ct) {

  if (g2.num > 1) {
    Int_t segsHit[36] = {0}; Int_t segmentsHit = 0;
    
    for (Int_t j=0; j<g2.num; j++) {
      if (g2.intpts[j].e > 0) {
	if (segsHit[g2.intpts[j].seg] == 0) { segmentsHit++; }
	segsHit[g2.intpts[j].seg] = 1;
      }
    }
    
    Int_t segmentHit[36] = {0};
    
    Int_t j = 0;
    while (j < segmentsHit) {
      for (Int_t i=0; i<36; i++) {
	if (segsHit[i] == 1) {
	  segmentHit[j] = i;
	  segsHit[i] = 0;
	  j++;
	}
      }
    }
    
    if (DEBUG) {
      cout << "Consolidating - " << segmentsHit << " segments hit. " << endl;
      cout << "   Segments: " << segmentHit[0] << endl;
      Int_t d = 1;
      while (d < segmentsHit) {
	cout << "             " << segmentHit[d] << endl;
	d++;
      }
    }
    
    for (Int_t n=0; n<segmentsHit; n++) {
      ip *ipNew = new ip();
      for (Int_t m=0; m<g2.num; m++) {
	if (g2.intpts[m].seg == segmentHit[n]) {
	  ipNew->e = g2.intpts[m].seg_energy;
	  ipNew->seg = g2.intpts[m].seg;
	  DeleteInteractionPoint(m);
	  m = -1;
	}
      }
      ipNew->seg_energy = ipNew->e;
      if (ct == 0) {
	ipNew->x = xB[ipNew->seg];
	ipNew->y = yB[ipNew->seg];
	ipNew->z = zB[ipNew->seg];
      } else if (ct == 1) {
	ipNew->x = xA[ipNew->seg];
	ipNew->y = yA[ipNew->seg];
	ipNew->z = zA[ipNew->seg];
      }
      InsertInteractionPoint(ipNew);
    }
  } else {
    if (ct == 0) {
      g2.intpts[0].x = xB[(Int_t)(g2.intpts[0].seg)];
      g2.intpts[0].y = yB[(Int_t)(g2.intpts[0].seg)];
      g2.intpts[0].z = zB[(Int_t)(g2.intpts[0].seg)];
    } else if (ct == 1) {
      g2.intpts[0].x = xA[(Int_t)(g2.intpts[0].seg)];
      g2.intpts[0].y = yA[(Int_t)(g2.intpts[0].seg)];
      g2.intpts[0].z = zA[(Int_t)(g2.intpts[0].seg)];
    }
  }

  BubbleSortIntPts();
  
};

void PrintHelpInformation(char *argv[]) {
  cout << "Usage: " << argv[0] << " <usageFlags> -fIn <inputFileName> -fOut <outputFileName> " << endl;
  cout << "    Valid usage flags: -fSet <setFileName>" << endl;
};

void PrintIntPts(Int_t num) {
  printf("CrystalID: %d, CCenergy = %0.3f, #intpts = %d\n",
	 g2.crystal_id, g2.tot_e, g2.num);
  if (num < 15) {
    for (int b=0; b<num+1; b++) {
      printf("%d: (%0.3f, %0.3f, %0.3f), e = %0.3f, seg# = %d, segE = %0.3f\n",
	     b, g2.intpts[b].x, g2.intpts[b].y, g2.intpts[b].z,
	     g2.intpts[b].e, g2.intpts[b].seg, g2.intpts[b].seg_energy);
    }
  } else {
    for (int b=0; b<16; b++) {
      printf("%d: (%0.3f, %0.3f, %0.3f), e = %0.3f, seg# = %d, segE = %0.3f\n",
	     b, g2.intpts[b].x, g2.intpts[b].y, g2.intpts[b].z,
	     g2.intpts[b].e, g2.intpts[b].seg, g2.intpts[b].seg_energy);
    }
  }
    
  printf("\n");
};

Int_t ReadResolutions(TString resFilename) {

  FILE *resf;
  char *st, str[200];
  Int_t i1; Float_t f1, f2;

  resf = fopen(resFilename.Data(), "r");
  if (resf == NULL) {
    cerr << "Could not open \"" << resFilename.Data() << "\"." << endl;
    exit(1);
  } 
  cout << "\"" << resFilename.Data() << "\" open... " << endl;

  Int_t nn = 0;
  st = fgets(str, 200, resf);
  while (st != NULL) {
    if (str[0] == 35) {
      /* '#' comment line, do nothing */
    } else if (str[0] == 59) {
      /* ';' comment line, do nothing */
    } else if (str[0] == 10) {
      /* Empty line, do nothing */
    } else {
      sscanf(str, "%i %f %f", &i1, &f1, &f2);
      if (i1 >= 0 && i1 <=120) {
	resA[(i1-1)] = f1;
	resB[(i1-1)] = f2;
      }
      nn++;
    }

    /* Attempt to read the next line... */
    st = fgets(str, 200, resf);
  }
  cout << "Read resolutions parameters for " << nn << " crystals. " << endl;

  /* Done! */
  fclose(resf);
  return (0);
}
