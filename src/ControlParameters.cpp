#include <stdlib.h>

#include "ControlParameters.h"

#include "TMath.h"

using std::cout;

ClassImp(controlParameters);

controlParameters::controlParameters() {
  reset();
}

void controlParameters::reset() {
  smearPosition = 1;  highEdifferent = 0;
  useRealSegments = 1;
  sigmaX[0] = 2.0;  sigmaX[1] = 2.0;
  sigmaY[0] = 2.0;  sigmaY[1] = 2.0;
  sigmaZ[0] = 2.0;  sigmaZ[1] = 2.0;
  
  Ax = 0.0; Bx = 0.0;
  Ay = 0.0; By = 0.0;
  Az = 0.0; Bz = 0.0;

  ASeg = 0.3;  BSeg = 0.24;

  ccResolution = 1;
  ccResFile = "resolutions.dat";
  
  segResolution = 0;
  segResFile = "";

  combinePoints = 0;  sameSegOnly = 1;
  coalescenceD = 2.0;
  
  moveLowE = 0;
  minSegMult = 3;
  maxE2Move = 100.;
}

Int_t controlParameters::getSettings(TString fileName) {

  FILE *fin;

  char *p, *line, str[512] = {'0'}, str1[512] = {'0'}, str2[512] = {'0'}; 
  
  Int_t nn = 0, nret = 0;
  Int_t d1 = 0, d2 = 0, d3 = 0;
  Float_t f1 = 0., f2 = 0., f3 = 0.;

  if ((fin = fopen(fileName.Data(), "r")) == NULL) {
    cout << ALERTTEXT;
    printf("getSettings(): Could not open settings file \"%s\"\n", fileName.Data());
    cout << RESET_COLOR;  fflush(stdout);
    return (-1);
  } else {
    printf("getSettings(): Opened file \"%s\"\n",  fileName.Data());
    fflush(stdout);
  }

  line = fgets(str, 512, fin);
  while (line != NULL) {
    /* Interpret the line */
    if (str[0] == 35 || str[0] == 59 || str[0] == 10) {
      /* Do nothing, comment lines or empty lines */
    } else if ((p = strstr(str, "useRealSegments")) != NULL) {
      nret = sscanf(str, "%s %d", str1, &d1);
      useRealSegments = d1;
      if (useRealSegments) {
	printf(DCYAN "  Using realistic segment boundaries...\n" RESET_COLOR);
      }
    } else if ((p = strstr(str, "smearPosition")) != NULL) {
      nret = sscanf(str, "%s %d %d %d", str1, &d1, &d2, &d3);
      smearPosition = d1;  highEdifferent = d2; eDependent = d3;
      line = fgets(str, 512, fin);
      nret = sscanf(str, "%d %f %f %f", &d1, &f1, &f2, &f3);
      sigmaX[d1-1] = f1;  sigmaY[d1-1] = f2;  sigmaZ[d1-1] = f3;
      if (highEdifferent || eDependent) {
	line = fgets(str, 512, fin);
	nret = sscanf(str, "%d %f %f %f", &d1, &f1, &f2, &f3);
	sigmaX[d1-1] = f1;  sigmaY[d1-1] = f2;  sigmaZ[d1-1] = f3;
      }
      if (highEdifferent && eDependent) {
	cout << ALERTTEXT;
	printf("getSettings(): Please use either position smearing with first interaction (highE) different, or the energy dependent formalism, not both.\n");
	cout << RESET_COLOR;  fflush(stdout);
	return (-1);
      }

      if (eDependent) {
	Ax = ((sigmaX[0] - sigmaX[1])/((1/TMath::Sqrt(100)) - (1/TMath::Sqrt(1000)))); 
	Ay = ((sigmaY[0] - sigmaY[1])/((1/TMath::Sqrt(100)) - (1/TMath::Sqrt(1000)))); 
	Az = ((sigmaZ[0] - sigmaZ[1])/((1/TMath::Sqrt(100)) - (1/TMath::Sqrt(1000)))); 
	Bx = (sigmaX[0] - (Ax/TMath::Sqrt(100))); 
	By = (sigmaY[0] - (Ay/TMath::Sqrt(100)));
	Bz = (sigmaZ[0] - (Az/TMath::Sqrt(100)));
      }

      printf(DCYAN "  Doing position resolution smearing with");
      if (!highEdifferent && !eDependent) {
	printf(" uniform sigmas: %0.2f mm, %0.2f mm, %0.2f mm\n" RESET_COLOR,
	       sigmaX[0], sigmaY[0], sigmaZ[0]);
      } else if (highEdifferent) {
	printf(" different highest E position resolution.\n");
	printf("    (1): %0.2f mm, %0.2f mm, %0.2f mm\n" RESET_COLOR,
	       sigmaX[0], sigmaY[0], sigmaZ[0]);
	printf("    (2): %0.2f mm, %0.2f mm, %0.2f mm\n" RESET_COLOR,
	       sigmaX[1], sigmaY[1], sigmaZ[1]);
      } else if (eDependent) {
	printf(" energy-dependent position resolution.\n" RESET_COLOR);
	printf("    (1): %0.2f mm, %0.2f mm, %0.2f mm\n" RESET_COLOR,
	       sigmaX[0], sigmaY[0], sigmaZ[0]);
	printf("    (2): %0.2f mm, %0.2f mm, %0.2f mm\n" RESET_COLOR,
	       sigmaX[1], sigmaY[1], sigmaZ[1]);
      }

    } else if ((p = strstr(str, "combinePoints")) != NULL) {
      nret = sscanf(str, "%s %d %f", str1, &d1, &f1);
      combinePoints = 1;  
      sameSegOnly = d1;  coalescenceD = f1;

      printf(DCYAN "  Combining points with coalescence distance of %0.2f mm (same segment = %d)\n" RESET_COLOR,
	     coalescenceD, sameSegOnly);
    } else if ((p = strstr(str, "moveLowE2Boundary")) != NULL) {
      nret = sscanf(str, "%s %d %f", str1, &d1, &f1);
      moveLowE = 1;
      minSegMult = d1;  maxE2Move = f1;
    } else if ((p = strstr(str, "ccResolutions")) != NULL) {
      nret = sscanf(str, "%s %s", str1, str2);
      ccResolution = 1;
      ccResFile = str2;
    } else if ((p = strstr(str, "segResolutions")) != NULL) {
      nret = sscanf(str, "%s %s", str1, str2);
      segResolution = 1;
      segResFile = str2;
    } else {
      /* Unknown command */
      cout << ALERTTEXT;
      printf("getSettings(): Unknown command: \"%s\"\n", str);
      printf("               -----> Aborting!!\n");
      cout << RESET_COLOR;  fflush(stdout);
      return (-1);
    }

    nn++;  line = fgets(str, 512, fin);
  }

  fclose(fin);
  printf("getSettings(): Settings file closed.\n");
  fflush(stdout);

  return (0);

}
